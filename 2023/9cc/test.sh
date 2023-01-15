#!/bin/bash
assert() {
  expected="$1"
  input="$2"

  ./9cc "$input" > tmp.s
  cc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

assert 0 '0;'
assert 42 '42;'
assert 21 '5+20-4;'
assert 10 '3- 4 +11;'
assert 41 ' 12 + 34 - 5 ;'
assert 5 '10- 3-2;'
assert 9 '10-(3-2);'
assert 47 '5+6*7;'
assert 15 '5*(9-6);'
assert 39 '5* 9-6 ;'
assert 4 '(3+5)/2;'
assert 7 '3+12 /3;'
assert 12 '-3*2+3*(-(1+3)*(+2)+14);'
assert 1 '1+1==2;'
assert 0 '10*3-4==6+21;'
assert 1 '1 > 2 != 1 < 2;'
assert 0 '10 != 5 * 5 - 15;'
assert 1 '-1+4 <= 3*(+5)-(3*2);'
assert 1 '10<=5+5;'
assert 0 '2>=4;'
assert 1 '10>=10;';
assert 1 '3<4;'
assert 0 '10<10;'
assert 0 '0>16;'
assert 0 '5>5;'
assert 20 'a = 3; a = 3 + 7; b = (1+7)/(1+3); a * b;'
assert 8 'aaaa = 3; aaab = 5; aaaa+aaab;'
assert 7 'a = 4; b = 10; return (a + b)/2; return a;'
assert 15 'if (1 >= 0) return 15; return 20;'
assert 29 'if (98 > 97) 29; else 31;'
assert 31 'if (97 > 98) 29; else 31;'
assert 4 'a = 1; if (10>9) {b = 3; a = a + b;} a;'

echo OK
