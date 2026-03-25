#!/bin/bash

set -e

BASEDIR=$PWD
CM_FOLDER=coremark
BARE_TOOLCHAIN=/home/yinjianhui/Xuantie-900-gcc-elf-newlib-x86_64-V3.2.0
BARE_FLAGS="-march=rv64imf -mabi=lp64f -DHAS_FLOAT=0"
BARE_ITERATIONS=500

export PATH="$BASEDIR/toolchain-compat/bin:$PATH"
export COMPILER_PATH="$BASEDIR/toolchain-compat/bin${COMPILER_PATH:+:$COMPILER_PATH}"

cd "$BASEDIR/$CM_FOLDER"

echo "Start compilation"
make PORT_DIR=../riscv64 compile XCFLAGS="-march=rv64gc -mabi=lp64d"
mv coremark.riscv ../

make PORT_DIR=../riscv64-baremetal compile ITERATIONS="$BARE_ITERATIONS" RISCV="$BARE_TOOLCHAIN" XCFLAGS="$BARE_FLAGS"
mv coremark.bare.riscv ../
