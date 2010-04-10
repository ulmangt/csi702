
#ifndef FILTER_CUDA_UTIL_H_INCLUDED
#define FILTER_CUDA_UTIL_H_INCLUDED

#include "filter_cuda_data.h"

// prints a single particle's information to the console
void print_particle( struct particles *part );

// prints information on all particles to the console
void print_particles( struct particles *list, int num, int downsample );

#endif
