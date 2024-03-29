#include "filter_cuda_kernels.h"
#include "filter_constants.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define NUM_TRIALS 1

/////////////////////////////////////////////////////
// summation_test.c
//
// A simple test function for comparing the speed
// of various parallel reduction (summation) algorithms.
/////////////////////////////////////////////////////

extern void run_thrust_tests( float *data, int N, int reps );

void run_test( float *data, int N, int reps )
{
  // perform summation using custom cuda kernel (gpu)
  float *device_array;
  float *device_temp1;
  float *device_temp2;

  d_init_array_mem( &device_array, N );
  d_init_array_mem( &device_temp1, N );
  d_init_array_mem( &device_temp2, N );
  copy_array_host_to_device( data, device_array, N );

  float sum;
  time_t start = clock( );
  int i,j;
  for ( j = 0 ; j < reps ; j++ )
  {
    sum = sum_weight( device_array, device_temp1, device_temp2, N );
  }
  time_t end = clock( );

  float diff = (float) (end - start) / CLOCKS_PER_SEC;
  printf( "Parallel Sum (custom): %f Time (sec): %0.5f\n", sum, diff );

  // perform summation on host (cpu)
  start = clock( );
  for ( j = 0 ; j < reps ; j++ )
  {
    sum = 0.0f;
    for ( i = 0 ; i < N ; i++ )
    {
      sum = sum + data[i];
    }
  }
  end = clock( );

  diff = (float) (end - start) / CLOCKS_PER_SEC;
  printf( "Serial Sum: %f Time (sec): %0.5f\n", sum, diff );
}

float *generate_data( int N )
{
  float *data = (float *) malloc( sizeof(float) * N );

  // populate host array
  int i, j;
  for ( i = 0 ; i < N ; i++ )
  {
    data[i] = 1.0f;//(float) rand() / (float) RAND_MAX;
  }

  return data;
}

int main( int argc, char** argv )
{
  float *data = generate_data( 1000 );

  run_test( data, 1000, NUM_TRIALS );
  run_thrust_tests( data, 1000, NUM_TRIALS );

  return 0;
}
