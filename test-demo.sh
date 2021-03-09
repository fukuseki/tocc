#!/bin/bash -e

for i in test/demo/*; do
  echo $i
  ASAN_OPTIONS=detect_leaks=0 ./tocc $i > tmp.s
  arm-linux-gnueabihf-gcc -static -g -use-ld=gold -o tmp tmp.s foo.o
  (
    qemu-arm ./tmp
  )
done
