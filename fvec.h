/*
** @title:        FVec (Fat Pointer Vectors)
**
** @author:       Walker Rout
**
** @contact:      walkerrout04@gmail.com
**
** @date_written: December 6th, 2022
**
** @description:  This library implements fat pointer vectors that are able
**                to theoretically hold anything of the same size. It goes
**                without saying that this entire library is quite hacky,
**                probably non portable, but cool as shit.
**
** @WARNING:      TAKE EXTREME CAUTION WHEN USING THIS LIBRARY. ALL BEHAVIOUR
**                IS DECIDED BY THE USER.
**
** @license:      MIT
*/

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifndef FVEC_H
#define FVEC_H

// -----------------------------------------
/*
** INCLUDES
*/
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
// -----------------------------------------

// -----------------------------------------
/*
** ACCESS MODIFIER DEFINES
*/
#ifndef FVECDEF
// static inline
#define FVECDEF
#endif // FVECDEF
// -----------------------------------------

// -----------------------------------------
/*
** FAT POINTER VECTOR DATA FORWARD DEFINITION
*/
typedef struct _FVecData FVecData;
// -----------------------------------------

// -----------------------------------------
/*
** FAT POINTER VECTOR FUNCTION PROTOTYPES
*/
FVECDEF void *fvec(unsigned int element_size);
FVECDEF void *fvecci(unsigned int element_size, unsigned int initial_size);
FVECDEF FVecData *fvec_get_data(void *vector);
//FVECDEF int fvec_has_space(FVecData *v_data);
//FVECDEF void fvec_expand(FVecData **v_data);
FVECDEF void *fvec_push(void **vector);
FVECDEF void fvec_pop_back(void **vector);
FVECDEF void fvec_map(void *vector, void(*func)(void*));
FVECDEF unsigned int fvec_length(void *vector);
FVECDEF void fvec_free(void **vector);
FVECDEF void fvec_print(void *vector, void(*print_func)(void*));
// -----------------------------------------
#endif // FVEC_H

/*
** HEADER ONLY IMPLEMENTATION
*/
#ifdef FVEC_IMPLEMENTATION

// -----------------------------------------
/*
** FAT POINTER VECTOR DATA DEFINITION
*/
typedef struct _FVecData {
  unsigned int element_size; // size of each element
  unsigned int capacity;     // number of elements that can fit
  unsigned int bytes_alloc;  // element_size * capacity
  unsigned int length;       // element count in the vector
  unsigned char buffer[];
} FVecData;
// -----------------------------------------

// -----------------------------------------
/*
** HELPER FUNCTIONS DEFINITIONS
*/

/*
** @WARNING: !!! THIS FUNCTION IS LOCAL TO THE FILE IN WHICH FVEC IS DEFINED !!!
**
** @brief:   Produce the nearest highest power of two
** @params:  x {unsigned int} - initial value
** @returns: {unsigned int} - power of two closest to and greater than initial value
*/
static inline unsigned int pot(unsigned int x) {
  // from here (great read): https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
  
  // move all bits down while or'ing them in order to fill all
  // lower bits with 1's, effectively making x 1 less than a
  // power of 2
  // ie.. 43 after x |= x >> 1 -> 0000000000111111 (63)
  // this step continues, with no other bits changing, until a 1 is
  // added at the end
  
  x--; // decrement x (flip lowest bit)
  x |= x >> 1;
  x |= x >> 2;
  x |= x >> 4;
  x |= x >> 8;
  x |= x >> 16;
  x++; // round up to power of 2
  
  return x;
}

// -----------------------------------------

// -----------------------------------------
/*
** FAT POINTER VECTOR FUNCTION DEFINITIONS
*/

/*
** @brief:   Create a fat pointer vector able to contain elements of size element_size
** @params:  element_size {unsigned int} - size of each element in the vector
** @returns: {void *} - pointer to buffer of vector
*/
FVECDEF void *fvec(unsigned int element_size) {
  FVecData* v = calloc(1, sizeof(FVecData));
  v->element_size = element_size;
  v->capacity = 0;
  v->length = 0;
  v->bytes_alloc = v->capacity * element_size; // also 0

  return &v->buffer;
}

/*
** @brief:   Create a fat pointer vector able to contain elements of size element_size, with initial allocation of initial_size
** @params:  element_size {unsigned int} - size of each element in the vector, initial_size {unsigned int} - number of initial allocated slots for elements in the vector
** @returns: {void *} - pointer to buffer of vector
*/
FVECDEF void *fvecci(unsigned int element_size, unsigned int initial_size) {
  initial_size = pot(initial_size);
  
  FVecData* v = calloc(1, sizeof(FVecData) + initial_size * element_size);
  v->element_size = element_size;
  v->capacity = initial_size; // create a capacity rounded up to a multiple of 2 from initial_size
  v->length = 0;
  v->bytes_alloc = v->capacity * element_size; // also 0
  
  return &v->buffer;
}

/*
** @WARNING: !!! TAKE PRECAUTIONS THAT VECTOR POINTS TO VALID DATA !!!
**
** @brief:   Get the data behind a vector fat pointer
** @params:  vector {void *} - fat pointer pointing to a buffer to underlying data
** @returns: {FVecData *} - data behind the fat pointer vector
*/
FVECDEF FVecData *fvec_get_data(void *vector) {
  assert(vector);
  // total hack for pointer arithmetic
  // cast vector to FVecData*, then index back by 1
  // as the pointer being indexed is of type FVecData,
  // it decrements back to the address of the start of the structure
  return &((FVecData *)vector)[-1];
}

