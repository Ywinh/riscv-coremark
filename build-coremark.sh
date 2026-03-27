#!/bin/bash

set -e

BASEDIR=$PWD
CM_FOLDER=coremark
BARE_TOOLCHAIN=/home/yinjianhui/Xuantie-900-gcc-elf-newlib-x86_64-V3.2.0
BARE_FLAGS="-march=rv64imf -mabi=lp64f -DHAS_FLOAT=0"
BARE_ITERATIONS=50
BAREMETAL_ENABLE_PRINTF=${BAREMETAL_ENABLE_PRINTF:-1}
enable_920=${enable_920:-0}
TOOLCHAIN_COMPAT_BIN="$BASEDIR/toolchain-compat/bin"

cd "$BASEDIR/$CM_FOLDER"

echo "Start compilation"
PATH="$TOOLCHAIN_COMPAT_BIN:$PATH" COMPILER_PATH="$TOOLCHAIN_COMPAT_BIN${COMPILER_PATH:+:$COMPILER_PATH}" make PORT_DIR=../riscv64 compile XCFLAGS="-march=rv64gc -mabi=lp64d"
mv coremark.riscv ../

COMPILER_PATH= make PORT_DIR=../riscv64-baremetal compile ITERATIONS="$BARE_ITERATIONS" RISCV="$BARE_TOOLCHAIN" BAREMETAL_ENABLE_PRINTF="$BAREMETAL_ENABLE_PRINTF" enable_920="$enable_920" XCFLAGS="$BARE_FLAGS"
mv coremark.bare.riscv ../
