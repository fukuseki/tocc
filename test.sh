#!/bin/bash -e

./tocc test/test.c > tmp.s
arm-linux-gnueabihf-gcc -static -g -use-ld=gold -o tmp tmp.s foo.o
qemu-arm ./tmp
