#!/bin/bash
assert() {
  expected="$1"
  input="$2"

  ./9cc "$input" > tmp.s
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

assert 0 'main() {0;}'
assert 42 'main() { 42; }'
assert 21 'main() { 5+20-4; }'
assert 10 'main() { 3- 4 +11; }'
assert 41 'main() {  12 + 34 - 5 ; }'
assert 5 'main() { 10- 3-2; }'
assert 9 'main() { 10-(3-2); }'
assert 47 'main() { 5+6*7; }'
assert 15 'main() { 5*(9-6); }'
assert 39 'main() { 5* 9-6 ; }'
assert 4 'main() { (3+5)/2; }'
assert 7 'main() { 3+12 /3; }'
assert 12 'main() { -3*2+3*(-(1+3)*(+2)+14); }'
assert 1 'main() { 1+1==2; }'
assert 0 'main() { 10*3-4==6+21; }'
assert 1 'main() { 1 > 2 != 1 < 2; }'
assert 0 'main() { 10 != 5 * 5 - 15; }'
assert 1 'main() { -1+4 <= 3*(+5)-(3*2); }'
assert 1 'main() { 10<=5+5; }'
assert 0 'main() { 2>=4; }'
assert 1 'main() { 10>=10; }';
assert 1 'main() { 3<4; }'
assert 0 'main() { 10<10; }'
assert 0 'main() { 0>16; }'
assert 0 'main() { 5>5; }'
assert 20 'main() { a = 3; a = 3 + 7; b = (1+7)/(1+3); a * b; }'
assert 8 'main() { aaaa = 3; aaab = 5; aaaa+aaab; }'
assert 7 'main() { a = 4; b = 10; return (a + b)/2; return a; }'
assert 15 'main() { if (1 >= 0) return 15; return 20; }'
assert 29 'main() { if (98 > 97) 29; else 31; }'
assert 31 'main() { if (97 > 98) 29; else 31; }'
assert 4 'main() { a = 1; if (10>9) {b = 3; a = a + b;} a; }'
assert 10 'main() { a=10; foo(); a; }'
assert 22 'main() { a=12; b=34; bar(a, b); b-a; }'

echo OK
