#include <stdio.h>

#define FVEC_IMPLEMENTATION
#include "../fvec.h"
#undef  FVEC_IMPLEMENTATION

void triple(void *i) {
  *(int*)i *= 3;
}

void sum(void *curr, void *rsf) {
  *(int*)rsf += *(int*)curr;
}

int is_odd(void *i) {
  return *(int*)i % 2 == 1;
}

void print(void *i) {
  printf("%d ", *(int*)i);
}

int main(void) {
  int *data = fvec(sizeof(int));
  
  *(int*)fvec_push(&data) = 9;
  *(int*)fvec_push(&data) = -2;
  *(int*)fvec_push(&data) = 1;
  fvec_pop_front(&data);
  *(int*)fvec_push(&data) = 2;
  *(int*)fvec_push(&data) = -4;
  fvec_pop_back(&data);
  *(int*)fvec_push(&data) = 5;
  *(int*)fvec_push(&data) = 57;

  fvec_pop(&data, 3);
  fvec_print(data, print);
  
  fvec_map(data, triple);

  int result = 0;
  fvec_fold(data, &result, sum);
  printf("Result is: %d\n", result);

  int *odds = fvec(sizeof(int));
  fvec_filter(&odds, data, is_odd);
  
  fvec_print(data, print);
  fvec_print(odds, print);
 
  fvec_free(&data);
  fvec_free(&odds);
  
  return 0;
}
