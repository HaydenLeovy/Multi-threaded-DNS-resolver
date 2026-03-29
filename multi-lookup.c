#include "multi-lookup.h"
#include "array.h"


#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <sys/socket.h>
#include <sys/types.h>

//---------------global variable
array *DNSArray;

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

void requester(void *arg)
{
  //cast back to char
  (char*) arg;
  //open
  FILE * myFile = fopen(arg, "r");
  if(myFile == NULL) //check that we opened succesfully
  {
    fprintf(stderr, "File failed to open\n", );
    return;
  }
  //we will read into this
  char[100] line = "";
  //read from file
  while(fgets(line, 100, myFile))
  {
    line[strcspn(line, "\n")] = '\0';//remove null pointer
    printf("Line is: %s", line);
    //stack_push(&DNSArray, line);
  }
  fclose(arg);
}


int main(int argc, char *argv[]) {
  //init array
  stack_init(DNSArray);


  // sample hostname
  if(argc < 3)
  {
    printf(stdin, "invalid number of arguments");
    return 0;
  }
  
  char[] logFileName = argv[1];
  //open file. Overwrite contents and create a new file if it doesnt exist
  FILE *logftr = fopen(logFileName);
  if(*logftr == NUll)
  {
    printf(stdout, "File failed to open\n", );
  }
  printf("creating threads\n");
  const int numThreads = argc-2;
  pthread_t threadsArr = [numThreads];
  for(int i = 0; i < argc-2; i++)//loop and initalize the requesters based on number of input files
  {
    char* fileName = argv[i+2]//get file name create thread
    int threadStat = pthread_create(&thread[i], NULL, produce, (void)fileName);
    if(threadStat != 0)
    {
      printf(stdout, "thread %i failed\n", i);
    }
    else
    {
      printf(stdout, "thread %i success\n", i);
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
