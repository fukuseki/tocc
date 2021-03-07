#!/bin/bash -e

for i in test/42/*; do
  echo $i
  ASAN_OPTIONS=detect_leaks=0 ./tocc $i > tmp.s
  arm-linux-gnueabihf-gcc -static -g -use-ld=gold -o tmp tmp.s foo.o
  (
    set +e
    qemu-arm ./tmp
    result=$?
    if [[ $result -eq 42 ]]; then
      echo OK;
    else
      echo NG result=$result;
      exit 1
    fi
  )
done
