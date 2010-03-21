#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS	1


// September 2008 - J. Wallin

int itmax = 200000;
int n = 512;
// if 1: print output and timing results
// if 0: perform calculations only
int printResults = 1;

int *img;
double xmax, xmin, ymax, ymin;
double dx, dy;

double mandel_time[NUM_THREADS];


void *mandel(void *tid)
{
  int id;
  int *tt;

  int i, j, it, k;
  double zr, zi, zr2, zi2;
  double x,y;

  int istart, iend;
  clock_t t1, t2;
  double dt;

  // start the clock
  t1 = clock();


  // get the thread id from the input value
  tt = tid;
  id = (int) *tt;


  // zero the array counter
  k = 0;

  // set the limits for the outer loop
  istart = 0;
  iend = n;

  // loop over the pixels
  for (i= istart; i< iend; i++) {
    for (j = 0; j<n; j++) { 

      // set the x and y position 
      x = i*dx + xmin;
      y = j*dy + ymin;

      // set the initial values for the iteration loop
      zr = x;
      zi = y;
      it = 0;


      // iterate over the pixel values using 
      // the mandelbrot formula
      while (it < itmax && zr*zr + zi*zi < 2) {

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
      img[k] = it;     
      k = k  + 1;
    }
  }


  // find the final time
  t2 = clock();
  dt = (double) (t2 - t1) / CLOCKS_PER_SEC;
  mandel_time[id] = dt;


}

// usage: pthread [numThreads] [n] [itmax] [printResults]
int main(int argc, char *argv[])
{
  if ( argc >= 4 )
  {
    n = atoi( argv[1] );
    itmax = atoi( argv[2] );
    printResults = atoi( argv[3] );
  }

  mandel_main( );

  return 0;
}



int mandel_main( )
{
  int tid;

  double x,y;
  double zr, zi, zr2, zi2;
  int i, j, it;
  int id;
  int k;

  FILE *fp;


  // set the limits for the mandelbrot set
  xmin = -2.0;
  xmax =  2.0;
  ymin = -2.0;
  ymax =  2.0;

  // calculate pixel spacings
  dx = (xmax - xmin)/(n-1);
  dy = (ymax - ymin)/(n-1);

  // allocate space for the image array
  img = (int *) malloc(sizeof(int) * n * n);


  // call the mandelbrot system
  tid = 0;
  mandel(&tid);

  if ( printResults )
  {
    // open the output file and write the array data 
    k = 0;
    fp = fopen("mset","w");
    for (i=0;i<n;i++) {
      for (j = 0; j<n; j++) { 

        fprintf(fp, "%d \n",img[k]);
        k = k + 1;
      }
    }
    fclose(fp);


    // summarize routine times
    printf("times for routine = %g \n",mandel_time[tid]);
  }
}
