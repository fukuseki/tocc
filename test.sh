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

try 0 "int main(){return 0;}"
try 42 "int main(){return 42;}"
try 21 "int main(){return 5+20-4;}"
try 41 "int main(){return 12 + 34 - 5;}"
try 7 "int main(){return 1 + 2 * 3;}"
try 4 "int main(){return 2 * 3 - 4 / 2;}"
try 9 "int main(){return 1*2+(3+4);}"
try 1 "int main(){return 7 + 2 * -3;}"
try 3 "int main(){return +1 - (-2);}"
try 0 "int main(){return 5==4;}"
try 1 "int main(){return 5==5;}"
try 0 "int main(){return 5==6;}"
try 1 "int main(){return 5==2+3;}"
try 1 "int main(){return 5!=4;}"
try 0 "int main(){return 5!=5;}"
try 1 "int main(){return 5!=6;}"
try 0 "int main(){return 5<4;}"
try 0 "int main(){return 5<5;}"
try 1 "int main(){return 5<6;}"
try 1 "int main(){return 1+2<1+3;}"
try 1 "int main(){return 1<2==-1<0;}"
try 0 "int main(){return 5<=4;}"
try 1 "int main(){return 5<=5;}"
try 1 "int main(){return 5<=6;}"
try 1 "int main(){return 5>4;}"
try 0 "int main(){return 5>5;}"
try 0 "int main(){return 5>6;}"
try 1 "int main(){return 5>=4;}"
try 1 "int main(){return 5>=5;}"
try 0 "int main(){return 5>=6;}"
try 3 "int main(){int a;a=1; return a+2;}"
try 7 "int main(){int a;int b;int z;a=b=2; z=3; return a+b+z;}"
try 5 "int main(){int foo;int bar;foo=2; bar=3; return foo+bar;}"
try 1 "int main(){return 1;}"
try 4 "int main(){int a;int b_;a=b_=2; return a+b_;}"
try 1 "int main(){return 1; 2;}"
try 3 "int main(){int a;a=2; if(1) a=a+1; return a;}"
try 2 "int main(){int a;a=2; if(0) a=a+1; return a;}"
try 3 "int main(){int a;a=2; if(1) a=a+1; else a=a-1; return a;}"
try 1 "int main(){int a;a=2; if(0) a=a+1; else a=a-1; return a;}"
try 6 "int main(){int a;int b;a=3; b=0; while(a=a-1) b = b+3; return b;}"
try 6 "int main(){int a;int b;a=3; b=0; while(a=a-1) if(1) b = b+3; return b;}"
try 3 "int main(){int a;a=0; int i;for(i=1;i<3;i=i+1) a=a+i; return a;}"
try 6 "int main(){int a;int i;int j;a=0; for(i=1;i<3;i=i+1) for(j=0;j<2;j=j+1) a=a+i; return a;}"
try 3 "int main(){{int a;a=1; return a+2;}}"
try 3 "int main(){int a;if(1){a=1; return a+2;}else{int foo;int bar;foo=2; bar=3; return foo+bar;}}"
try 5 "int main(){if(0){int a;a=1; return a+2;}else{int foo;int bar;foo=2; bar=3; return foo+bar;}}"
try 42 "int main(){return foo();}"
try 43 "int main(){return 1+foo();}"
try 11 "int main(){return foo2(1,2);}"
try 12 "int main(){return foo2(1,2)+1;}"
try 11 "int main(){return foo6(1,2,3,4,5,6);}"
try 11 "int main(){return foo2(1,foo6(1,2,3,4,5,6));}"
try 2 "int main(){return hoge();}int hoge(){return 2;}"
try 3 "int main(){return hoge(2)+1;}int hoge(int a){return a;}"
try 11 "int main(){return bar6(1,2,3,4,5,6);}int bar6(int a,int b,int c,int d,int e,int f){return foo6(a,b,c,d,e,f);}"
try 34 "int main(){return fib(8);}int fib(int i){if(i<=1){return 1;}else{return fib(i-2)+fib(i-1);}}"
try 3 "int main(){int x;int y;x=3;y=&x;return *y;}"
try 3 "int main(){int x;int y;int z;x=3;y=5;z=&y + 8;return *z;}" #本テストはスタック変数の配置に依存
try 3 "int main(){int x; int *y; y=&x; *y=3; return x;}"
try 3 "int main(){int i;for(i=0;i<10000000;i=i+1){}return 3;}"

echo OK
