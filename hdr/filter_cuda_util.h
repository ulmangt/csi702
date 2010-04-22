
#ifndef FILTER_CUDA_UTIL_H_INCLUDED
#define FILTER_CUDA_UTIL_H_INCLUDED

#include "filter_cuda_data.h"

// prints a single particle's information to the console
void print_particle( float x_pos, float y_pos, float x_vel, float y_vel, float weight, float seed );

// prints information on all particles to the console
void print_particles( float *d_x_pos, float *d_y_pos, float *d_x_vel, float *d_y_vel, float *d_weight, float *d_seed, int num, int downsample );

// writes information on all particles to a file named out_name
void write_particles( float *d_x_pos, float *d_y_pos, float *d_x_vel, float *d_y_vel, float *d_weight, float *d_seed, char* out_name, int num, int downsample );

#endif
