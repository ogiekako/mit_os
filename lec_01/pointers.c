#include <stdio.h>
#include <stdlib.h>

  void
f(void)
{
  int a[4];
  int *b = malloc(16);
  int *c;
  int i;

  // array is decayed to its pointer when passed to a function.
  // a -> stack, b -> heap, c: undefined
  printf("1: a = %p, b = %p, c = %p\n", a, b, c);

  // pointer decay from array to pointer
  c = a;
  for (i = 0; i < 4; i++)
    a[i] = 100 + i;
  c[0] = 200;
  printf("2: a[0] = %d, a[1] = %d, a[2] = %d, a[3] = %d\n",
      a[0], a[1], a[2], a[3]);

  // x[i] is by definition, *((x) + (i)) .
  c[1] = 300;
  *(c + 2) = 301;
  3[c] = 302;
  printf("3: a[0] = %d, a[1] = %d, a[2] = %d, a[3] = %d\n",
      a[0], a[1], a[2], a[3]);

  c = c + 1;
  *c = 400;
  printf("4: a[0] = %d, a[1] = %d, a[2] = %d, a[3] = %d\n",
      a[0], a[1], a[2], a[3]);

  c = (int *) ((char *) c + 1);
  *c = 500;
  printf("5: a[0] = %d, a[1] = %d, a[2] = %d, a[3] = %d\n",
      a[0], a[1], a[2], a[3]);

  // 1, 4
  printf("char: %lu, int: %lu\n", sizeof(char), sizeof(int));
  b = (int *) a + 1;
  c = (int *) ((char *) a + 1);
  // b = a + 4 (byte)
  // c = a + 1 (byte)
  printf("6: a = %p, b = %p, c = %p\n", a, b, c);
}

  int
main(int ac, char **av)
{
  f();
  return 0;
}

