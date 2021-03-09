// #include <stdio.h>
// #include <stdlib.h>

int n = 8;

int print_row(int queen) {
  int i;
  for (i = 0; i < n; i = i + 1) {
    if (queen == i) {
      printf("Q");
    } else {
      printf(".");
    }
  }
  printf("\n");
  return 0;
}

int print_board(int* board) {
  int i;
  for (i = 0; i < n; i = i + 1) {
    print_row(board[i]);
  }
  printf("\n");
  return 0;
}

int can_placed(int* board, int current_row) {
  int ci = board[current_row];
  int ri;
  for (ri = 0; ri < current_row; ri = ri + 1) {
    // 重複チェック 縦
    if (board[ri] == ci) {
      return 0;
    }
    // 重複チェック 斜め
    if (abs(board[ri] - ci) == current_row - ri) {
      return 0;
    }
  }
  return 1;
}

int nqueen(int* board, int current_row) {
  int ci;
  for (ci = 0; ci < n; ci = ci + 1) {
    board[current_row] = ci;
    if (can_placed(board, current_row)) {
      if (current_row == n - 1) {
        print_board(board);
      } else {
        nqueen(board, current_row + 1);
      }
    }
  }
  return 0;
}

int main() {
  int* board = malloc(n * 4);
  nqueen(board, 0);
  free(board);
  return 0;
}
