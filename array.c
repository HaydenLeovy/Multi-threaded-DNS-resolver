#include "array.h"
// your thread-safe array implementation here
#include "array.h"
#include <stdlib.h>
#include <semaphore.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>

// initialise the stack
int stack_init(array *s)
{
    //printf("initing stack\n");
    //initalize semaphores
    int semFullState = 1;
    int semEmptyState = 1;
    int mutexStat = 1;
    semFullState = sem_init(&(s->fullEntries), 0, 0);
    semEmptyState = sem_init(&(s->emptyEntries), 0, STACK_SIZE);
    // initialize mutex
    mutexStat = pthread_mutex_init(&(s->mutex), NULL);
    if(semFullState != 0)
    {
        printf("sem full broke\n");
    }
    if(semEmptyState != 0)
    {
        printf("sem empty broke\n");
    }
    if(mutexStat != 0)
    {
        printf("mutex broke\n");
    }
    
    //initialize stack vars
    s->front = 0;
    s->end = 0;
    s->currentSize = 0;
    s->maxSize = STACK_SIZE;

    //make all inital pointers null
    for(int i = 0; i < STACK_SIZE; i++)
    {
        s->array[i] = NULL;
        
    } 
    return 0;
}

// place element on the top of the stack
int stack_push(array *s, char *hostname)
{
    // printf("pushing\n");
    sem_wait(&(s->emptyEntries)); // block when there are 0 empty entries and we are full. Each push decrements empty entries
    // printf("past wait\n");
    pthread_mutex_lock(&(s->mutex)); // lock for critical section
    //printf("push gained lock\n");
    // printf("in critical\n");
    //printf("pushing\n");
    s->array[s->end % STACK_SIZE] = hostname; //insert hostname at end. Mod keeps this within queue bounds
    s->currentSize += 1; //update current size
    //printf("pushing %c at pos %i i is %i\n", *hostname, s->end % STACK_SIZE, s->end);
    s->end = (s->end+1) % STACK_SIZE;//increment end   
    pthread_mutex_unlock(&(s->mutex)); // unlock after critical section
    //printf("push released lock\n");
    sem_post(&(s->fullEntries));        // we have added to buffer so we signal full entries
    
    return 0;
}

// remove element from the top of the stack
int stack_pop(array *s, char **hostname)
{
    //printf("about to wait on pop\n");
    sem_wait(&(s->fullEntries)); // block when there are 0 full entries. Nothing to consume
    //printf("past wait for pp\n");
    pthread_mutex_lock(&(s->mutex)); // lock for critical section
    //printf("front: %i\n", s->front % STACK_SIZE);
    *hostname = (s->array[s->front % STACK_SIZE]);//get element in front
    (s->array[s->front % STACK_SIZE]) = NULL; //replace pop element with null
    //printf("popping\n");
    s->front = s->front + 1 % STACK_SIZE; //increment front
    s->currentSize-1; //decrement current size
    pthread_mutex_unlock(&(s->mutex)); // unlock after critical section
    sem_post(&(s->emptyEntries));       // we consumed form the buffer so we signal emptyEntrie
    return 0;
}

bool stack_empty(array *s)
{
    if(s->currentSize == 0)//if current size = 0 return true else return false
    {
        return true;
    }
    return false;
}
bool stack_full(array *s)
{
    if(s->currentSize == s->maxSize)//if current size = max size return true else return false
    {
        return true;
    }
    return false;
}

void print_arr(array * s)
{
    for(int i = 0; i < STACK_SIZE; i++)
    {
        if(s->array[i] != NULL)
        {
            printf("%c\n", *(s->array[i]));
        }
        else
        {
            printf("Null\n");
        }
    }
}

// free the stack's resources
void stack_free(array *s) {
    sem_destroy(&s->fullEntries);
    sem_destroy(&s->emptyEntries);
    pthread_mutex_destroy(&s->mutex);
    free(s);
}