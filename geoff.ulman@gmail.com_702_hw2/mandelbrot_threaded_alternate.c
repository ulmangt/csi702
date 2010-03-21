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
int numThreads = 4;
// threads grab rowsPerJob rows at a time
int rowsPerJob = 4;
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

// Threads cooperate using the current_row global variable
// When a thread finishes its workload (a set of rowsPerJob
// consecutive rows) it locks the mutex and looks at the value
// of current_row to determine what to process next. It then
// updates current_row to indicate to the other threads that
// its rows are being worked on and unlocks the mutex.
pthread_mutex_t mutex;
int current_row;

void *mandel(void *data)
{
  int *tdata = (int *) data;

  int i, j, it, id;
  double zr, zi, zr2, zi2;
  double x,y;

  int istart, iend;
  clock_t t1, t2;
  double dt;

  // start the clock
  t1 = clock();

  // get the thread id from the input value
  id = *tdata;

  do
  {

    // all the threads use current_row to determine
    // their next job, so we must lock the mutex
    // when accessing and updating it
    pthread_mutex_lock( &mutex );

    // set the limits for the outer loop
    istart = current_row;
    iend = current_row + rowsPerJob;
    if ( iend > n ) iend = n;
    current_row = iend;

    pthread_mutex_unlock( &mutex );

    if ( printResults )
    {
      printf("thread %d working on %d to %d\n", id, istart, iend);
    }

    // loop over the pixels
    for ( i = istart ; i < iend ; i++ )
    {
      for ( j = 0 ; j < n ; j++ )
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
        while (it < itmax && zr*zr + zi*zi < 2)
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

  } while ( istart != n );

  // free malloced thread_data struct
  free( data );

  // exit thread
  pthread_exit( NULL );
}

// create a thread to process Mandelbrot image rows
void start_worker( pthread_t threads[], int tid )
{
  int *data = (int *) malloc(sizeof(int));

  *data = tid;

  int rc = pthread_create(&threads[tid], NULL, mandel, (void *) data);
  if ( rc )
  {
    printf("Error in pthread_create: %d\n", rc);
    exit(-1);
  }
}

// usage: pthread [numThreads] [rowsPerJob] [n] [itmax] [printResults]
int main(int argc, char *argv[])
{
  if ( argc >= 6 )
  {
    numThreads = atoi( argv[1] );
    rowsPerJob = atoi( argv[2] );
    n = atoi( argv[3] );
    itmax = atoi( argv[4] );
    printResults = atoi( argv[5] );
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

  double x,y;
  double zr, zi, zr2, zi2;
  int i, j, it;
  int id;
  int k;
  int rc;

  FILE *fp;

  // create the mutex
  pthread_mutex_init( &mutex, NULL );

  // initialize the current row counter
  current_row = 0;

  // set the limits for the mandelbrot set
  xmin = -2.0;
  xmax =  2.0;
  ymin = -2.0;
  ymax =  2.0;

  // calculate pixel spacings
  dx = ( xmax - xmin ) / ( n-1 );
  dy = ( ymax - ymin ) / ( n-1 );

  // allocate space for the image array
  img = (int *) malloc(sizeof(int) * n * n);

  int tid = 0;

  for ( tid = 0 ; tid < numThreads ; tid++ )
  {
    start_worker( threads, tid );
  }

  for ( tid = 0 ; tid < numThreads ; tid++ )
  {
    void *status;
    int rc = pthread_join( threads[tid], &status );
    if ( rc )
    {
      printf("Error in pthread_join: %d\n", rc);
      exit(-1);
    }
  }

  // destroy the mutex
  pthread_mutex_destroy( &mutex );

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
