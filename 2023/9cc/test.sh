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

assert  0 'int main() { return 0; }'
assert 42 'int main() { return 42; }'
assert 21 'int main() { return 5+20-4; }'
assert 10 'int main() { return 3- 4 +11; }'
assert 41 'int main() { return  12 + 34 - 5 ; }'
assert  5 'int main() { return 10- 3-2; }'
assert  9 'int main() { return 10-(3-2); }'
assert 47 'int main() { return 5+6*7; }'
assert 15 'int main() { return 5*(9-6); }'
assert 39 'int main() { return 5* 9-6 ; }'
assert  4 'int main() { return (3+5)/2; }'
assert  7 'int main() { return 3+12 /3; }'
assert 12 'int main() { return -3*2+3*(-(1+3)*(+2)+14); }'
assert  1 'int main() { return 1+1==2; }'
assert  0 'int main() { return 10*3-4==6+21; }'
assert  1 'int main() { return 1 > 2 != 1 < 2; }'
assert  0 'int main() { return 10 != 5 * 5 - 15; }'
assert  1 'int main() { return -1+4 <= 3*(+5)-(3*2); }'
assert  1 'int main() { return 10<=5+5; }'
assert  0 'int main() { return 2>=4; }'
assert  1 'int main() { return 10>=10; }';
assert  1 'int main() { return 3<4; }'
assert  0 'int main() { return 10<10; }'
assert  0 'int main() { return 0>16; }'
assert  0 'int main() { return 5>5; }'
assert 20 'int main() { int a; a = 3; a = 3 + 7; int b; b = (1+7)/(1+3); return a * b; }'
assert  8 'int main() { int aaaa; aaaa = 3; int aaab; aaab = 5; return aaaa+aaab; }'
assert  7 'int main() { int a; int b; a = 4; b = 10; return (a + b)/2; return a; }'
assert 15 'int main() { if (1 >= 0) return 15; return 20; }'
assert 29 'int main() { if (98 > 97) return 29; else return 31; }'
assert 31 'int main() { if (97 > 98) return 29; else return 31; }'
assert  4 'int main() { int a; a = 1; if (10>9) { int b; b = 3; a = a + b;} return a; }'
assert 10 'int main() { int a; a=10; foo(); return a; }'
assert 22 'int main() { int a; a=12; int b; b=34; bar(a, b); return b-a; }'
assert 10 'int f(int x) { if (5>x) return 10; foo(); return f(x-1); } int main() { return f(10); }'
assert 91 'int mc(int n) { if (n > 100) return n-10; else mc(mc(n+11)); } int main() { return mc(99); }'
assert 21 'int sum(int a,int b,int c,int d,int e,int f){return a+b+c+d+e+f;}int main(){return sum(1,2,3,4,5,6);}'
assert 13 'int fib(int n) { if (n < 2) return 1; else fib(n-1)+fib(n-2); } int main() { return fib(6); }'
assert 15 'int main() { int x; int *p; x = 15; p = &x; return *p; }'
assert  3 'int main() { int x; x = 10; int *p; p = &x; *p = 3; return x; }'
assert  4 'int main() { int x; return sizeof(x); }'
assert  8 'int main() { int *y; return sizeof(y); }'
assert  4 'int main() { int x; return sizeof(x+3); }'
assert  8 'int main() { int *y; return sizeof(y+3); }'
assert  4 'int main() { int *y; return sizeof(*y); }'
assert  4 'int main() { return sizeof(100); }'
assert  4 'int main() { return sizeof(sizeof(100)); }'
assert 13 'int main() { int x[10]; return 13; }'
assert 14 'int main() { int a[2]; *a = 10; *(a+1) = 4; return *a + *(a+1); }'
assert 17 'int main() { int a[10]; a[0] = 10; a[5] = 6; a[a[5]] = 1; return a[0]+a[5]+a[6];}'
assert 20 'int main() { int a[3][3][3]; a[0][0][0] = 10; a[1][1][1] = 10; return a[0][0][0]+a[1][1][1];}'
assert 53 'int main() { int f(); return f(); } int f() { return 53; }'
assert 31 'int a; int main() { a = 31; return a; }'
assert 43 'int bb; int g() { bb = 21; return bb; } int main() { int a; a = g(); return bb * 2 + 1; }'
assert 10 'int a[10]; int main() { a[7] = 10; return a[7]; }'
assert 19 'int *p; int main() { int a; p = &a; *p = 19; return a; }'
assert  3 'int main() { char x[3]; x[0] = -1; x[1] = 2; int y; y = 4; return x[0] + y; }'
assert  3 'int main() { char x[13]; int y; y = 4; x[0] = 100; x[1] = -1; x[7] = 100; x[8] = 100; x[9] = 100; x[10] = 100; return x[1] + y; }'
assert 11 'int main() { char x[50]; x[36] = -1; x[37] = 11; x[38] = -1; return x[37];  }'
assert 32 'int main() { char *s; s = "Aa"; return s[1] - s[0]; }'

echo OK
