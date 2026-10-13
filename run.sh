#!/bin/bash

./dockcross make $2 -C $1

file $1/build/$1

qemu-mips -strace "$1/build/$1" < /dev/null