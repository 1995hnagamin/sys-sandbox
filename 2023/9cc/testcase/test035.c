int f(int x) { if (5>x) return 10; foo(); return f(x-1); } int main() { return f(10); }
