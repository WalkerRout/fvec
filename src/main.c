#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct _VecData {
  unsigned int element_size; // size of each element
  unsigned int capacity;     // number of elements that can fit
  unsigned int bytes_alloc;  // element_size * capacity
  unsigned int length;       // element count in the vector
  unsigned char buffer[];
} VecData;

void *vector_create(unsigned int element_size) {
  VecData* v = calloc(1, sizeof(VecData) + element_size);
  v->element_size = element_size;
  v->capacity = 0;
  v->length = 0;
  v->bytes_alloc = v->capacity * element_size; // also 0

  return &v->buffer;
}

VecData *vector_get_data(void *vector) {
  // total hack for pointer arithmetic
  // cast vector to VecData*, then index back by 1
  // as the pointer being indexed is of type VecData,
  // it decrements back to the address of the start of the structure
  return &((VecData *)vector)[-1];
}

int vector_has_space(VecData *v_data) {
  //assert(v_data->bytes_alloc == (v_data->capacity * v_data->element_size));
  return (v_data->bytes_alloc - (v_data->length * v_data->element_size)) > 0;
}

void vector_expand(VecData **v_data) {
  assert((*v_data)->capacity >= (*v_data)->length);
  assert((*v_data)->bytes_alloc == ((*v_data)->capacity * (*v_data)->element_size));

  int old_bytes = sizeof(**v_data) + (*v_data)->bytes_alloc;
  
  if((*v_data)->bytes_alloc == 0) {
    (*v_data)->capacity = 2;
    (*v_data)->bytes_alloc = (*v_data)->capacity * (*v_data)->element_size;
  } else {
    (*v_data)->capacity *= 2;
    (*v_data)->bytes_alloc = (*v_data)->capacity * (*v_data)->element_size;
  }

  //fprintf(stderr, "Reallocating from %d bytes to %d bytes!\n", old_bytes, sizeof(**v_data) + (*v_data)->bytes_alloc);

  *v_data = realloc(*v_data, sizeof(VecData) + (*v_data)->bytes_alloc);
}

void *vector_push(void **vector) {
  VecData *v_data = vector_get_data(*vector);
  
  if(!vector_has_space(v_data)){
    vector_expand(&v_data);
  }
  
  v_data->length += 1;
  *vector = &v_data->buffer;
  return *vector;
}

#define vec_push(vec) vector_push(&(vec)); (vec)

int main(void) {
  int *is = vector_create(sizeof(int));

  for(int i = 0; i < 55; i++) {
    vec_push(is)[i] = i + 2; // or ((int *)vector_push(&is))[i] = 2;
  }
  
  for(int i = 0; i < 55; i++) {
    printf("Vector at %d: %d\n", i, is[i]);
  }
  
  return 0;
}


