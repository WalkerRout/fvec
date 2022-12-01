
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define VECTOR_IMPLEMENTATION
#include "vector.h"
#undef VECTOR_IMPLEMENTATION

int main(void) {
  // create fat pointer
  int *is = vector_create(sizeof(int));

  // fill fat pointer (allocating new memory when necessary)
  for(int i = 0; i < 55; i++) {
    *(int*)vector_push(&is) = i;
  }
  // print guts
  for(int i = 0; i < 55; i++) {
    printf("Vector at %d: %d\n", i, is[i]);
  }
  
  // free fat pointer
  free(vector_get_data(is));

  // create initialized fat pointer with capacity of 100 and length 0
  is = vector_create_init(sizeof(int), 100);

  // fill fat pointer (not allocating any new memory, as the capacity is the nearest highest power of 2 from the given initial size)
  for(int i = 0; i < 100; i++) {
    is[i] = i;
  }
  // print guts (pt2)
  for(int i = 0; i < 100; i++) {
    printf("Vector at %d: %d\n", i, is[i]);
  }

  // free fat pointer
  free(vector_get_data(is));
  is = NULL;
  
  return 0;
}

