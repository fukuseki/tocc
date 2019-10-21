#!/bin/bash -e

./tocc test/test.c > tmp.s
gcc -static -g -use-ld=gold -o tmp tmp.s foo.o
./tmp
