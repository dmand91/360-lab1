#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define NUM_THREADS     10
#include <queue>
#include <iostream>
#include <semaphore.h>



sem_t full, empty, mutex;
//queue socketqueue;

class myqueue{
	std::queue <int> stlqueue;
	public: 
	void push(int sock){
		sem_wait(&empty);
		sem_wait(&mutex);
		stlqueue.push(sock);	
		sem_wait(&mutex);
		sem_wait(&full);

	}

	int pop (){

		sem_wait(&full);
		sem_wait(&mutex);
		int rval=stlqueue.front();
		stlqueue.pop();
		sem_wait(&mutex);
		sem_wait(&empty);
		return (rval);

	{	
}   sockqueue;


void *PrintHello(void *threadid)
{
   int tid;
   tid = (long)threadid;
   //printf("Hello World! It's me, thread #%ld!\n", tid);
		std::cout<<"Got "<<sockqueue.pop()<<std::endl;
   pthread_exit(NULL);
}

int main (int argc, char *argv[])
{
   pthread_t threads[NUM_THREADS];
   int rc;
   long t;

    sem_init(&full, PTHREAD_PROCESS_PRIVATE, 0);
    sem_init(&empty, PTHREAD_PROCESS_PRIVATE, NQUEUE);
    sem_init(&mutex, PTHREAD_PROCESS_PRIVATE, 1);


	for(int i =0; i<10; i++){
		sockqueue.push(i);
	}
	for(int i =0; i<10; i++){

		std::cout<<"Got "<<sockqueue.pop()<<std::endl;
	}
   for(t=0; t<NUM_THREADS; t++){
      printf("In main: creating thread %ld\n", t);
      rc = pthread_create(&threads[t], NULL, PrintHello, (void *)t);
      if (rc){
         printf("ERROR; return code from pthread_create() is %d\n", rc);
         exit(-1);
      }
   }

//need to put locks in the quue
//create own class that does the semaphore and then unlock 
	//Set up  socket, bind, listen
//for (;;)
// fd = accept
// Put fd in queue
   /* Last thing that main() should do */
   pthread_exit(NULL);
}

