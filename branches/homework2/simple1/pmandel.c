#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// original code: J. Wallin - September 2008
// modified by: G. Ulman - February 2010

// Global Parameters (should remain constant):
// width and height of Mandelbrot image
int n = 512;
// maximum Julia Set iterations to try
int itmax = 200000;
// number of threads to create
int numThreads = 512;
// bounds of Mandelbrot Set on complex plane
double xmax, xmin, ymax, ymin;
// step size in complex plane
double dx, dy;
// if 1: print output and timing results
// if 0: perform calculations only
int printResults = 1;

// Global Shared Memory (modified by all threads):
// array of iterations necessary for Julia Set convergence
int *img;

// data passed to each thread containing:
// thread_id : unqiue thread identifier
// start_index : img row to start processing on (inclusive)
// end_index : img row to end processing on (exclusive)
struct thread_data
{
  int thread_id;
  int start_index;
  int end_index;
};

// a routine which calculates a section of the entries in the
// global img array (a number of rows of the Mandelbrot Set)
// no mechanisms for enforcing mutual exclusion or synchronizing
// data access between threads is required as they are working
// on separate sections of the array
void *mandel(void *data)
{
  struct thread_data *tdata = (struct thread_data *) data;

  int i, j, it, id;
  double zr, zi, zr2, zi2;
  double x,y;

  int istart, iend;
  clock_t t1, t2;
  double dt;

  // start the clock
  t1 = clock();

  // get the thread id from the input value
  id = tdata->thread_id;

  // set the limits for the outer loop
  istart = tdata->start_index;
  iend = tdata->end_index;

  // loop over the pixels
  for ( i = istart ; i< iend ; i++ )
  {
    for ( j = 0 ; j<n ; j++ )
    {
      // set the x and y position 
      x = i*dx + xmin;
      y = j*dy + ymin;

      // set the initial values for the iteration loop
      zr = x;
      zi = y;
      it = 0;

      // iterate over the pixel values using 
      // the mandelbrot formula
      while ( it < itmax && zr*zr + zi*zi < 2 )
      {
        // calculate the new value in the complex plane
        zr2 = zr*zr - zi*zi + x;
        zi2 = 2*zi*zr + y;

        // update the old value  
        zr = zr2;
        zi = zi2;

        // increase the iteration number
        it = it + 1;

      }

      // save the iteration number into the image array
      // and update the image counter
      img[ i * n + j ] = it;
    }
  }

  // find the final time
  t2 = clock();
  dt = (double) (t2 - t1) / CLOCKS_PER_SEC;

  // exit thread
  pthread_exit( NULL );
}

// create a thread to process Mandelbrot image rows from start to end
void start_worker( pthread_t *thread, struct thread_data *data, int tid, int start, int end )
{
  data->thread_id = tid;
  data->start_index = start;
  data->end_index = end;

  int rc = pthread_create(thread, NULL, mandel, (void *) data);
  if ( rc )
  {
    printf("Error in pthread_create: %d\n", rc);
    exit(-1);
  }
}

// usage: pthread [numThreads] [n] [itmax] [printResults]
int main(int argc, char *argv[])
{
  if ( argc >= 5 )
  {
    numThreads = atoi( argv[1] );
    n = atoi( argv[2] );
    itmax = atoi( argv[3] );
    printResults = atoi( argv[4] );
  }

  pmandel( );

  return 0;
}

int pmandel( )
{
  clock_t t1, t2;
  double dt;

  // start the clock
  t1 = clock();

  pthread_t threads[numThreads];
  struct thread_data thread_data[numThreads];

  double x,y;
  double zr, zi, zr2, zi2;
  int i, j, it;
  int id;
  int k;
  int rc;

  FILE *fp;

  // set the limits for the mandelbrot set
  xmin = -2.0;
  xmax =  2.0;
  ymin = -2.0;
  ymax =  2.0;

  // calculate pixel spacings
  dx = ( xmax - xmin ) / ( n-1 );
  dy = ( ymax - ymin ) / ( n-1 );

  // allocate space for the image array
  img = (int *) malloc( sizeof(int) * n * n );

  // calculate the number of rows to assign to each thread
  // if the total number of threads is not divisible by the
  // number of rows, the remainder will be assigned to the
  // last thread
  int remainder = n % numThreads;
  int step =  ( n - remainder ) / numThreads;
  int tid = 0;
  int start = 0;
  int end = 0;

  // create numThreads-1 threads with even workloads
  for ( tid = 0 ; tid < numThreads - 1 ; tid++ )
  {
    end = start + step;
    start_worker( threads+tid, thread_data+tid, tid, start, end );
    start = end;
  }

  // create the last thread with any remaining
  // not evenly divisible work
  start_worker( threads+tid, thread_data+tid, tid, end, n );

  // wait for all threads to complete
  for ( tid = 0 ; tid < numThreads ; tid++ )
  {
    void *status;
    int rc = pthread_join( *(threads+tid), &status );
    if ( rc )
    {
      printf("Error in pthread_join: %d\n", rc);
      exit(-1);
    }
  }

  if ( printResults )
  {
    // find the final time once all threads have finished
    t2 = clock();
    dt = (double) (t2 - t1) / CLOCKS_PER_SEC;

    printf("Total CPU time recorded by clock() = %g\n", dt);

    // open the output file and write the array data
    k = 0;
    fp = fopen( "mset","w" );
    for ( i = 0 ; i < n ; i++ )
    {
      for ( j = 0 ; j < n ; j++ )
      { 
        fprintf( fp, "%d \n",img[k] );
        k = k + 1;
      }
    }
    fclose( fp );
  }
}
