ENTRY(_start)

SECTIONS
{
  . = 0x80000000;
  . = ALIGN(16);

  .text : {
    KEEP(*(.text.init))
    *(.text)
    *(.text.*)
  }

  . = ALIGN(16);

  .rodata : {
    *(.srodata)
    *(.srodata.*)
    *(.rodata)
    *(.rodata.*)
  }

  . = ALIGN(16);

  .data : {
    *(.data)
    *(.data.*)
    __global_pointer$ = . + 0x800;
    *(.sdata)
    *(.sdata.*)
    *(COMMON)
  }

  . = ALIGN(16);

  .bss : {
    *(.sbss)
    *(.sbss.*)
    *(.scommon)
    *(.bss)
    *(.bss.*)

    . = ALIGN(16);
    heap_low = .;

    . = . + 0x1000000;

    . = ALIGN(16);
    heap_top = .;

    . = . + 0x1000000;

    . = ALIGN(16);
    stack_top = .;
  }
}
