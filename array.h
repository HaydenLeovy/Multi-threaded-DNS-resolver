#pragma once
//this is a lifo stack. We push and pop from the top of the stack
#include <stdbool.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdatomic.h>

#define STACK_SIZE 8 // max elements in stack
#define PTHRED once
typedef struct {
    
    sem_t fullEntries; //track full entries
    sem_t emptyEntries; //track empty entries
    pthread_mutex_t mutex; //lock
    char* array[STACK_SIZE]; //DNS array 
    int front;
    int end;
    int currentSize;
    int maxSize;
} array;

int stack_init(array *s);                 // initialise the array
int stack_push(array *s, char *hostname);  // place element into the array, block when full
int stack_pop(array *s, char **hostname); // remove element from the array, block when empty
bool stack_empty(array *s);
bool stack_full(array *s);
void print_arr(array *s);
void stack_free(array *s);               // free the stack's resources
