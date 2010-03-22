#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 10

pthread_mutex_t counter_lock;
pthread_mutex_t barrier_lock;

int counter;

int randn( int );

void *run( int *index )
{
  printf( "index %d started\n", *index );

  // do work
  sleep( randn( 10 ) );

  pthread_mutex_lock(&counter_lock);
  counter--;
  printf( "index %d counter %d\n", *index, counter );
  int temp_counter = counter;
  pthread_mutex_unlock(&counter_lock);
  
  if ( temp_counter == 0 )
  {
    pthread_mutex_unlock(&barrier_lock);
  }
  else
  {
    pthread_mutex_lock(&barrier_lock);
    pthread_mutex_unlock(&barrier_lock);
  }

  printf( "index %d done\n", *index, counter );
}

int main( int argc, char** argv )
{
  // initialize random seed
  srand( time( NULL ) );

  // initialize mutex
  pthread_mutex_init(&counter_lock, NULL);
  
  // initialize thread handles
  pthread_t threads[NUM_THREADS];
  int indices[NUM_THREADS];

  // set up barrier
  pthread_mutex_lock(&barrier_lock);
  counter = NUM_THREADS;

  printf( "creating %d threads\n", NUM_THREADS );

  int i;
  for ( i = 0 ; i < NUM_THREADS ; i++ )
  {
    indices[i] = i;
    pthread_create(threads+i, NULL, ( void* (*)(void *) ) run, (void *) &indices[i]);
  }

  printf( "done creating threads\n" );

  pthread_mutex_lock(&barrier_lock);
  pthread_mutex_unlock(&barrier_lock);

  printf( "all threads finished\n" );

  for ( i = 0 ; i < NUM_THREADS ; i++ )
  {
    pthread_join(*(threads+i), NULL);
  }

  pthread_mutex_destroy( &counter_lock );
  pthread_mutex_destroy( &barrier_lock );
  pthread_exit( NULL );
}

int randn( int n )
{
  return rand( ) % n;
}
