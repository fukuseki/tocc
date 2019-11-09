int test_1() {
  return 0;
}
int test_2() {
  return 42;
}
int test_3() {
  return 5 + 20 - 4;
}
int test_4() {
  return 12 + 34 - 5;
}
int test_5() {
  return 1 + 2 * 3;
}
int test_6() {
  return 2 * 3 - 4 / 2;
}
int test_7() {
  return 1 * 2 + (3 + 4);
}
int test_8() {
  return 7 + 2 * -3;
}
int test_9() {
  return +1 - (-2);
}
int test_10() {
  return 5 == 4;
}
int test_11() {
  return 5 == 5;
}
int test_12() {
  return 5 == 6;
}
int test_13() {
  return 5 == 2 + 3;
}
int test_14() {
  return 5 != 4;
}
int test_15() {
  return 5 != 5;
}
int test_16() {
  return 5 != 6;
}
int test_17() {
  return 5 < 4;
}
int test_18() {
  return 5 < 5;
}
int test_19() {
  return 5 < 6;
}
int test_20() {
  return 1 + 2 < 1 + 3;
}
int test_21() {
  return 1 < 2 == -1 < 0;
}
int test_22() {
  return 5 <= 4;
}
int test_23() {
  return 5 <= 5;
}
int test_24() {
  return 5 <= 6;
}
int test_25() {
  return 5 > 4;
}
int test_26() {
  return 5 > 5;
}
int test_27() {
  return 5 > 6;
}
int test_28() {
  return 5 >= 4;
}
int test_29() {
  return 5 >= 5;
}
int test_30() {
  return 5 >= 6;
}
int test_31() {
  int a;
  a = 1;
  return a + 2;
}
int test_32() {
  int a;
  int b;
  int z;
  a = b = 2;
  z = 3;
  return a + b + z;
}
int test_33() {
  int foo;
  int bar;
  foo = 2;
  bar = 3;
  return foo + bar;
}
int test_34() {
  return 1;
}
int test_35() {
  int a;
  int b_;
  a = b_ = 2;
  return a + b_;
}
int test_36() {
  return 1;
  2;
}
int test_37() {
  int a;
  a = 2;
  if (1)
    a = a + 1;
  return a;
}
int test_38() {
  int a;
  a = 2;
  if (0)
    a = a + 1;
  return a;
}
int test_39() {
  int a;
  a = 2;
  if (1)
    a = a + 1;
  else
    a = a - 1;
  return a;
}
int test_40() {
  int a;
  a = 2;
  if (0)
    a = a + 1;
  else
    a = a - 1;
  return a;
}
int test_41() {
  int a;
  int b;
  a = 3;
  b = 0;
  while (a = a - 1)
    b = b + 3;
  return b;
}
int test_42() {
  int a;
  int b;
  a = 3;
  b = 0;
  while (a = a - 1)
    if (1)
      b = b + 3;
  return b;
}
int test_43() {
  int a;
  a = 0;
  int i;
  for (i = 1; i < 3; i = i + 1)
    a = a + i;
  return a;
}
int test_44() {
  int a;
  int i;
  int j;
  a = 0;
  for (i = 1; i < 3; i = i + 1)
    for (j = 0; j < 2; j = j + 1)
      a = a + i;
  return a;
}
int test_45() {
  {
    int a;
    a = 1;
    return a + 2;
  }
}
int test_46() {
  int a;
  if (1) {
    a = 1;
    return a + 2;
  } else {
    int foo;
    int bar;
    foo = 2;
    bar = 3;
    return foo + bar;
  }
}
int test_47() {
  if (0) {
    int a;
    a = 1;
    return a + 2;
  } else {
    int foo;
    int bar;
    foo = 2;
    bar = 3;
    return foo + bar;
  }
}
int test_48() {
  return foo();
}
int test_49() {
  return 1 + foo();
}
int test_50() {
  return foo2(1, 2);
}
int test_51() {
  return foo2(1, 2) + 1;
}
int test_52() {
  return foo6(1, 2, 3, 4, 5, 6);
}
int test_53() {
  return foo2(1, foo6(1, 2, 3, 4, 5, 6));
}
int hoge54() {
  return 2;
}
int test_54() {
  return hoge54();
}
int hoge55(int a) {
  return a;
}
int test_55() {
  return hoge55(2) + 1;
}
int test_56() {
  return bar6(1, 2, 3, 4, 5, 6);
}
int bar6(int a, int b, int c, int d, int e, int f) {
  return foo6(a, b, c, d, e, f);
}
int test_57() {
  return fib(8);
}
int fib(int i) {
  if (i <= 1) {
    return 1;
  } else {
    return fib(i - 2) + fib(i - 1);
  }
}
int test_58() {
  int x;
  int* y;
  x = 3;
  y = &x;
  return *y;
}
int test_59() {
  int x;
  int y;
  int* z;
  x = 3;
  y = 5;
  z = &y + 1;
  return *z;
}
int test_60() {
  int x;
  int* y;
  y = &x;
  *y = 3;
  return x;
}
int test_61() {
  int i;
  for (i = 0; i < 10000000; i = i + 1) {
  }
  return 3;
}
int test_62() {
  int* p;
  alloc4(&p, 1, 2, 4, 8);
  return *p;
}
int test_63() {
  int* p;
  alloc4(&p, 1, 2, 4, 8);
  return *(p + 1);
}
int test_64() {
  int* p;
  alloc4(&p, 1, 2, 4, 8);
  return *(p + 2);
}
int test_65() {
  int* p;
  alloc4(&p, 1, 2, 4, 8);
  return *(p + 3);
}
int test_66() {
  int* p;
  alloc4(&p, 1, 2, 4, 8);
  return *(p - 1 + 4);
}
int test_67() {
  int* p;
  alloc4(&p, 1, 2, 4, 8);
  return *(3 + p);
}
int test_68() {
  int* p;
  alloc4(&p, 1, 2, 4, 8);
  return *(+3 + p);
}
int test_69() {
  int* p;
  alloc4(&p, 1, 2, 4, 8);
  return *(-1 + p + 4);
}
int test_70() {
  int* p;
  alloc4(&p, 1, 2, 4, 8);
  return *(p + 1 * 3);
}
int test_71() {
  int* p;
  alloc4(&p, 1, 2, 4, 8);
  return *(p + 9 / 3);
}
int test_72() {
  int a;
  int b;
  a = 1;
  b = 3;
  int** p;
  alloc2p(&p, &a, &b);
  return **(p + 1);
}
int test_73() {
  int a;
  return sizeof(a);
}
int test_74() {
  int* a;
  return sizeof(a);
}
int test_75() {
  int** a;
  return sizeof(a);
}
int test_76() {
  int a;
  return sizeof(&a);
}
int test_77() {
  int a;
  return sizeof(&&a);
}
int test_78() {
  int a;
  return sizeof(a + 3);
}
int test_79() {
  int* a;
  return sizeof(a + 3);
}
int test_80() {
  int* a;
  return sizeof(*a);
}
int test_81() {
  return sizeof(1);
}
int test_82() {
  return sizeof(sizeof(1));
}
int test_83() {
  int a[2];
  *a = 1;
  *(a + 1) = 2;
  int* p;
  p = a;
  return *p + *(p + 1);
}
int test_84() {
  int a[2];
  a[0] = 1;
  a[1] = 2;
  int* p;
  p = a;
  return p[0] + 1 [p];
}
int g85;
int test_85() {
  return 3;
}
int g86;
int test_86() {
  g86 = 3;
  return g86;
}
int g87;
int bar87() {
  g87 = g87 + 2;
  return 0;
}
int test_87() {
  g87 = 1;
  bar87();
  return g87;
}
int test_88() {
  char x[3];
  x[0] = -1;
  x[1] = 2;
  int y;
  y = 4;
  return x[0] + y;
}
int test_89() {
  "abc";
  return 0;
}
int test_90() {
  char* p;
  p = "abc";
  return p[2];
}
int test_91() {
  char* p;
  p = "ad";
  return p[1] - p[0];
}
int test_92() {
  printf("abc\n");
  return 0;
}
int test_93() {
  printf("i=%d\n", 3);
  return 0;
}
int test_94() {
  // 行コメント
  return 0; /* ブロックコメント */
}

