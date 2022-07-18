#include <stdio.h>

int main() {
  unsigned long count = 0;
  unsigned long num = 2;

  while (count < 987654) {
    while (num < 987654) {
      num = (num * 2);
    }
    count = (count + 1);
  }

  printf("%lu\n", num);

  return 0;
}
