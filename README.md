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
  
  // triple all values in the vector
  fvec_map(data, triple);
  
  int result = 0;
  fvec_foldr(data, &result, sum);
  
  // prints '3 6 9 \n' -> newline is automatically inserted
  // (might change later, but for now its convenient)
  fvec_print(data, print);
 
  // free the data behind the vector
  fvec_free(&data);
  
  return 0;
}
```

**Run with `make`**
