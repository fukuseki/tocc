#!/bin/bash
try() {
  expected=$1
  input=$2

  ./tocc "$input" > tmp.s
  if [ $? != 0 ]; then
    echo ./tocc $input compile failed
    exit 1
  fi
  gcc -g -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ $actual = $expected ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

try 0 "0;"
try 42 "42;"
try 21 "5+20-4;"
try 41 "12 + 34 - 5;"
try 7 "1 + 2 * 3;"
try 4 "2 * 3 - 4 / 2;"
try 9 "1*2+(3+4);"
try 1 "7 + 2 * -3;"
try 3 "+1 - (-2);"
try 0 "5==4;"
try 1 "5==5;"
try 0 "5==6;"
try 1 "5==2+3;"
try 1 "5!=4;"
try 0 "5!=5;"
try 1 "5!=6;"
try 0 "5<4;"
try 0 "5<5;"
try 1 "5<6;"
try 1 "1+2<1+3;"
try 1 "1<2==-1<0;"
try 0 "5<=4;"
try 1 "5<=5;"
try 1 "5<=6;"
try 1 "5>4;"
try 0 "5>5;"
try 0 "5>6;"
try 1 "5>=4;"
try 1 "5>=5;"
try 0 "5>=6;"
try 3 "a=1; a+2;"
try 7 "a=b=2; z=3; a+b+z;"
try 5 "foo=2; bar=3; foo+bar;"
try 1 "return 1;"
try 4 "a=b_=2; return a+b_;"
try 1 "return 1; 2;"
try 3 "a=2; if(1) a=a+1; return a;"
try 2 "a=2; if(0) a=a+1; return a;"
try 3 "a=2; if(1) a=a+1; else a=a-1; return a;"
try 1 "a=2; if(0) a=a+1; else a=a-1; return a;"
try 6 "a=3; b=0; while(a=a-1) b = b+3; return b;"
try 6 "a=3; b=0; while(a=a-1) if(1) b = b+3; return b;"

echo OK
