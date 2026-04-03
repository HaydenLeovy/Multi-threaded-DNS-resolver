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
#include <stdatomic.h>

//---------------global variable
array DNSArray;
pthread_mutex_t logMutex;
pthread_mutex_t fileOpenLock;
atomic_int activeRequesterCount;
FILE *logFile;
size_t maxRead;

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
        if (inet_ntop(AF_INET, &(address->sin_addr), ipv4, ipv4_len) != NULL) //convert address to int and store in ipv4
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
    char * fileName = (char*) arg;
    //printf("In requester thread\n");
    //-----open file
    FILE *myInputFile = fopen(fileName, "r");
    if(myInputFile == NULL)
    {
      printf("input file null\n");
      return NULL;
    }
    else
    {
      printf("%s opened succesfully\n", fileName);
    }
    
    //--------read from file into buffer
    char buffer[256]; //this is the buffer we will read into. We need to malloc a new buffer because if we dont we will assign the same pointer to eveyr index in array
    printf("aboiut to read\n");
    
    while(fgets(buffer, sizeof(buffer), myInputFile))//loop read until end of file. getline wnats a char ** which is why we do &buffer
    {
      char * hostname = strdup(buffer);
      if(!hostname)
      {
        printf("memory allocation failed\n");
        continue;
      }
      
      printf("reading from %s: %s", fileName, buffer);
      //push onto DNS array
      stack_push(&DNSArray, hostname);
     
    
    }
    //printf("exited requester\n");
    atomic_fetch_add(&activeRequesterCount, -1); //do this atomically because decremnets are not atomic
    if(activeRequesterCount == 0)
    {
        char *poisonOne = strdup("PI");
        stack_push(&DNSArray, poisonOne);
        char *poisonTwo = strdup("PI");
        stack_push(&DNSArray, poisonTwo);
    }
    return NULL;
}

void *resolver(void *arg)
{

    //printf("in resolver\n");
    if(logFile != NULL)
    {
        printf("log file not null for resolver\n");
    }
    
    while(1)//whilwe there are still requester erequesting or array is not empty loop
    {
        printf("requesters: %i\n", activeRequesterCount);
        //-----------get domain from DNSarray
        //pop a element from stack
        char * domainName = NULL;
        char ** domainNameP = &domainName;
        stack_pop(&DNSArray, domainNameP);
        //make sure domain is not null
        if(*domainNameP == NULL)
        {
            printf("domain is null\n");
            continue;
        }
        if(strcmp(*domainNameP, "PI") == 0)//poison pill in array
        {
            free(*domainNameP);
            return NULL;
        }
        printf("domain name is: %s\n", *domainNameP);
        //----------get ip address
        //create a buffer for the ip address
        const size_t ipAddLen = 16;//this is constant
        char ipv4[ipAddLen];
        // strip newline
        domainName[strcspn(domainName, "\n")] = '\0';
        //resolve address
        if(resolve_ipv4_address(*domainNameP, ipv4, ipAddLen) != 0)//failed to get ip address
        {
            strcpy(ipv4, "Domain no found");
            printf("%s Ip address not found\n", *domainNameP);
        }
        else
        {
            printf("IP add: %s\n", ipv4);
        }
        int bytesWritten = fprintf(logFile, "%s, %s\n", *domainNameP, ipv4);
        //we can now free the domainName pointer
        free(*domainNameP);
       
        printf("%i\n", bytesWritten);
    }
    //printf("resolver has exited succesfully\n");
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
    // init read max
    maxRead = 30;
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
    /*stack_push(&DNSArray, "host 1");
    stack_push(&DNSArray, "facebook");
    stack_push(&DNSArray, "host 3");
    stack_push(&DNSArray, "google");
    stack_push(&DNSArray, "host 5");
    stack_push(&DNSArray, "safari");*/
    print_arr(&DNSArray);
    //--------settings
    int const resolverNum = 2;

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
    activeRequesterCount = 2;
    printf("creating threads\n");
    // create arrays for threads
    const int requesterNum = argc - 2;
    pthread_t requesterThreadsArr[requesterNum];
    pthread_t resolverThreadsArr[resolverNum];
    //---------producers/requesters
    printf("trying to prints requester threads\n");
    for (int i = 0; i < requesterNum; i++)
    {
        // create requester
        if (pthread_create(&requesterThreadsArr[i], NULL, requester, (void *) argv[i+2]) != 0)
        {
            printf("resolver thread %i failed\n", i);
        }
        else
        {
            printf("resolver thread %i success\n", i);
        }
    }


    //--------consumers/resolvers
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
    //join threads so main waits for them to complete 
    for(int i = 0; i < requesterNum; i++)
    {
        pthread_join(requesterThreadsArr[i], NULL);
    }
    for (int i = 0; i < resolverNum; i++)
    {
        pthread_join(resolverThreadsArr[i], NULL);
    }
    
    print_arr(&DNSArray);
    printf("reached bottom of main\n");
    stack_free(&DNSArray);
    return 0;
}