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

int main( int argc, char** argv )
{
  int N = 1000;

  // alloate host array
  thrust::host_vector<int> host_data( N );

  // populate host array
  int i;
  for ( i = 0 ; i < N ; i++ )
  {
    host_data[i] = (float) i;
  }

  // copy data to device
  thrust::device_vector<float> device_data = host_data;

  // perform summation on device (gpu)
  time_t start = time( NULL );
  float sum = thrust::reduce(device_data.begin(), device_data.end(), 0.0f, thrust::plus<float>());
  time_t end = time( NULL );

  double diff = difftime( end, start );
  printf( "Parallel Sum (thrust): %f Time (sec): %0.5f\n", sum, diff );

  // allocate a device and host vector for the results
  thrust::device_vector<float> device_scan_data( N );
  thrust::host_vector<float> host_scan_data( N );

  // perform cumulative sum
  start = time( NULL );
  thrust::inclusive_scan(device_data.begin(), device_data.end(), device_scan_data.begin());
  end = time( NULL );

  // transfer partial sums from device (not included in timing test)
  host_scan_data = device_scan_data;

  diff = difftime( end, start );
  printf( "Parallel Scan (thrust) Time (sec): %0.5f\n", diff );

  // don't print the cumulative sum array 
  /*
  for ( i = 0 ; i < N ; i++ )
  {
    printf("%f\n", host_scan_data[i]);
  }
  */

  return 0;
}
