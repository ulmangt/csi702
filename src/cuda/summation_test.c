#include "filter_cuda_kernels.h"
#include "filter_constants.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define NUM_TRIALS 3000

extern void run_thrust_tests( float *data, int N, int reps );

void run_test( float *data, int N, int reps )
{
  // perform summation using custom cuda kernel (gpu)
  float *device_array = d_init_array_mem( N );
  float *device_temp1 = d_init_array_mem( N );
  float *device_temp2 = d_init_array_mem( N );
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
    data[i] = (float) rand() / (float) RAND_MAX;
  }

  return data;
}

int main( int argc, char** argv )
{
  float *data = generate_data( NUM_PARTICLES );

  run_test( data, NUM_PARTICLES, NUM_TRIALS );
  run_thrust_tests( data, NUM_PARTICLES, NUM_TRIALS );
}
