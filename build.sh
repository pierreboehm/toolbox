#!/bin/bash

SOURCE="utmp.c net.c last.c"
BINARY="last"
GCC_PATH="/Users/pierreboehm/Downloads/aarch64-unknown-linux-gnu/bin/"
GCC_COMPILER="aarch64-unknown-linux-gnu-gcc"
GCC_STRIPPER="aarch64-unknown-linux-gnu-strip"

#   ./aarch64-unknown-linux-gnu-gcc -g -Wall -march=armv8-a -o helloworld64 ~/test/helloworld.c

$GCC_PATH$GCC_COMPILER -g -Wall -march=armv8-a -o $BINARY $SOURCE

if [ ! -e $BINARY ]; then
    echo "Build failed"
else
    echo "Build success. Stripping follows"

    eval $(stat -s $BINARY)
    size=$(echo $st_size)
    echo "(Unstripped size = $size)"

    $GCC_PATH$GCC_STRIPPER $BINARY

    eval $(stat -s $BINARY)
    size=$(echo $st_size)
    echo "(Stripped size = $size)"
fi
