int pos[8];

int diag(int x, int y, int dir) {
  int a;
  a = x - 1;
  int b;
  b = y + dir;
  while (a >= 0) {
    if (pos[a] == b) {
      return 1;
    }
    a = a - 1;
    b = b + dir;
  }
  return 0;
}

int queen(int row) {
  if (diag(row, pos[row], 1)) {
    return 0;
  }
  if (diag(row, pos[row], -1)) {
    return 0;
  }
  if (row == 7) {
    return 1;
  }
  int used[8];
  int i;
  i = 0;
  while (i < 8) {
    used[i] = 0;
    i = i + 1;
  }
  i = 0;
  while (i <= row) {
    used[pos[i]] = 1;
    i = i + 1;
  }
  int sum;
  sum = 0;
  i = 0;
  while (i < 8) {
    if (used[i] == 0) {
      pos[row + 1] = i;
      sum = sum + queen(row + 1);
    }
    i = i + 1;
  }
  return sum;
}

int main() {
  int i;
  int sum;
  i = 0;
  sum = 0;
  while (i < 8) {
    pos[0] = i;
    sum = sum + queen(0);
    i = i + 1;
  }
  return sum;
}
