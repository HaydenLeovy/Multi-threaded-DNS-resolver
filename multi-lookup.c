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
pthread_mutex_t fileOpenLock;
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
  printf("In requester thread\n");
  void *returnP = NULL;
  //cast back to char
  (char*) arg;
  //open
  printf("trying to open file\n");
  FILE * myFile = fopen(arg, "r");
  if(myFile == NULL) //check that we opened succesfully
  {
    printf("File failed to open\n");
    return returnP;
  }
  printf("Past file opening\n");
  //we will read into this
  char line[100] = "";
  //read from file
  /*
  while(fgets(line, 100, myFile))
  {
    line[strcspn(line, "\n")] = '\0';//remove null pointer
    printf("Line is: %s", line);
    stack_push(DNSArray, line);//push onto qeueu
    

  }
  fclose(arg);
  atomic_fetch_add(&activeRequesterCount, -1); //atomicaly decrement poison pill
  */
  
  return returnP;
}

void *resolver(void *arg)
{
  printf("in resolver\n");
  //cast back to char
  char *logFileNameP = (char*) arg;
  printf("trying to open log file\n");
  if(logFileNameP == NULL)
  {
    printf("file name null\n");
  }
  if(logFileNameP != NULL)
  {
    printf("file name is null\n");
  }
  //open
  FILE * myFile = fopen(logFileNameP, "w");
  if(myFile != NULL)
  {
    printf("Log file opened succesfully\n");
  }
  else
  {
    printf("log file failed to open\n");
  }
  
  /*while(activeRequesterCount != 0 && !stack_empty(DNSArray))
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
  }*/
}

int main(int argc, char *argv[]) {
  array DNSArrayOne;
  DNSArray = &DNSArrayOne;
  printf("entering main\n");
  //init array and mutex
  stack_init(DNSArray);
  printf("stack inited\n");
  pthread_mutex_init(&logMutex, 0);
  pthread_mutex_init(&fileOpenLock, 0);
  //init requester count
  activeRequesterCount = argc - 2;//number of input files is argc - a.exe and log file
  //--------settings
  int resolverNum = 2;

  if(argc < 3)
  {
    printf("invalid number of arguments");
    return 0;
  }
  else
  {
    printf("correct amount of requirements\n");
  }
  
  char *logFileName = argv[1];
  if (logFileName == NULL) {
    printf("argv[1] is NULL!\n");
  } 
  else 
  {
    printf("argv file name is: %s\n", logFileName);
  }
  
  

  printf("creating threads\n");
  //create arrays for threads
  const int requesterNum = argc-2;
  pthread_t requesterThreadsArr[requesterNum];
  pthread_t resolverThreadsArr[resolverNum];
  //create a array for file names and fill it. This is because we cant have a filename variablelocal to our loop. Not threadsafe
  char* inputFiles[argc-2];
  for(int i = 0; i < argc-2; i++)
  {
    inputFiles[i]=NULL; //get rid of garbage
    inputFiles[i]=argv[i+2];//get ith filename;
  }
  //--------producers
  /*for(int i = 0; i < requesterNum; i++)//loop and initalize the requesters based on number of input files
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
  }*/
  //--------consumers
  printf("trying to init resolver threads\n");
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
