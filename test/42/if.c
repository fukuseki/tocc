int main() {
  int i = 0;
  if (1) {
    i = 40;
  } else {
    i = 3;
  }
  if (0) {
    i = 5;
  } else {
    i = i + 2;
  }
  return i;
}
