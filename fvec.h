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
**                probably non portable, and pretty dang neat.
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
// -----------------------------------------

// -----------------------------------------
/*
** ACCESS MODIFIER DEFINES
*/
#ifndef FVECDEF
// if want to make use of inline with FVEC_IMPLEMENTATION defined in another file, use: #define FVECDEF extern inline
#define FVECDEF
#endif // FVECDEF
  
#ifndef FVECHELP
#define FVECHELP static inline
#endif // FVECHELP
// -----------------------------------------

// -----------------------------------------
/*
** POSSIBLE FEATURE FLAGS
*/
// - FVEC_NO_RESIZE
// - ...
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
FVECDEF void *fvec_from_ptr(void *ptr, unsigned int length, unsigned int element_size);
FVECDEF FVecData *fvec_get_data(void *vector);
FVECDEF void *fvec_get(void *vector, unsigned int index);
FVECDEF void *fvec_push(void **vector);
FVECDEF void fvec_pop_back(void **vector);
FVECDEF void fvec_pop_front(void **vector);
FVECDEF void fvec_pop(void **vector, unsigned int index);
FVECDEF void fvec_shrink_to_fit(void **vector);
FVECDEF void fvec_clear(void *vector, void *default_value);
FVECDEF void fvec_map(void *vector, void(*func)(void*));
FVECDEF void fvec_filter(void *src_vector, void **dest_vector, int(*predicate)(void*));
FVECDEF void fvec_fold(void *vector, void *base, void(*binop)(void*, void*));
FVECDEF unsigned int fvec_length(void *vector);
FVECDEF unsigned int fvec_element_size(void *vector);
FVECDEF unsigned int fvec_capacity(void *vector);
FVECDEF unsigned int fvec_bytes_alloc(void *vector);
FVECDEF void fvec_free(void **vector);
FVECDEF void fvec_print(void *vector, void(*print_func)(void*));
// Helpers:
// - FVECHELP unsigned int pot(unsigned int x);
// - FVECHELP int is_pot(unsigned int x);
// - FVECHELP int fvec_has_space(FVecData *v_data);
// - FVECHELP void fvec_expand(FVecData **_v_data);
// - FVECHELP void __fvec_expand_nr(FVecData **_v_data); // feature flag
// - FVECHELP void fvec_shrink(FVecData **_v_data);
// - FVECHELP void __fvec_shrink_nr(FVecData **_v_data); // feature flag
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
** @brief:   Produce the nearest highest power of two
** @params:  x {unsigned int} - initial value
** @returns: {unsigned int} - power of two closest to and greater than initial value
*/
FVECHELP unsigned int pot(unsigned int x) {
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

/*
** @brief:   Check if a number is a power of 2
** @params:  x {unsigned int} - number to check
** @returns: {int} - a boolean value representing whether or not the number is a power of 2
*/
FVECHELP int is_pot(unsigned int x) {
  return ceil(log2(x)) == floor(log2(x));
}

/*
** @brief:   Check if a vector has space for another element
** @params:  v_data {FVecData *} - vector to check
** @returns: {int} - a boolean value representing whether or not the vector has space
*/
FVECHELP int fvec_has_space(FVecData *v_data) {
  return ((long int)v_data->bytes_alloc - ((long int)v_data->length * (long int)v_data->element_size)) > 0;
}

/*
** @brief:   Expand a vector's allocation
** @params:  _v_data {FVecData **} - the vector to expand
** @returns: N/A
*/
FVECHELP void fvec_expand(FVecData **_v_data) {
  FVecData *v_data = *_v_data;
  assert(v_data->capacity >= v_data->length);
  assert(v_data->bytes_alloc == (v_data->capacity * v_data->element_size));

  if(v_data->bytes_alloc == 0) {
    v_data->capacity = 2;
    v_data->bytes_alloc = v_data->capacity * v_data->element_size;
  } else {
    v_data->capacity *= 2;
    v_data->bytes_alloc = v_data->capacity * v_data->element_size;
  }

  *_v_data = realloc(*_v_data, sizeof(FVecData) + v_data->bytes_alloc);
  if(*_v_data == NULL) {
    fprintf(stderr, "Unable to reallocate vector after expanding vector!\n");
    exit(1);
  }
}

/*
** @FFs:      !!! FEATURE FLAGS !!!
**
** @brief:   Expand a vector's allocation in no-resize mode -> no-op
** @params:  _v_data {FVecData **} - the vector to no-op
** @returns: N/A
*/
#ifdef FVEC_NO_RESIZE
#define fvec_expand __fvec_expand_nr
#endif // FVEC_NO_RESIZE
FVECHELP void __fvec_expand_nr(FVecData **_v_data) {
  FVecData *v_data = *_v_data;
  assert(v_data->length <= v_data->capacity && "Invalid operation in no-resize mode! No more room in vector!");
  assert(v_data->bytes_alloc == (v_data->capacity * v_data->element_size));
}

/*
** @brief:   Shrink a vector's allocation if the length is a power of 2
** @params:  _v_data {FVecData **} - the vector to expand
** @returns: N/A
*/
FVECHELP void fvec_shrink(FVecData **_v_data) {
  FVecData *v_data = *_v_data;
  assert(v_data->capacity >= v_data->length);
  assert(v_data->bytes_alloc == (v_data->capacity * v_data->element_size));

  // know length is a power of two, capacity remains consistent
  v_data->capacity = v_data->length;
  v_data->bytes_alloc = v_data->element_size * v_data->length;
  // shrink allocation
  *_v_data = realloc(*_v_data, sizeof(FVecData) + v_data->bytes_alloc);
  if(v_data == NULL) {
    fprintf(stderr, "Unable to reallocate vector after shrinking vector!\n");
    exit(1);
  }
}

/*
** @FFs:      !!! FEATURE FLAGS !!!
**
** @brief:   Shrink a vector's allocation in no-resize mode -> no-op
** @params:  _v_data {FVecData **} - the vector to no-op
** @returns: N/A
*/
#ifdef FVEC_NO_RESIZE
#define fvec_shrink __fvec_shrink_nr
#endif // FVEC_NO_RESIZE
FVECHELP void __fvec_shrink_nr(FVecData **_v_data) {
  FVecData *v_data = *_v_data;
  assert(v_data->length <= v_data->capacity && "Invalid operation in no-resize mode! No more room in vector!");
  assert(v_data->bytes_alloc == (v_data->capacity * v_data->element_size));
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
  if(v == NULL) {
    fprintf(stderr, "Unable to calloc vector in fvec!\n");
    exit(1);
  }
  
  v->element_size = element_size;
  v->capacity = 1;
  v->length = 0;
  v->bytes_alloc = v->capacity * element_size;

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
  if(v == NULL) {
    fprintf(stderr, "Unable to calloc vector in fvecci!\n");
    exit(1);
  }
  
  v->element_size = element_size;
  v->capacity = initial_size; // create a capacity rounded up to a multiple of 2 from initial_size
  v->length = 0;
  v->bytes_alloc = v->capacity * element_size;
  
  return &v->buffer;
}

/*
** @WARNING: !!! TAKE PRECAUTIONS THAT PTR POINTS TO VALID DATA WITH A PROPER LENGTH !!!
**
** @brief:   Create a fat pointer vector from an initial array provided by ptr
** @params:  ptr {void *} - pointer to data to copy into a vector, length {unsigned int} - the length of ptr, element_size {unsigned int} - the size of each element in ptr
** @returns: {void *} - pointer to buffer of vector
*/
FVECDEF void *fvec_from_ptr(void *ptr, unsigned int length, unsigned int element_size) {
  assert(ptr);
  
  unsigned int initial_size = pot(length);
  
  FVecData* v = calloc(1, sizeof(FVecData) + initial_size * element_size);
  if(v == NULL) {
    fprintf(stderr, "Unable to calloc vector in fvec_from_ptr!\n");
    exit(1);
  }
  
  v->element_size = element_size;
  v->capacity = initial_size; // create a capacity rounded up to a multiple of 2 from initial_size
  v->length = length;
  v->bytes_alloc = v->capacity * element_size;

  memcpy(&v->buffer, ptr, element_size * length);
  
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
** @WARNING: !!! TAKE PRECAUTIONS THAT THE RETURN VALUE IS CAST TO THE PROPER DATATYPE !!!
**
** @brief:   Get an element out of a generic fat pointer vector
** @params:  vector {void *} - fat pointer with desired element,
** @returns: {void *} - element at target index
*/
FVECDEF void *fvec_get(void *vector, unsigned int index) {
  assert(vector);
  FVecData *v_data = fvec_get_data(vector);
  assert(index < v_data->length && "Index out of bounds! Cannot access beyond length!");

  return v_data->buffer + (index * v_data->element_size);
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
** @brief:   Push a value into the vector
** @params:  vector {void **} - fat pointer vector to push into
** @returns: res {void *} - pointer to new address at the end of the vector
*/
FVECDEF void *fvec_push(void **vector) {
  assert(vector);
  FVecData *v_data = fvec_get_data(*vector);

  if(!fvec_has_space(v_data))
    fvec_expand(&v_data);

  v_data->length += 1;
  if(!fvec_has_space(v_data))
    fvec_expand(&v_data);

  // make sure to increment the length
  *vector = &v_data->buffer;
  
  // return a void pointer to the next available slot in the vector (0 indexed, sub1)
  return *vector + v_data->element_size * (v_data->length - 1);
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
  if(is_pot(v_data->length))
    fvec_shrink(&v_data);

  *vector = &v_data->buffer;
}

/*
** @brief:   Remove the first element from a fat pointer vector, shrinking allocation if the length is a power of 2
** @params:  vector {void **} - fat pointer vector to remove an element from
** @returns: N/A
*/
FVECDEF void fvec_pop_front(void **vector) {
  assert(vector);
  FVecData *v_data = fvec_get_data(*vector);
  assert(v_data->length > 0 && "Cannot pop an empty vector!");
  
  v_data->length -= 1;

  void *dest = v_data->buffer;
  void *src  = v_data->buffer + v_data->element_size;
  unsigned int length = v_data->length * v_data->element_size;
  
  memcpy(dest, src, length);
  
  // if its a power of 2...
  if(is_pot(v_data->length))
    fvec_shrink(&v_data);

  *vector = &v_data->buffer;
}

/*
** @brief:   Remove an element from a fat pointer vector, shrinking allocation if the length is a power of 2
** @params:  vector {void **} - fat pointer vector to remove an element from, index {unsigned int} - index of element to remove
** @returns: N/A
*/
FVECDEF void fvec_pop(void **vector, unsigned int index) {
  assert(vector);
  FVecData *v_data = fvec_get_data(*vector);
  assert(v_data->length > 0 && "Cannot pop an empty vector!");
  assert(index < v_data->length && "Index out of bounds! Cannot access beyond length!");

  if(index == v_data->length-1) {
    fvec_pop_back(vector);
    return;
  }
  
  v_data->length -= 1;

  void *dest = v_data->buffer + index * v_data->element_size;
  void *src  = v_data->buffer + (1 + index) * v_data->element_size;
  unsigned int length = v_data->length * v_data->element_size - index * v_data->element_size;

  memcpy(dest, src, length);
  
  // if its a power of 2...
  if(is_pot(v_data->length))
    fvec_shrink(&v_data);

  *vector = &v_data->buffer;
}

/*
** @brief:   Shrink the allocation of a vector to nearest highest power of 2 based on the length (useful with FVEC_NO_RESIZE, though the vec *cannot* be re-expanded as of now)
** @params:  vector {void **} - fat pointer to shrink allocation to length
** @returns: N/A
*/
FVECDEF void fvec_shrink_to_fit(void **vector) {
  FVecData *v_data = fvec_get_data(*vector);

  v_data->capacity = (v_data->length == 0) ? 1 : pot(v_data->length);
  v_data->bytes_alloc = v_data->capacity * v_data->element_size;
  v_data = realloc(v_data, sizeof(FVecData) + v_data->bytes_alloc);

  *vector = &v_data->buffer;
}

/*
** @brief:   Set all element in a vector to a default value
** @params:  vector {void *} - fat pointer vector to clear, default_value {void *} - pointer to default element specified by user
** @returns: N/A
*/
FVECDEF void fvec_clear(void *vector, void *default_value) {
  assert(vector);
  assert(default_value && "Default value may not be NULL!");
  FVecData *v_data = fvec_get_data(vector);

  for(int i = 0; i < v_data->length; ++i)
    memcpy(fvec_get(vector, i), default_value, v_data->element_size);
}

/*
** @brief:   Maps a function onto each element of a fat pointer vector
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
** @brief:   Push items that meet a predicate to the back of a destination vector
** @params:  src_vector {void *} - vector being filtered, dest_vector {void **} - target the elements are inserted into, func {void(*)(void*)} - a predicate to apply to each element of src_vector
** @returns: N/A
*/
FVECDEF void fvec_filter(void *src_vector, void **dest_vector, int(*predicate)(void*)) {
  assert(*dest_vector);
  assert(src_vector);
  FVecData *v_data = fvec_get_data(src_vector);
  FVecData *d_data = fvec_get_data(*dest_vector);
  assert(d_data->element_size == v_data->element_size && "Elements must be of the same size!");

  for(int i = 0; i < v_data->length; ++i) {
    void *curr = src_vector + i * v_data->element_size;
    if(predicate(curr))
      memcpy(fvec_push(dest_vector), curr, v_data->element_size);
  }
}

/*
** @brief:   Perform a right fold over a vector using a recursive base value and binary operator
** @params:  vector {void *} - fat pointer vector to fold, base {void *} - what would be the result of the natural recursion, func {void (*)(void*, void*)} - binary function to fold over each element, storing the current result in the 2nd argument (base)
** @returns: N/A
*/
FVECDEF void fvec_fold(void *vector, void *base, void(*binop)(void*, void*)) {
  assert(vector);
  FVecData *v_data = fvec_get_data(vector);

  for(int i = 0; i < v_data->length; ++i)
    binop(fvec_get(vector, i), base); // vector still valid -> no reallocations yet
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
** @brief:   Get the element_size of a fat pointer vector
** @params:  vector {void *} - target for element_size
** @returns: {unsigned int} - element_size of the vector
*/
FVECDEF unsigned int fvec_element_size(void *vector) {
  FVecData *v_data = fvec_get_data(vector);
  return v_data->element_size;
}

/*
** @brief:   Get the capacity of a fat pointer vector
** @params:  vector {void *} - capacity for length
** @returns: {unsigned int} - capacity of the vector
*/
FVECDEF unsigned int fvec_capacity(void *vector) {
  FVecData *v_data = fvec_get_data(vector);
  return v_data->capacity;
}

/*
** @brief:   Get the bytes_alloc of a fat pointer vector
** @params:  vector {void *} - target for bytes_alloc
** @returns: {unsigned int} - bytes_alloc of the vector
*/
FVECDEF unsigned int fvec_bytes_alloc(void *vector) {
  FVecData *v_data = fvec_get_data(vector);
  return v_data->bytes_alloc;
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

#ifdef __cplusplus
}
#endif // __cplusplus

/*
** TODO:
** - fvec_iter() -> function called 'fvec_iter_next()' steps the iterator by the size of the element
**   - use a union to represent the iterator's return value (inspect if bits are equal to some flag value)
** - fvec_push -> rename to fvec_push_back
** - fvec_push_front() -> push a value into the front of the vector
**
*/
