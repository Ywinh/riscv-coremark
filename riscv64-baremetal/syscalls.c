#include <limits.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/signal.h>
#include "util.h"

#define SYS_write 64
#define SYS_exit 93

static long do_syscall3(long which, long arg0, long arg1, long arg2)
{
  register long a0 asm("a0") = arg0;
  register long a1 asm("a1") = arg1;
  register long a2 asm("a2") = arg2;
  register long a7 asm("a7") = which;

  asm volatile ("ecall"
      : "+r"(a0)
      : "r"(a1), "r"(a2), "r"(a7)
      : "memory");

  return a0;
}

static long do_syscall1(long which, long arg0)
{
  register long a0 asm("a0") = arg0;
  register long a7 asm("a7") = which;

  asm volatile ("ecall"
      : "+r"(a0)
      : "r"(a7)
      : "memory");

  return a0;
}

static char stdout_buf[64] __attribute__((aligned(64)));
static int stdout_len;

static void flush_stdout(void)
{
  if (stdout_len == 0)
    return;

  do_syscall3(SYS_write, 1, (uintptr_t)stdout_buf, stdout_len);
  stdout_len = 0;
}

void setStats(int enable)
{
  (void)enable;
}

void exit(int code)
{
  flush_stdout();
  do_syscall1(SYS_exit, code);
  while (1)
    asm volatile ("wfi");
}

void abort(void)
{
  exit(128 + SIGABRT);
}

size_t strlen(const char *s)
{
  const char *p = s;
  while (*p)
    p++;
  return p - s;
}

size_t strnlen(const char *s, size_t n)
{
  const char *p = s;
  while (n-- && *p)
    p++;
  return p - s;
}

void printstr(const char *s)
{
  do_syscall3(SYS_write, 1, (uintptr_t)s, strlen(s));
}

#undef putchar
int putchar(int ch)
{
  stdout_buf[stdout_len++] = ch;

  if (ch == '\n' || stdout_len == (int)sizeof(stdout_buf))
    flush_stdout();

  return ch;
}

void printhex(uint64_t x)
{
  char str[17];
  int i;

  for (i = 0; i < 16; i++) {
    str[15 - i] = (x & 0xF) + ((x & 0xF) < 10 ? '0' : 'a' - 10);
    x >>= 4;
  }
  str[16] = 0;

  printstr(str);
}

static inline void printnum(void (*putch)(int, void **), void **putdat,
                            unsigned long long num, unsigned base, int width,
                            int padc)
{
  unsigned digs[sizeof(num) * CHAR_BIT];
  int pos = 0;

  while (1) {
    digs[pos++] = num % base;
    if (num < base)
      break;
    num /= base;
  }

  while (width-- > pos)
    putch(padc, putdat);

  while (pos-- > 0)
    putch(digs[pos] + (digs[pos] >= 10 ? 'a' - 10 : '0'), putdat);
}

static unsigned long long getuint(va_list *ap, int lflag)
{
  if (lflag >= 2)
    return va_arg(*ap, unsigned long long);
  if (lflag)
    return va_arg(*ap, unsigned long);
  return va_arg(*ap, unsigned int);
}

static long long getint(va_list *ap, int lflag)
{
  if (lflag >= 2)
    return va_arg(*ap, long long);
  if (lflag)
    return va_arg(*ap, long);
  return va_arg(*ap, int);
}

