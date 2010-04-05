
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "cuda_constants.h"

extern "C" void copy_particles_host_to_device( int num )
{
  int size = sizeof( float ) * num;

  cudaMemcpy( h_x_pos, d_x_pos, size, cudaMemcpyHostToDevice );
  cudaMemcpy( h_y_pos, d_y_pos, size, cudaMemcpyHostToDevice );
  cudaMemcpy( h_x_vel, d_x_vel, size, cudaMemcpyHostToDevice );
  cudaMemcpy( h_y_vel, d_y_vel, size, cudaMemcpyHostToDevice );
  cudaMemcpy( h_weight, d_weight, size, cudaMemcpyHostToDevice );
  cudaMemcpy( h_seed, d_seed, size, cudaMemcpyHostToDevice );
}

extern "C" void copy_particles_device_to_host( int num )
{
  int size = sizeof( float ) * num;

  cudaMemcpy( h_x_pos, d_x_pos, size, cudaMemcpyDeviceToHost );
  cudaMemcpy( h_y_pos, d_y_pos, size, cudaMemcpyDeviceToHost );
  cudaMemcpy( h_x_vel, d_x_vel, size, cudaMemcpyDeviceToHost );
  cudaMemcpy( h_y_vel, d_y_vel, size, cudaMemcpyDeviceToHost );
  cudaMemcpy( h_weight, d_weight, size, cudaMemcpyDeviceToHost );
  cudaMemcpy( h_seed, d_seed, size, cudaMemcpyDeviceToHost );
}

// allocate memory for num particles on device
extern "C" void d_init_particle_mem( int num )
{
  int size = sizeof( float ) * num;

  cudaMalloc( (void **) &d_x_pos, size );
  cudaMalloc( (void **) &d_y_pos, size );
  cudaMalloc( (void **) &d_x_vel, size );
  cudaMalloc( (void **) &d_y_vel, size );
  cudaMalloc( (void **) &d_weight, size );
  cudaMalloc( (void **) &d_seed, size );
}

// free particle memory on device
extern "C" void d_free_particle_mem( )
{
  cudaFree( d_x_pos );
  cudaFree( d_y_pos );
  cudaFree( d_x_vel );
  cudaFree( d_y_vel );
  cudaFree( d_weight );
  cudaFree( d_seed );
}

// function adapted from CUDA Technical Training
extern "C" void checkCUDAError(const char *msg)
{
    cudaError_t err = cudaGetLastError();
    if( cudaSuccess != err) 
    {
        fprintf(stderr, "Cuda error: %s: %s.\n", msg, cudaGetErrorString( err) );
        exit(EXIT_FAILURE);
    }                         
}