/*
** @brief:   Clone a fat pointer vector
** @params:  vector {void *} - fat pointer vector to clone
** @returns: new_vector {void *} - new fat pointer vector pointing to separate memory
*/
FVECDEF void *fvec_clone(void *vector) {
  assert(vector);
  FVecData *v_data = fvec_get_data(vector);
  void *new_vector = fvecci(v_data->element_size, v_data->length);
  
  memcpy(new_vector, vector, v_data->element_size * v_data->length);
  
  return new_vector;
}

/*
** @brief:   Check if a vector has space for another element
** @params:  v_data {FVecData *} - vector to check
** @returns: {int} - a boolean value representing whether or not the vector has space
*/
FVECDEF int fvec_has_space(FVecData *v_data) {
  //assert(v_data->bytes_alloc == (v_data->capacity * v_data->element_size));
  return (v_data->bytes_alloc - (v_data->length * v_data->element_size)) > 0;
}

/*
** @brief:   Expand a vector's allocation
** @params:  v_data {FVecData **} - the vector to expand
** @returns: N/A
*/
FVECDEF void fvec_expand(FVecData **v_data) {
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

  *v_data = realloc(*v_data, sizeof(FVecData) + (*v_data)->bytes_alloc);
}

/*
** @brief:   Push a value into the vector
** @params:  vector {void **} - fat pointer vector to push into
** @returns: res {void *} - pointer to new address at the end of the vector
*/
FVECDEF void *fvec_push(void **vector) {
  assert(vector);
  FVecData *v_data = fvec_get_data(*vector);
  
  if(!fvec_has_space(v_data)) {
    fvec_expand(&v_data);
  }

  v_data->length += 1;
  if(!fvec_has_space(v_data)) {
    fvec_expand(&v_data);
  }

  // make sure to increment the length
  *vector = &v_data->buffer;
  // return a void pointer to the next available slot in the vector (0 indexed, sub1)
  void *res = *vector + v_data->element_size * (v_data->length-1);
  
  return res;
}

/*
** @brief:   Remove the last element from a fat pointer vector, shrinking allocation if the length is a power of 2
** @params:  vector {void **} - fat pointer vector to remove an element from
** @returns: N/A
*/
FVECDEF void fvec_pop_back(void **vector) {
  assert(vector);
  FVecData *v_data = fvec_get_data(*vector);
  assert(v_data->length > 0 && "Cannot pop an empty vector!");
  
  v_data->length -= 1;
  
  // if its a power of 2...
  if(ceil(log2(v_data->length)) == floor(log2(v_data->length))) {
    // know length is a power of two, capacity remains consistent
    v_data->capacity = v_data->length;
    v_data->bytes_alloc = v_data->element_size * v_data->length;
    // shrink allocation
    v_data = realloc(v_data, sizeof(FVecData) + v_data->bytes_alloc);
  }

  *vector = &v_data->buffer;
}

/*
** @brief:   Maps a function of type :: Void -> Void* onto each element of a fat pointer vector
** @params:  vector {void *} - fat pointer vector to map, func {void (*)(void*)} - function to apply to each vector element
** @returns: N/A
*/
FVECDEF void fvec_map(void *vector, void (*func)(void*)) {
  assert(vector);
  FVecData *v_data = fvec_get_data(vector);
  
  for(int i = 0; i < v_data->length; ++i)
    func(vector + i * v_data->element_size); // vector still valid -> no reallocations yet
}

/*
** @brief:   Free a fat pointer vector (also sets pointer to NULL)
** @params:  vector {void **} - reference to vector to free
** @returns: N/A 
*/
FVECDEF void fvec_free(void **vector) {
  FVecData *v_data = fvec_get_data(*vector);
  free(v_data);
  *vector = NULL;
}

/*
** @brief:   Get the length of a fat pointer vector
** @params:  vector {void *} - target for length
** @returns: {unsigned int} - length of the vector
*/
FVECDEF unsigned int fvec_length(void *vector) {
  FVecData *v_data = fvec_get_data(vector);
  return v_data->length;
}

/*
** @brief:   Print a fat pointer vector
** @params:  vector {void *} - vector to print, print_func {void(*)(void*)} - function pointer to user function for printing elements (no newline expected)
** @returns: N/A
*/
FVECDEF void fvec_print(void *vector, void(*print_func)(void*)) {
  assert(vector);
  FVecData *v_data = fvec_get_data(vector);
  for(int i = 0; i < v_data->length; ++i)
    print_func(vector + i * v_data->element_size); // vector still valid -> no reallocations yet
  printf("\n");
}

// -----------------------------------------

#endif // FVEC_IMPLEMENTATION

/*
** TODO:
** - fvec_pop_front(); -> malloc and vector+sizeof(element) it in memcpy
** - fvec_foldr(); -> binary function with rsf and current element
** - fvec_filter(); -> no clue yet
** - fvec_remove(int index); -> delete the given index from the vector
** - fvec_clear(void *default_value) -> create a default value and pass its address to set everything to it
** - fvec_shrink_to(unsigned int new_length) -> drop all elements after new_length (easy with realloc)
** - ... more I can't think of right now
**
**
*/
