#include "filter_cuda_kernels.h"

#include <thrust/device_ptr.h>
#include <thrust/host_vector.h>
#include <thrust/device_vector.h>
#include <thrust/generate.h>
#include <thrust/reduce.h>
#include <thrust/functional.h>
#include <thrust/scan.h>
#include <cstdlib>

int small_rand( )
{
  return rand() % 10;
}

extern "C" void run_thrust_tests( float *data, int N, int reps )
{
  // alloate host array
  thrust::host_vector<float> host_data( N );

  int j;
  for ( j = 0 ; j < N ; j++ )
  {
    host_data[j] = data[j];
  }

  // copy data to device
  thrust::device_vector<float> device_data = host_data;


  // perform summation on device (gpu)
  time_t start = clock( );
  float sum;
  for ( j = 0 ; j < reps ; j++ )
  {
    sum = thrust::reduce(device_data.begin(), device_data.end(), 0.0f, thrust::plus<float>());
  }
  time_t end = clock( );

  double diff = (float) (end - start) / CLOCKS_PER_SEC;
  printf( "Parallel Sum (thrust): %f Time (sec): %0.5f\n", sum, diff );



  // allocate a device and host vector for the results
  thrust::device_vector<float> device_scan_data( N );
  thrust::host_vector<float> host_scan_data( N );

  // perform cumulative sum
  start = clock( );
  for ( j = 0 ; j < reps ; j++ )
  {
    thrust::inclusive_scan(device_data.begin(), device_data.end(), device_scan_data.begin());
  }
  end = clock( );

  // transfer partial sums from device (not included in timing test)
  host_scan_data = device_scan_data;

  diff = (float) (end - start) / CLOCKS_PER_SEC;
  printf( "Parallel Scan (thrust) Time (sec): %0.5f\n", diff );

  // don't print the cumulative sum array
  for ( j = 0 ; j < N ; j++ )
  {
    printf("%f\n", host_scan_data[j]);
  }
}
