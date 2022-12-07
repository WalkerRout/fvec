
#include <time.h>
#include <stdio.h>
#include <string.h>

#define FVEC_IMPLEMENTATION
#include "../fvec.h"
#undef  FVEC_IMPLEMENTATION

void print(void *i) {
  printf("%d ", *(int*)i);
}

void sum(void *curr, void *rsf) {
  // add the current element onto the result of the natural recursion
  *(int*)rsf += *(int*)curr;
}

int main(void) {
  // this is not guaranteed to be null terminated...
  // only fvec_* functions are safe on this string
  int *test = fvec(sizeof(int));
  *(int*)fvec_push(&test) = 1;
  *(int*)fvec_push(&test) = 2;
  *(int*)fvec_push(&test) = 3;
  *(int*)fvec_push(&test) = 4;
  *(int*)fvec_push(&test) = 5;

  int result = 0;
  fvec_foldr(test, &result, sum);

  // 1 + 2 + 3 + 4 + 5 = 15
  printf("Result is: %d\n", result);

  fvec_print(test, print);
  fvec_free(&test);
  
  return 0;
}

