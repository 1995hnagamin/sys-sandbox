#!/bin/bash
assert() {
  expected="$1"
  input="$2"

  ./9cc "testcase/$input" > tmp.s
  cc -o tmp foo.o tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

assert  0 test001.c
assert 42 test002.c
assert 21 arith/test003.c
assert 10 arith/test004.c
assert 41 arith/test005.c
assert  5 arith/test006.c
assert  9 arith/test007.c
assert 47 arith/test008.c
assert 15 arith/test009.c
assert 39 arith/test010.c
assert  4 arith/test011.c
assert  7 arith/test012.c
assert 12 arith/test013.c
assert  1 arith/test014.c
assert  0 arith/test015.c
assert  1 arith/test016.c
assert  0 arith/test017.c
assert  1 arith/test018.c
assert  1 arith/test019.c
assert  0 arith/test020.c
assert  1 arith/test021.c
assert  1 arith/test022.c
assert  0 arith/test023.c
assert  0 arith/test024.c
assert  0 arith/test025.c
assert 20 lvar/test026.c
assert  8 lvar/test027.c
assert  7 lvar/test028.c
assert 15 ctrlflow/test029.c
assert 29 ctrlflow/test030.c
assert 31 ctrlflow/test031.c
assert  4 ctrlflow/test032.c
assert 55 ctrlflow/while01.c
assert 10 fn/test033.c
assert 22 fn/test034.c
assert 10 fn/test035.c
assert 91 fn/test036.c
assert 21 fn/test037.c
assert 13 fn/test038.c
assert  0 fn/negarg01.c
assert 15 ptr/test039.c
assert  3 ptr/test040.c
assert  4 sizeof/test041.c
assert  8 sizeof/test042.c
assert  4 sizeof/test043.c
assert  8 sizeof/test044.c
assert  4 sizeof/test045.c
assert  4 sizeof/test046.c
assert  4 sizeof/test047.c
assert 13 arr/test048.c
assert 14 arr/test049.c
assert 17 arr/test050.c
assert 20 arr/test051.c
assert 53 test052.c
assert 31 gvar/test053.c
assert 43 gvar/test054.c
assert 10 gvar/test055.c
assert 19 gvar/test056.c
assert  3 char/test057.c
assert  3 char/test058.c
assert 11 char/test059.c
assert 32 char/test060.c

echo OK
