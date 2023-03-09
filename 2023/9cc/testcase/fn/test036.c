int mc(int n) {
  if (n > 100)
    return n - 10;
  else
    mc(mc(n + 11));
}
int main() {
  return mc(99);
}
