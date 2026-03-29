#include "multi-lookup.h"
#include "array.h"


#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
//---------------global variable
array *DNSArray;
pthread_mutex_t logMutex;
int activeRequesterCount;

static int resolve_ipv4_address(const char *hostname, char *ipv4, size_t ipv4_len) {
  struct addrinfo hints;
  struct addrinfo *results = NULL;
  struct addrinfo *current = NULL;
  int status = 0;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  status = getaddrinfo(hostname, NULL, &hints, &results);
  if (status != 0) {
    return -1;
  }

  for (current = results; current != NULL; current = current->ai_next) {
    struct sockaddr_in *address = (struct sockaddr_in *) current->ai_addr;
    if (inet_ntop(AF_INET, &(address->sin_addr), ipv4, ipv4_len) != NULL) {
      freeaddrinfo(results);
      return 0;
    }
  }

  freeaddrinfo(results);
  return -1;
}

void *requester(void *arg)
{
  void *returnP = NULL;
  //cast back to char
  (char*) arg;
  //open
  FILE * myFile = fopen(arg, "r");
  if(myFile == NULL) //check that we opened succesfully
  {
    fprintf(stderr, "File failed to open\n");
    return returnP;
  }
  //we will read into this
  char line[100] = "";
  //read from file
  while(fgets(line, 100, myFile))
  {
    line[strcspn(line, "\n")] = '\0';//remove null pointer
    printf("Line is: %s", line);
    stack_push(DNSArray, line);//push onto qeueu
    

  }
  fclose(arg);
  atomic_fetch_add(&activeRequesterCount, -1); //atomicaly decrement poison pill
  
  return returnP;
}

void *resolver(void *arg)
{
  //cast back to char
  (char*) arg;
  //open
  FILE * myFile = fopen(arg, "w");
  if(myFile == NULL)
  {
    printf("Log file opened succesfully");
  }
  while(activeRequesterCount != 0 && !stack_empty(DNSArray))
  {
    //take element from array: threadsafe
    char ** domainName = NULL;
    stack_pop(DNSArray, domainName);


    //get ip address: threadsafe
    char *ipAdd = NULL;
    size_t ipAddLen = 16;
    if(resolve_ipv4_address(*domainName, ipAdd, ipAddLen) != 0)
    {
      printf("failed to get IP add\n");
      char * domainNameP = "could not resolve IP address";
      domainName = &domainNameP;
    }
    //----log to file
    pthread_mutex_lock(&logMutex);//lock mutex
    fprintf(myFile, "%i, %c", **domainName, *ipAdd);//write to file
    pthread_mutex_unlock(&logMutex); //unlock after writing
  }
}

int main(int argc, char *argv[]) {
  //init array and mutex
  stack_init(DNSArray);
  pthread_mutex_init(&logMutex, 0);
  //init requester count
  activeRequesterCount = argc - 2;//number of input files is argc - a.exe and log file
  //--------settings
  int resolverNum = 2;

  if(argc < 3)
  {
    printf("invalid number of arguments");
    return 0;
  }
  
  char *logFileName = argv[1];
  //open file. Overwrite contents and create a new file if it doesnt exist
  FILE *logftr = fopen(logFileName, "r");
  if(logftr == NULL)
  {
    printf("File failed to open\n");
  }

  printf("creating threads\n");
  //create arrays for threads
  const int requesterNum = argc-2;
  pthread_t requesterThreadsArr[requesterNum];
  pthread_t resolverThreadsArr[resolverNum];
  for(int i = 0; i < requesterNum; i++)//loop and initalize the requesters based on number of input files
  {
    char* fileName = argv[i+2];//get file name create thread
    //create requester
    if(pthread_create(&requesterThreadsArr[i], NULL, requester, (void *)fileName) != 0)
    {
      printf("requester thread %i failed\n", i);
    }
    else
    {
      printf("requester thread %i success\n", i);
    }
  }

  for(int i = 0; i < resolverNum; i++)
  {
    //create resolver
    if(pthread_create(&resolverThreadsArr[i], NULL, resolver, (void *)logFileName) != 0)
    {
      printf("resolver thread %i failed\n", i);
    }
    else
    {
      printf("resolver thread %i success\n", i);
    }
  }
  /*  
  const char *hostname = "google.com";
  // buffer to store the first IPv4 address returned by getaddrinfo
  char ipv4[INET_ADDRSTRLEN];

  // resolve hostname to IPv4 address
  if (!resolve_ipv4_address(hostname, ipv4, sizeof(ipv4))) {
    printf("%s -> %s\n", hostname, ipv4);
  } else {
    printf("%s -> NOT_RESOLVED\n", hostname);
  }
  fclose(logftr);
  return 0; */
}
