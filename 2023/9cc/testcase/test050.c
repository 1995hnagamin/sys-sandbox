int main() { int a[10]; a[0] = 10; a[5] = 6; a[a[5]] = 1; return a[0]+a[5]+a[6];}
