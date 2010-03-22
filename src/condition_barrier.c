#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 10

pthread_mutex_t mutex;
pthread_cond_t cond;

int counter;

int randn( int );

void *run( int *index )
{
  printf( "index %d started\n", *index );

  // do work
  sleep( randn( 10 ) );

  pthread_mutex_lock( &mutex );

  if ( --counter == 0 )
  {
    printf( "index %d broadcasting counter: %d\n", *index, counter );
    pthread_cond_broadcast( &cond );
  }
  else
  {
    printf( "index %d waiting counter: %d\n", *index, counter );
    pthread_cond_wait( &cond, &mutex );    
  }

  pthread_mutex_unlock( &mutex );

  printf( "index %d done\n", *index );
}

int main( int argc, char** argv )
{
  // initialize random seed
  srand( time( NULL ) );

  // initialize mutex
  pthread_mutex_init( &mutex, NULL );

  // initialize condition variable
  pthread_cond_init( &cond, NULL );

  counter = NUM_THREADS;

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

  pthread_mutex_destroy( &mutex );
  pthread_cond_destroy( &cond );
  pthread_exit( NULL );
}

int randn( int n )
{
  return rand( ) % n;
}