int g95 = 3;
int test_95() {
  return g95;
}

int g96[2] = {1, 2};
int test_96() {
  return g96[0] + g96[1];
}

char g97[3] = {1, 2, 3};
int test_97() {
  return g97[0] + g97[1] + g97[2];
}

int error_count;
int expect_eq(int expected, int actual, char* test_name) {
  if (expected == actual) {
    printf("ok %s\n", test_name);
    return 0;
  } else {
    error_count = error_count + 1;
    printf("not ok %s: expected=%d, actual=%d(%#8x)\n", test_name, expected,
           actual, actual);
    return 1;
  }
}

int main() {
  expect_eq(0, test_1(), "test_1");
  expect_eq(42, test_2(), "test_2");
  expect_eq(21, test_3(), "test_3");
  expect_eq(41, test_4(), "test_4");
  expect_eq(7, test_5(), "test_5");
  expect_eq(4, test_6(), "test_6");
  expect_eq(9, test_7(), "test_7");
  expect_eq(1, test_8(), "test_8");
  expect_eq(3, test_9(), "test_9");
  expect_eq(0, test_10(), "test_10");
  expect_eq(1, test_11(), "test_11");
  expect_eq(0, test_12(), "test_12");
  expect_eq(1, test_13(), "test_13");
  expect_eq(1, test_14(), "test_14");
  expect_eq(0, test_15(), "test_15");
  expect_eq(1, test_16(), "test_16");
  expect_eq(0, test_17(), "test_17");
  expect_eq(0, test_18(), "test_18");
  expect_eq(1, test_19(), "test_19");
  expect_eq(1, test_20(), "test_20");
  expect_eq(1, test_21(), "test_21");
  expect_eq(0, test_22(), "test_22");
  expect_eq(1, test_23(), "test_23");
  expect_eq(1, test_24(), "test_24");
  expect_eq(1, test_25(), "test_25");
  expect_eq(0, test_26(), "test_26");
  expect_eq(0, test_27(), "test_27");
  expect_eq(1, test_28(), "test_28");
  expect_eq(1, test_29(), "test_29");
  expect_eq(0, test_30(), "test_30");
  expect_eq(3, test_31(), "test_31");
  expect_eq(7, test_32(), "test_32");
  expect_eq(5, test_33(), "test_33");
  expect_eq(1, test_34(), "test_34");
  expect_eq(4, test_35(), "test_35");
  expect_eq(1, test_36(), "test_36");
  expect_eq(3, test_37(), "test_37");
  expect_eq(2, test_38(), "test_38");
  expect_eq(3, test_39(), "test_39");
  expect_eq(1, test_40(), "test_40");
  expect_eq(6, test_41(), "test_41");
  expect_eq(6, test_42(), "test_42");
  expect_eq(3, test_43(), "test_43");
  expect_eq(6, test_44(), "test_44");
  expect_eq(3, test_45(), "test_45");
  expect_eq(3, test_46(), "test_46");
  expect_eq(5, test_47(), "test_47");
  expect_eq(42, test_48(), "test_48");
  expect_eq(43, test_49(), "test_49");
  expect_eq(11, test_50(), "test_50");
  expect_eq(12, test_51(), "test_51");
  expect_eq(11, test_52(), "test_52");
  expect_eq(11, test_53(), "test_53");
  expect_eq(2, test_54(), "test_54");
  expect_eq(3, test_55(), "test_55");
  expect_eq(11, test_56(), "test_56");
  expect_eq(34, test_57(), "test_57");
  expect_eq(3, test_58(), "test_58");
  expect_eq(3, test_59(), "test_59");
  expect_eq(3, test_60(), "test_60");
  expect_eq(3, test_61(), "test_61");
  expect_eq(1, test_62(), "test_62");
  expect_eq(2, test_63(), "test_63");
  expect_eq(4, test_64(), "test_64");
  expect_eq(8, test_65(), "test_65");
  expect_eq(8, test_66(), "test_66");
  expect_eq(8, test_67(), "test_67");
  expect_eq(8, test_68(), "test_68");
  expect_eq(8, test_69(), "test_69");
  expect_eq(8, test_70(), "test_70");
  expect_eq(8, test_71(), "test_71");
  expect_eq(3, test_72(), "test_72");
  expect_eq(4, test_73(), "test_73");
  expect_eq(8, test_74(), "test_74");
  expect_eq(8, test_75(), "test_75");
  expect_eq(8, test_76(), "test_76");
  expect_eq(8, test_77(), "test_77");
  expect_eq(4, test_78(), "test_78");
  expect_eq(8, test_79(), "test_79");
  expect_eq(4, test_80(), "test_80");
  expect_eq(4, test_81(), "test_81");
  expect_eq(4, test_82(), "test_82");
  expect_eq(3, test_83(), "test_83");
  expect_eq(3, test_84(), "test_84");
  expect_eq(3, test_85(), "test_85");
  expect_eq(3, test_86(), "test_86");
  expect_eq(3, test_87(), "test_87");
  expect_eq(3, test_88(), "test_88");
  expect_eq(0, test_89(), "test_89");
  expect_eq(99, test_90(), "test_90");
  expect_eq(3, test_91(), "test_91");
  expect_eq(0, test_92(), "test_92");
  expect_eq(0, test_93(), "test_93");
  expect_eq(0, test_94(), "test_94");
  expect_eq(3, test_95(), "test_95");
  expect_eq(3, test_96(), "test_96");
  expect_eq(6, test_97(), "test_97");

  if (error_count) {
    printf("%d ERROR\n", error_count);
  } else {
    printf("OK\n");
  }

  return error_count;
}
