#include <stdio.h>
#include <stdlib.h>
#include "array.h"

/*int main(int argc, char *argv[]) {
 
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
}*/

#include <stdio.h>
#include <pthread.h>
array *DNSarray;
int counter = 0;
pthread_mutex_t countMutex = PTHREAD_MUTEX_INITIALIZER;

void* threadOneFunc(void* arg)
{
    for(int iOne = 0; iOne < 5; iOne++)
    {
        pthread_mutex_lock(&countMutex);
        counter = counter - 1;
        pthread_mutex_unlock(&countMutex);
    }
    return NULL;
}

void* threadTwoFunc(void* arg)
{
    for(int iTwo = 0; iTwo < 5; iTwo++)
    {
        pthread_mutex_lock(&countMutex);
        counter = counter - 1;
        pthread_mutex_unlock(&countMutex);
    }
    return NULL;
}

int main()
{
    DNSarray = NULL;
    stack_init(DNSarray);
    printf("afterinit\n");
    
    counter = 10;
    pthread_t threadOne;
    pthread_t threadTwo;

    pthread_create(&threadOne, NULL, threadOneFunc, NULL);
    pthread_create(&threadTwo, NULL, threadTwoFunc, NULL);

    pthread_join(threadOne, NULL);
    pthread_join(threadTwo, NULL);

    printf("%i\n", counter);

    pthread_mutex_destroy(&countMutex);

    return 0;
}

