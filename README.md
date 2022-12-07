# fvec
> Dynamically sized vector in C, implemented using fat pointers

Example:

```C
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

int is_odd(void *i) {
  return *(int*)i % 2 == 0;
}

void print(void *i) {
  printf("%d ", *(int*)i);
}

int main(void) {
  int *data = fvec(sizeof(int));
  
  *(int*)fvec_push(&data) = 1;
  *(int*)fvec_push(&data) = 2;
  *(int*)fvec_push(&data) = 3;
  *(int*)fvec_push(&data) = 4;
  fvec_pop_back(&data); // removes last (4), now [1, 2, 3], length = 3
  
  // map a function to triple all values in the vector
  fvec_map(data, triple);
  
  // fold a binary function over data to sum all values
  int result = 0;
  fvec_fold(data, &result, sum); // 3 + 6 + 9 = 18
  printf("Result is: %d\n", result);
  
  // create a new vector filled with all odd numbers in data
  int *odds = fvec(sizeof(int));
  fvec_filter(&odds, data, is_odd);
  
  // print the vectors
  fvec_print(data, print);
  fvec_print(odds, print);
 
  // free the data behind the vectors
  fvec_free(&data);
  fvec_free(&odds);
  
  return 0;
}
```

**Run with `make`**
