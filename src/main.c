#include <stdio.h>

#define FVEC_IMPLEMENTATION
#include "fvec.h"
#undef  FVEC_IMPLEMENTATION

void triple(void *i) {
  *(int*)i *= 3;
}

void sum(void *curr, void *rsf) {
  *(int*)rsf += *(int*)curr;
}

int is_even(void *i) {
  return *(int*)i % 2 == 0;
}

void print(void *i) {
  printf("%d ", *(int*)i);
}

int main(void) {
  int *data = fvec(sizeof(int));
  
  *(int*)fvec_push(&data) = 9;
  *(int*)fvec_push(&data) = -2;
  *(int*)fvec_push(&data) = 1;
  *(int*)fvec_push(&data) = 2;
  *(int*)fvec_push(&data) = -4;
  *(int*)fvec_push(&data) = 5;
  
  // triple all values in the vector
  fvec_map(data, triple);

  // 27 - 6 + 3 + 6 - 12 + 15 = 33, will print "Result is: 33"
  int result = 0;
  fvec_foldr(data, &result, sum);
  printf("Result is: %d\n", result);

  // grab all even number in the vector and insert them into the back of evens
  int *evens = fvec(sizeof(int));
  fvec_filter(&evens, data, is_even);
  
  // print the vectors using their print functions; a newline is automatically inserted following each print
  fvec_print(data, print);
  fvec_print(evens, print);
 
  // free the data behind the vector
  fvec_free(&data);
  fvec_free(&evens);
  
  return 0;
}
