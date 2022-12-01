
#ifndef VECTOR_H
#define VECTOR_H

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

void *vector_create(unsigned int element_size);
void *vector_create_init(unsigned int element_size, unsigned int initial_size);
void vector_free(void **vector);
void *vector_push(void **vector);

/*
 * Static Functions:
 * static VecData *vector_get_data(void *vector);
 * static int vector_has_space(VecData *v_data);
 * static void vector_expand(VecData **v_data);
 */

#endif // VECTOR_H



#ifdef VECTOR_IMPLEMENTATION

// calculate the nearest power of 2 and round up specifically for 32 bit unsigned integers
static int __pot(unsigned int x) {
  x--; // decrement x (flip lowest bit)
  x |= x >> 1;  // x |= x / 2 * 1
  x |= x >> 2;  // x |= x / 2 * 2
  x |= x >> 4;  // x |= x / 2 * 4
  x |= x >> 8;  // x |= x / 2 * 8
  x |= x >> 16; // x |= x / 2 * 16
  x++;

  return x;
}

void *vector_create(unsigned int element_size) {
	VecData* v = calloc(1, sizeof(VecData));
  v->element_size = element_size;
  v->capacity = 0;
  v->length = 0;
	v->bytes_alloc = v->capacity * element_size; // also 0

	return &v->buffer;
}

void *vector_create_init(unsigned int element_size, unsigned int initial_size) {
  initial_size = __pot(initial_size);
  
	VecData* v = calloc(1, sizeof(VecData) + initial_size * element_size);
  v->element_size = element_size;
  v->capacity = initial_size; // create a capacity rounded up to a multiple of 2 from initial_size
  v->length = 0;
	v->bytes_alloc = v->capacity * element_size; // also 0

  //printf("Closest upwards power of 2: %u\n", __pot(initial_size));
  
	return &v->buffer;
}

void vector_free(void **vector) {
  VecData *v_data = vector_get_data(*vector);
  free(v_data);
  *vector = NULL;
}

static VecData *vector_get_data(void *vector) {
  // total hack for pointer arithmetic
  // cast vector to VecData*, then index back by 1
  // as the pointer being indexed is of type VecData,
  // it decrements back to the address of the start of the structure
  return &((VecData *)vector)[-1];
}

static int vector_has_space(VecData *v_data) {
  //assert(v_data->bytes_alloc == (v_data->capacity * v_data->element_size));
  return (v_data->bytes_alloc - (v_data->length * v_data->element_size)) > 0;
}

static void vector_expand(VecData **v_data) {
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

  
  if(!vector_has_space(v_data)) {
    vector_expand(&v_data);
  }

  v_data->length += 1;
  if(!vector_has_space(v_data)) {
    vector_expand(&v_data);
  }

  // make sure to increment the length
  *vector = &v_data->buffer;
  // return a void pointer to the next available slot in the vector (0 indexed, sub1)
  void *res = *vector + v_data->element_size * (v_data->length-1);

  // is the new increase in length invalid?
  if(!vector_has_space(v_data)){
    vector_expand(&v_data);
  }
  
  return res;
}

#endif // VECTOR_IMPLEMENTATION

/* TODO:
 * - Implement the following functions:
 *   - vector_pop_front(); -> return a copy of the value before mallocing and vector+sizeof(element) it in memcpy
 *   - vector_pop_back();  -> return a copy of the value before length-1 in memcpy
 *   - vector_remove(int index); -> delete the given index from the vector
 *   - vector_clear(void *default_value) -> create a default value and pass its address to set everything to it
 *   - ... more I can't think of right now
 *
 * - Implement a way to choose an allocator (ie... preprocessor defines to pick a malloc definition)
 */
