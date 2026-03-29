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
    s->top = 0;
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
    if (s->top >= STACK_SIZE)
    {
        printf("push broke\n");
        return -1; // return failed if the stack is full
    }
    //printf("pushing\n");
    s->array[s->top++] = *hostname;
    
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
    if (s->top < 0)
    {
        printf("pop broke\n");
        return -1; // return failed if the stack is empty
    }
    //printf("popping\n");
    **hostname = s->array[--s->top];
    pthread_mutex_unlock(&(s->mutex)); // unlock after critical section
    sem_post(&(s->emptyEntries));       // we consumed form the buffer so we signal emptyEntrie
    return 0;
}

// free the stack's resources
void stack_free(array *s) {
    sem_destroy(&s->fullEntries);
    sem_destroy(&s->emptyEntries);
    pthread_mutex_destroy(&s->mutex);
    free(s);
}