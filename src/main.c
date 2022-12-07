
#include <time.h>
#include <stdio.h>
#include <string.h>

#define FVEC_IMPLEMENTATION
#include "../fvec.h"
#undef  FVEC_IMPLEMENTATION

char *make_string(char *lit) {
  char *str = fvecci(sizeof(char), strlen(lit));

  while(*lit != '\0')
    *(char*)fvec_push(&str) = *lit++;
  
  return str;
}

void print(void *c) {
  printf("%c", *(char*)c);
}

int main(void) {
  // this is not guaranteed to be null terminated...
  // only fvec_* functions are safe on this string
  char *test = make_string("This is a test string");

  fvec_print(test, print);
  fvec_free(&test);
  
  return 0;
}

