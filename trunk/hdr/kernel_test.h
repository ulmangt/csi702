

extern void freeParticleMemory( int *h_a, int *d_a );
extern void finishComputation( int numBlocks, int numThreadsPerBlock, int *h_a, int *d_a );
extern void allocateParticleMemory( int numBlocks, int numThreadsPerBlock, void **h_a, void **d_a );
extern void callMyFirstKernel( int numBlocks, int numThreadsPerBlock, int* d_a );
extern void checkCUDAError(const char *msg);
