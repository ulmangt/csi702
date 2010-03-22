#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

#define NUM_THREADS 10

sem_t sem;
pthread_mutex_t mutex;

int count;

int randn( int );

void *run( int *index )
{
  printf( "index %d started\n", *index );

  // do work
  sleep( randn( 10 ) );

  pthread_mutex_lock( &mutex );
  count--;
  int temp_count = count;
  pthread_mutex_unlock( &mutex );

  if ( temp_count == 0 )
  {
    printf( "index %d waking up first, count %d\n", *index, count );

    sem_post( &sem );
  }
  else
  {
    printf( "index %d waiting,count %d\n", *index, count );

    sem_wait( &sem );
    sem_post( &sem ); // wake up the next guy
  }

  //sem_post( &sem );
  //sem_getvalue( &sem, &value );

  printf( "index %d done\n", *index );
}

int main( int argc, char** argv )
{
  // initialize random seed
  srand( time( NULL ) );

  // initialize semaphore
  sem_init( &sem, 0, 0 );

  // initialize mutex
  pthread_mutex_init( &mutex, NULL );

  count = NUM_THREADS;

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

  int value;
  sem_getvalue( &sem, &value );

  printf( "semaphore final value %d\n", value ); 

  // free semaphore
  sem_destroy( &sem );

  pthread_mutex_destroy( &mutex );

  pthread_exit( NULL );
}

int randn( int n )
{
  return rand( ) % n;
}
