#include "multi-lookup.h"
#include "array.h"

#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <pthread.h>

//---------------global variable
array DNSArray;
pthread_mutex_t logMutex;
pthread_mutex_t fileOpenLock;
int activeRequesterCount;
FILE *logFile;

static int resolve_ipv4_address(const char *hostname, char *ipv4, size_t ipv4_len)
{
    struct addrinfo hints;
    struct addrinfo *results = NULL;
    struct addrinfo *current = NULL;
    int status = 0;

    memset(&hints, 0, sizeof(hints)); // fill hints with size of hints 0s
    hints.ai_family = AF_INET;        // address family
    hints.ai_socktype = SOCK_STREAM;  // socket type

    status = getaddrinfo(hostname, NULL, &hints, &results); // getaddrinfo returns a ip address for hostname
    if (status != 0)
    {
        return -1;
    }

    for (current = results; current != NULL; current = current->ai_next)
    {                                                                         // loop through ip objects in result
        struct sockaddr_in *address = (struct sockaddr_in *)current->ai_addr; // get socket address and make address point to it
        if (inet_ntop(AF_INET, &(address->sin_addr), ipv4, ipv4_len) != NULL)
        { // put address in ipv4
            freeaddrinfo(results);
            return 0;
        }
    }

    freeaddrinfo(results);
    return -1;
}

void *requester(void *arg)
{
    printf("In requester thread\n");
    printf("exited requester\n");
}

void *resolver(void *arg)
{

    printf("in resolver\n");
    if(logFile != NULL)
    {
        printf("log file not null for resolver. Attempting write\n");
    }
    while(activeRequesterCount != 0|| !stack_empty(&DNSArray))//whilwe there are still requester erequesting or array is not empty loop
    {
        //pop a element from stack
        char * domainName = NULL;
        char ** domainNameP = &domainName;
        stack_pop(&DNSArray, domainNameP);
        printf("domain name is: %s\n", *domainNameP);
        fwrite(*domainNameP, sizeof(char), strlen(*domainNameP), logFile);
    }
    printf("resolver has exited succesfully\n");
    return NULL;
}

int main(int argc, char *argv[])
{

    
    printf("entering main\n");
    // init array and mutex
    stack_init(&DNSArray);
    printf("stack inited\n");
    pthread_mutex_init(&logMutex, 0);
    pthread_mutex_init(&fileOpenLock, 0);
    // init requester count
    activeRequesterCount = argc - 2; // number of input files is argc - a.exe and log file
    // init log file
    char *logFileName = argv[1];

    logFile = fopen(argv[1], "w");
    if (logFile == NULL)
    {
        printf("log file null. log file name is %s\n", logFileName);
    }
    else
    {
        printf("logFile not null\n");
    }
    // fill array
    printf("printing array\n");
    stack_push(&DNSArray, "host 1");
    stack_push(&DNSArray, "host 2");
    stack_push(&DNSArray, "host 3");
    print_arr(&DNSArray);
    //--------settings
    int const resolverNum = 1;

    if (argc < 3)
    {
        printf("invalid number of arguments");
        return 0;
    }
    else
    {
        printf("correct amount of requirements\n");
    }

    if (logFileName == NULL)
    {
        printf("argv[1] is NULL!\n");
    }
    else
    {
        printf("argv file name is: %s\n", logFileName);
    }

    printf("creating threads\n");
    // create arrays for threads
    const int requesterNum = argc - 2;
    pthread_t requesterThreadsArr[requesterNum];
    pthread_t resolverThreadsArr[resolverNum];

    //--------consumers
    printf("trying to init resolver threads\n");
    for (int i = 0; i < resolverNum; i++)
    {

        // create resolver
        if (pthread_create(&resolverThreadsArr[i], NULL, resolver, NULL) != 0)
        {
            printf("resolver thread %i failed\n", i);
        }
        else
        {
            printf("resolver thread %i success\n", i);
        }
    }
    for (int i = 0; i < resolverNum; i++)
    {
        pthread_join(resolverThreadsArr[i], NULL);
    }

    printf("reached bottom of main\n");

    return 0;
}
