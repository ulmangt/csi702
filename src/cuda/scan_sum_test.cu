#include "filter_cuda_kernels.h"

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
  // generate random data on the host
  thrust::host_vector<int> h_vec(10);
  thrust::generate(h_vec.begin(), h_vec.end(), small_rand);

  printf("original array:\n");
  int i;
  for ( i = 0 ; i < 10 ; i++ )
  {
    printf("%d\n", h_vec[i]);
  }

  // transfer to device and compute sum
  thrust::device_vector<int> d_vec = h_vec;
  int x = thrust::reduce(d_vec.begin(), d_vec.end(), 0, thrust::plus<int>());

  std::cout << "SUM " << x << std::endl;

  // compute cumulative sum on device
  thrust::inclusive_scan(d_vec.begin(), d_vec.end(), d_vec.begin());

  // transfer cumulative sum to host
  h_vec = d_vec;

  printf("cumulative sum:\n");
  for ( i = 0 ; i < 10 ; i++ )
  {
    printf("%d\n", h_vec[i]);
  }

  return 0;
}
