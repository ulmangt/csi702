#include <stdio.h>
#include <assert.h>

#include "kernel_test.h"


////////////////////////////////////////////////////////////////////////////////
// Program main
////////////////////////////////////////////////////////////////////////////////
int main( int argc, char** argv) 
{
    // pointer for host memory
    int *h_a;

    // pointer for device memory
    int *d_a;

    // define grid and block size
    int numBlocks = 8;
    int numThreadsPerBlock = 8;

    // allocate host and device memory
    allocateParticleMemory( numBlocks, numThreadsPerBlock, (void**) &h_a, (void**) &d_a );

    // Part 2 of 5: configure and launch kernel
    callMyFirstKernel( numBlocks, numThreadsPerBlock, d_a );

    finishComputation( numBlocks, numThreadsPerBlock, h_a, d_a );

    // Part 5 of 5: verify the data returned to the host is correct
    int i, j;
    for (i = 0; i < numBlocks ; i++)
    {
        for (j = 0; j < numThreadsPerBlock ; j++)
        {
            assert(h_a[i * numThreadsPerBlock + j] == 1000 * i + j);
        }
    }

    freeParticleMemory( h_a, d_a );

    // If the program makes it this far, then the results are correct and
    // there are no run-time errors.  Good work!
    printf("Correct!\n");

    return 0;
}
