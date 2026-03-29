#include <stdio.h>
#include <stdlib.h>
#include "array.h"

int main(int argc, char *argv[]) {
 
  array testArray;
  array* testArrayP = &testArray;
  char *A = "a";
  char *B = "b";
  
  char *C = "c";
  char *D = "d";

  char *E = "e";
  char *F = "f";

  char *G = "g";
  char *H = "h";
   char *i = "i";
    char *j = "j";
    char *k = "k";

  stack_init(testArrayP);
  

  stack_push(testArrayP, A);
  stack_push(testArrayP, B);
  stack_push(testArrayP, C);
  stack_push(testArrayP, D);
  stack_push(testArrayP, E);
   stack_push(testArrayP, F);
    stack_push(testArrayP, G);
     stack_push(testArrayP, H);
     stack_push(testArrayP, H);
      stack_push(testArrayP, i);
       stack_push(testArrayP, j);
        stack_push(testArrayP, k);
  print_arr(testArrayP);
  char *poval1 = NULL;
  char **popVal = &poval1;          
  stack_pop(testArrayP, popVal);
  printf("pop val %c\n", **popVal);
  stack_pop(testArrayP, popVal);
  printf("pop val %c\n", **popVal);
  stack_pop(testArrayP, popVal);
  printf("pop val %c\n", **popVal);
  stack_pop(testArrayP, popVal);
  printf("pop val %c\n", **popVal);
  stack_pop(testArrayP, popVal);
  printf("pop val %c\n", **popVal);
  stack_pop(testArrayP, popVal);
  printf("pop val %c\n", **popVal);
  stack_pop(testArrayP, popVal);
  printf("pop val %c\n", **popVal);
  stack_pop(testArrayP, popVal);
  printf("pop val %c\n", **popVal);
  print_arr(testArrayP);


  return 0;
}
