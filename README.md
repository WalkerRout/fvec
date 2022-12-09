# fvec
> Dynamically sized vector in C, implemented using fat pointers.<br>
> **Forewarning:** This is probably the hackiest project I have ever written, I mainly wrote it for fun/as an exercise, so the performance may not be fantastic (though, from my experience, it is quite fast).
> If performance is the goal, use fvec_pop_back and FVEC_NO_RESIZE mode (other fvec_pop_* functions use memcpy to shift elements in O(n) time).

Standard example with automatic reallocations:

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
  return *(int*)i % 2 == 1;
}

void print(void *i) {
  printf("%d ", *(int*)i);
}

int main(void) {
  int *data = fvec(sizeof(int));
  
  *(int*)fvec_push(&data) = 0;
  *(int*)fvec_push(&data) = 1;
  *(int*)fvec_push(&data) = 2;
  *(int*)fvec_push(&data) = 3;
  *(int*)fvec_push(&data) = 4;
  *(int*)fvec_push(&data) = 5;
  
  fvec_pop(&data, 2); // removes element at index 2 (2), now [0, 1, 3, 4, 5]
  fvec_pop_back(&data); // removes last (5), now [0, 1, 3, 4], length = 4
  fvec_pop_front(&data); // removes first (0), now [1, 3, 4], length = 3
  
  // map a function to triple all values in the vector
  fvec_map(data, triple); // [1, 3, 4] -> [3, 9, 12]
  
  // fold a binary function over data to sum all values
  int result = 0;
  fvec_fold(data, &result, sum); // <result> + 3 + 9 + 12 = 24
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

Example without automatic reallocations:

```C
#include <stdio.h>

#define FVEC_IMPLEMENTATION
#define FVEC_NO_RESIZE
#include "fvec.h"
#undef  FVEC_IMPLEMENTATION

int main(void) {
  // cap will be made highest closest power of 2
  // ie.. 33 <= x <= 64 -> capacity = 64
  // or alternatively;
  // capacity = 2^(n-1)+1 <= x <= 2^n = 2^n
  int *data = fvecci(sizeof(int), 60); // capacity will be set to 64

  // fill vector
  for(int i = 0; i < fvec_capacity(data); ++i)
    *(int*)fvec_push(&data) = i;

  // remove back half of the elements; need to shrink uneccessary allocation
  for(int i = 0; i < fvec_capacity(data) / 2; ++i)
    fvec_pop_back(&data);
    
  printf("Initial cap %d, len: %d\n", fvec_capacity(data), fvec_length(data));
  // "Initial cap 64, len: 32\n" (cap is always a power of 2)

  // shrink the allocation to fit current length
  fvec_shrink_to_fit(&data);
  
  printf("After cap %d, len: %d\n", fvec_capacity(data), fvec_length(data));
  // "After cap 32, len: 32\n"
  
  fvec_free(&data);
  
  return 0;
}
```

**Run with `make`**
