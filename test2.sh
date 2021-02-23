#!/bin/bash

./tocc test/42.c > tmp.s
arm-linux-gnueabihf-gcc -static -g -use-ld=gold -o tmp tmp.s foo.o
qemu-arm ./tmp
if [[ $? -eq 42 ]]; then
  echo OK;
else
  echo NG;
fi
