#!/bin/bash

./dockcross make $1

file ./build/c2

qemu-mips ./build/c2
