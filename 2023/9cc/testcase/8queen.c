int pos[8];

int diag(int i, int dir) {
  int x;
  x = i - 1;
  int y;
  y = pos[i] + dir;
  while (x >= 0) {
    if (pos[x] == y) {
      return 1;
    }
    x = x - 1;
    y = y + dir;
  }
  return 0;
}

int queen(int row) {
  if (diag(row, 1)) {
    return 0;
  }
  if (diag(row, 0)) {
    return 0;
  }
  if (diag(row, -1)) {
    return 0;
  }
  if (row == 7) {
    return 1;
  }
  int i;
  int sum;
  sum = 0;
  i = 0;
  while (i < 8) {
    pos[row + 1] = i;
    sum = sum + queen(row + 1);
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
  printnum(sum);
  return sum;
}
