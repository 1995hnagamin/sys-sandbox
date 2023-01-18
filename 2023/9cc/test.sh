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

assert  0 'main() { return 0; }'
assert 42 'main() { return 42; }'
assert 21 'main() { return 5+20-4; }'
assert 10 'main() { return 3- 4 +11; }'
assert 41 'main() { return  12 + 34 - 5 ; }'
assert  5 'main() { return 10- 3-2; }'
assert  9 'main() { return 10-(3-2); }'
assert 47 'main() { return 5+6*7; }'
assert 15 'main() { return 5*(9-6); }'
assert 39 'main() { return 5* 9-6 ; }'
assert  4 'main() { return (3+5)/2; }'
assert  7 'main() { return 3+12 /3; }'
assert 12 'main() { return -3*2+3*(-(1+3)*(+2)+14); }'
assert  1 'main() { return 1+1==2; }'
assert  0 'main() { return 10*3-4==6+21; }'
assert  1 'main() { return 1 > 2 != 1 < 2; }'
assert  0 'main() { return 10 != 5 * 5 - 15; }'
assert  1 'main() { return -1+4 <= 3*(+5)-(3*2); }'
assert  1 'main() { return 10<=5+5; }'
assert  0 'main() { return 2>=4; }'
assert  1 'main() { return 10>=10; }';
assert  1 'main() { return 3<4; }'
assert  0 'main() { return 10<10; }'
assert  0 'main() { return 0>16; }'
assert  0 'main() { return 5>5; }'
assert 20 'main() { a = 3; a = 3 + 7; b = (1+7)/(1+3); return a * b; }'
assert  8 'main() { aaaa = 3; aaab = 5; return aaaa+aaab; }'
assert  7 'main() { a = 4; b = 10; return (a + b)/2; return a; }'
assert 15 'main() { if (1 >= 0) return 15; return 20; }'
assert 29 'main() { if (98 > 97) return 29; else return 31; }'
assert 31 'main() { if (97 > 98) return 29; else return 31; }'
assert  4 'main() { a = 1; if (10>9) {b = 3; a = a + b;} return a; }'
assert 10 'main() { a=10; foo(); return a; }'
assert 22 'main() { a=12; b=34; bar(a, b); return b-a; }'
assert 10 'f(x) { if (5>x) return 10; foo(); return f(x-1); } main() { return f(10); }'
assert 91 'mc(n) { if (n > 100) return n-10; else mc(mc(n+11)); } main() { return mc(99); }'
assert 13 'fib(n) { if (n < 2) return 1; else fib(n-1)+fib(n-2); } main() { return fib(6); }'
assert 15 'main() { x = 15; p = &x; return *p; }'

echo OK
