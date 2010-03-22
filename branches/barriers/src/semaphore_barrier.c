#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

#define NUM_THREADS 10

sem_t sem;

int randn( int );

void *run( int *index )
{
  printf( "index %d started\n", *index );

  // do work
  sleep( randn( 10 ) );

  // semaphore commands
  //sem_wait( &sem );
  //sem_post( &sem );
  //sem_getvalue( &sem, &value );

  printf( "index %d done\n", *index );
}

int main( int argc, char** argv )
{
  // initialize random seed
  srand( time( NULL ) );

  // initialize semaphore
  sem_init( &sem, 0, NUM_THREADS );

  pthread_t threads[NUM_THREADS];
  int thread_ids[NUM_THREADS];

  int i;
  for ( i = 0 ; i < NUM_THREADS ; i++ )
  {
    thread_ids[i] = i;
    pthread_create( &threads[i], NULL, ( void* (*)(void*) ) run, (void*) &thread_ids[i] ); 
  }

  // wait for all threads to complete
  for ( i = 0 ; i < NUM_THREADS ; i++ )
  {
    pthread_join( threads[i], NULL ); 
  }

  // free semaphore
  sem_destroy( &sem );

  pthread_exit( NULL );
}

int randn( int n )
{
  return rand( ) % n;
}
