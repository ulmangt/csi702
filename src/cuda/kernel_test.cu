// includes, system
#include <stdio.h>
#include <assert.h>

__global__ void myFirstKernel( int* d_a )
{
	int index = blockIdx.x * blockDim.x + threadIdx.x;
	d_a[index] = blockIdx.x * 1000 + threadIdx.x;
}

extern "C" void checkCUDAError(const char *msg)
{
    cudaError_t err = cudaGetLastError();
    if( cudaSuccess != err) 
    {
        fprintf(stderr, "Cuda error: %s: %s.\n", msg, cudaGetErrorString( err) );
        exit(-1);
    }                         
}

extern "C" void freeParticleMemory( int *h_a, int *d_a )
{
    // free device memory
    cudaFree( d_a );

    // free host memory
    cudaFreeHost( h_a );
}


extern "C" void finishComputation( int numBlocks, int numThreadsPerBlock, int *h_a, int *d_a )
{
    size_t memSize = numBlocks * numThreadsPerBlock * sizeof(int);

    // block until the device has completed
    cudaThreadSynchronize();

    // check if kernel execution generated an error
    checkCUDAError("kernel execution");

    // Part 4 of 5: device to host copy
    cudaMemcpy( h_a, d_a, memSize, cudaMemcpyDeviceToHost );

    // Check for any CUDA errors
    checkCUDAError("cudaMemcpy");
}

extern "C" void allocateParticleMemory( int numBlocks, int numThreadsPerBlock, void **h_a, void **d_a )
{
    size_t memSize = numBlocks * numThreadsPerBlock * sizeof(int);
    cudaMallocHost( h_a, memSize);
    cudaMalloc( d_a, memSize );
}

extern "C" void callMyFirstKernel( int numBlocks, int numThreadsPerBlock, int* d_a )
{
    // configure and launch kernel
    dim3 dimGrid( numBlocks );
    dim3 dimBlock( numThreadsPerBlock );
    myFirstKernel<<< dimGrid , dimBlock >>>( d_a );
}