static void vprintfmt(void (*putch)(int, void **), void **putdat,
                      const char *fmt, va_list ap)
{
  const char *p;
  const char *last_fmt;
  int ch;
  unsigned long long num;
  int base, lflag, width, precision, altflag;
  char padc;

  while (1) {
    while ((ch = *(const unsigned char *)fmt) != '%') {
      if (ch == '\0')
        return;
      fmt++;
      putch(ch, putdat);
    }
    fmt++;

    last_fmt = fmt;
    padc = ' ';
    width = -1;
    precision = -1;
    lflag = 0;
    altflag = 0;
reswitch:
    switch (ch = *(const unsigned char *)fmt++) {
    case '-':
      padc = '-';
      goto reswitch;
    case '0':
      padc = '0';
      goto reswitch;
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      for (precision = 0;; ++fmt) {
        precision = precision * 10 + ch - '0';
        ch = *fmt;
        if (ch < '0' || ch > '9')
          break;
      }
      goto process_precision;
    case '*':
      precision = va_arg(ap, int);
      goto process_precision;
    case '.':
      if (width < 0)
        width = 0;
      goto reswitch;
    case '#':
      altflag = 1;
      (void)altflag;
      goto reswitch;
process_precision:
      if (width < 0)
        width = precision, precision = -1;
      goto reswitch;
    case 'l':
      lflag++;
      goto reswitch;
    case 'c':
      putch(va_arg(ap, int), putdat);
      break;
    case 's':
      if ((p = va_arg(ap, char *)) == NULL)
        p = "(null)";
      if (width > 0 && padc != '-')
        for (width -= strnlen(p, precision); width > 0; width--)
          putch(padc, putdat);
      for (; (ch = *p) != '\0' && (precision < 0 || --precision >= 0); width--) {
        putch(ch, putdat);
        p++;
      }
      for (; width > 0; width--)
        putch(' ', putdat);
      break;
    case 'd':
      num = getint(&ap, lflag);
      if ((long long)num < 0) {
        putch('-', putdat);
        num = -(long long)num;
      }
      base = 10;
      goto signed_number;
    case 'u':
      base = 10;
      goto unsigned_number;
    case 'o':
      base = 8;
      goto unsigned_number;
    case 'p':
      static_assert(sizeof(long) == sizeof(void *));
      lflag = 1;
      putch('0', putdat);
      putch('x', putdat);
    case 'X':
    case 'x':
      base = 16;
unsigned_number:
      num = getuint(&ap, lflag);
signed_number:
      printnum(putch, putdat, num, base, width, padc);
      break;
    case '%':
      putch(ch, putdat);
      break;
    default:
      putch('%', putdat);
      fmt = last_fmt;
      break;
    }
  }
}

int printf(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  vprintfmt((void *)putchar, 0, fmt, ap);
  va_end(ap);
  return 0;
}

int puts(const char *s)
{
  while (*s)
    putchar(*s++);
  putchar('\n');
  return 0;
}

int sprintf(char *str, const char *fmt, ...)
{
  va_list ap;
  char *str0 = str;

  va_start(ap, fmt);

  void sprintf_putch(int ch, void **data)
  {
    char **pstr = (char **)data;
    **pstr = ch;
    (*pstr)++;
  }

  vprintfmt(sprintf_putch, (void **)&str, fmt, ap);
  *str = 0;

  va_end(ap);
  return str - str0;
}

void *memcpy(void *dest, const void *src, size_t len)
{
  uintptr_t d_addr = (uintptr_t)dest;
  uintptr_t s_addr = (uintptr_t)src;
  uintptr_t end = d_addr + len;

  if (((d_addr | s_addr | len) & (sizeof(uintptr_t) - 1)) == 0) {
    const uintptr_t *s = (const uintptr_t *)src;
    uintptr_t *d = (uintptr_t *)dest;
    while ((uintptr_t)d < end)
      *d++ = *s++;
  } else {
    const char *s = (const char *)src;
    char *d = (char *)dest;
    while ((uintptr_t)d < end)
      *d++ = *s++;
  }
  return dest;
}

void *memset(void *dest, int byte, size_t len)
{
  uintptr_t d_addr = (uintptr_t)dest;
  uintptr_t end = d_addr + len;

  if (((d_addr | len) & (sizeof(uintptr_t) - 1)) == 0) {
    uintptr_t word = byte & 0xFF;
    word |= word << 8;
    word |= word << 16;
#if __riscv_xlen == 64
    word |= word << 32;
#endif

    uintptr_t *d = (uintptr_t *)dest;
    while ((uintptr_t)d < end)
      *d++ = word;
  } else {
    char *d = (char *)dest;
    while ((uintptr_t)d < end)
      *d++ = byte;
  }
  return dest;
}

int strcmp(const char *s1, const char *s2)
{
  unsigned char c1, c2;

  do {
    c1 = *s1++;
    c2 = *s2++;
  } while (c1 != 0 && c1 == c2);

  return c1 - c2;
}

char *strcpy(char *dest, const char *src)
{
  char *d = dest;
  while ((*d++ = *src++))
    ;
  return dest;
}

long atol(const char *str)
{
  long res = 0;
  int sign = 0;

  while (*str == ' ')
    str++;

  if (*str == '-' || *str == '+') {
    sign = *str == '-';
    str++;
  }

  while (*str) {
    res *= 10;
    res += *str++ - '0';
  }

  return sign ? -res : res;
}
