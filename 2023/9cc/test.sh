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

assert 92 8queen.c
assert  0 test001.c
assert 42 test002.c
assert 21 test003.c
assert 10 test004.c
assert 41 test005.c
assert  5 test006.c
assert  9 test007.c
assert 47 test008.c
assert 15 test009.c
assert 39 test010.c
assert  4 test011.c
assert  7 test012.c
assert 12 test013.c
assert  1 test014.c
assert  0 test015.c
assert  1 test016.c
assert  0 test017.c
assert  1 test018.c
assert  1 test019.c
assert  0 test020.c
assert  1 test021.c
assert  1 test022.c
assert  0 test023.c
assert  0 test024.c
assert  0 test025.c
assert 20 test026.c
assert  8 test027.c
assert  7 test028.c
assert 15 test029.c
assert 29 test030.c
assert 31 test031.c
assert  4 test032.c
assert 55 ctrlflow/while01.c
assert 10 test033.c
assert 22 test034.c
assert 10 test035.c
assert 91 test036.c
assert 21 test037.c
assert 13 test038.c
assert 15 test039.c
assert  3 test040.c
assert  4 test041.c
assert  8 test042.c
assert  4 test043.c
assert  8 test044.c
assert  4 test045.c
assert  4 test046.c
assert  4 test047.c
assert 13 test048.c
assert 14 test049.c
assert 17 test050.c
assert 20 test051.c
assert 53 test052.c
assert 31 test053.c
assert 43 test054.c
assert 10 test055.c
assert 19 test056.c
assert  3 test057.c
assert  3 test058.c
assert 11 test059.c
assert 32 test060.c

echo OK
