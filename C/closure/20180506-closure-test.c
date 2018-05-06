#include <stdio.h>

typedef struct {
  int *a;
  int *b;
  int (*f)(a, b, c);
} closure_t;

void map(closure_t *p, int x[], int len) {
  for(int i = 0; i < len; i++) 
  {
    x[i] = (*p->f)(*p->a, *p->b, x[i]); 
  }
}

int f(int a, int b, int c) {
  return a*c + b;
}

int main() {
  int a = 3;
  int b = 1;
  int l[] = {1, 2, 3, 4, 5};
  closure_t closure = {
    &a, &b, f 
  };
  map(&closure, l, 5);
  for(int i = 0; i < 5; i++) {
    printf("%d\n", l[i]);
  }
  return 0;
}