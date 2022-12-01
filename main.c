#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define VECTOR_IMPLEMENTATION
#include "vector.h"
#undef VECTOR_IMPLEMENTATION

int main(void) {
  int *is = vector_create(sizeof(int));

  for(int i = 0; i < 55; i++) {
    *(int*)vector_push(&is) = i;
  }
  
  for(int i = 0; i < 55; i++) {
    printf("Vector at %d: %d\n", i, is[i]);
  }

  free(vector_get_data(is));
  
  return 0;
}

