#!/bin/bash
try() {
  expected=$1
  input=$2

  ./tocc "$input" > tmp.s
  if [ $? != 0 ]; then
    echo ./tocc $input compile failed
    exit 1
  fi
  gcc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ $actual = $expected ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

try 0 0
try 42 42
try 21 "5+20-4"
try 41 "12 + 34 - 5"
try 7 "1 + 2 * 3"
try 4 "2 * 3 - 4 / 2"
try 9 "1*2+(3+4)"
try 1 "7 + 2 * -3"
try 3 "+1 - (-2)"
try 0 "5==4"
try 1 "5==5"
try 0 "5==6"
try 1 "5==2+3"

echo OK
