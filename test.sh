#!/bin/bash
try() {
  expected=$1
  input=$2

  ./tocc "$input" > tmp.s
  if [ $? != 0 ]; then
    echo ./tocc $input compile failed
    exit 1
  fi
  gcc -g -o tmp tmp.s foo.o
  ./tmp
  actual="$?"

  if [ $actual = $expected ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

try 0 "main(){return 0;}"
try 42 "main(){return 42;}"
try 21 "main(){return 5+20-4;}"
try 41 "main(){return 12 + 34 - 5;}"
try 7 "main(){return 1 + 2 * 3;}"
try 4 "main(){return 2 * 3 - 4 / 2;}"
try 9 "main(){return 1*2+(3+4);}"
try 1 "main(){return 7 + 2 * -3;}"
try 3 "main(){return +1 - (-2);}"
try 0 "main(){return 5==4;}"
try 1 "main(){return 5==5;}"
try 0 "main(){return 5==6;}"
try 1 "main(){return 5==2+3;}"
try 1 "main(){return 5!=4;}"
try 0 "main(){return 5!=5;}"
try 1 "main(){return 5!=6;}"
try 0 "main(){return 5<4;}"
try 0 "main(){return 5<5;}"
try 1 "main(){return 5<6;}"
try 1 "main(){return 1+2<1+3;}"
try 1 "main(){return 1<2==-1<0;}"
try 0 "main(){return 5<=4;}"
try 1 "main(){return 5<=5;}"
try 1 "main(){return 5<=6;}"
try 1 "main(){return 5>4;}"
try 0 "main(){return 5>5;}"
try 0 "main(){return 5>6;}"
try 1 "main(){return 5>=4;}"
try 1 "main(){return 5>=5;}"
try 0 "main(){return 5>=6;}"
try 3 "main(){a=1; return a+2;}"
try 7 "main(){a=b=2; z=3; return a+b+z;}"
try 5 "main(){foo=2; bar=3; return foo+bar;}"
try 1 "main(){return 1;}"
try 4 "main(){a=b_=2; return a+b_;}"
try 1 "main(){return 1; 2;}"
try 3 "main(){a=2; if(1) a=a+1; return a;}"
try 2 "main(){a=2; if(0) a=a+1; return a;}"
try 3 "main(){a=2; if(1) a=a+1; else a=a-1; return a;}"
try 1 "main(){a=2; if(0) a=a+1; else a=a-1; return a;}"
try 6 "main(){a=3; b=0; while(a=a-1) b = b+3; return b;}"
try 6 "main(){a=3; b=0; while(a=a-1) if(1) b = b+3; return b;}"
try 3 "main(){a=0; for(i=1;i<3;i=i+1) a=a+i; return a;}"
try 6 "main(){a=0; for(i=1;i<3;i=i+1) for(j=0;j<2;j=j+1) a=a+i; return a;}"
try 3 "main(){{a=1; return a+2;}}"
try 3 "main(){if(1){a=1; return a+2;}else{foo=2; bar=3; return foo+bar;}}"
try 5 "main(){if(0){a=1; return a+2;}else{foo=2; bar=3; return foo+bar;}}"
try 42 "main(){return foo();}"
try 43 "main(){return 1+foo();}"
try 11 "main(){return foo2(1,2);}"
try 12 "main(){return foo2(1,2)+1;}"
try 11 "main(){return foo6(1,2,3,4,5,6);}"
try 11 "main(){return foo2(1,foo6(1,2,3,4,5,6));}"
try 2 "main(){return hoge();}hoge(){return 2;}"
try 3 "main(){return hoge(2)+1;}hoge(a){return a;}"
try 11 "main(){return bar6(1,2,3,4,5,6);}bar6(a,b,c,d,e,f){return foo6(a,b,c,d,e,f);}"
try 34 "main(){return fib(8);}fib(i){if(i<=1){return 1;}else{return fib(i-2)+fib(i-1);}}"
try 3 "main(){x=3;y=&x;return *y;}"
try 3 "main(){x=3;y=5;z=&y + 8;return *z;}" #本テストはスタック変数の配置に依存

echo OK
