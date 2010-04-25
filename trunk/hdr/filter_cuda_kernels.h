
#ifndef FILTER_CUDA_KERNELS_H_INCLUDED
#define FILTER_CUDA_KERNELS_H_INCLUDED

#include "obs_math.h"

extern void init_particles( float *d_x_pos, float *d_y_pos, float *d_x_vel, float *d_y_vel, float *d_weight, float *d_seed, int num );

extern float sum_weight( float *d_weights, float *d_temp_weight1, float *d_temp_weight2, int num );

extern float sum_weight_thrust( float *d_weights, int num );

extern void information_update( struct observation *obs, float *d_x_pos, float *d_y_pos, float *d_x_vel, float *d_y_vel, float *d_weight, float *d_seed, int num );

extern void time_update( float *d_x_pos, float *d_y_pos, float *d_x_vel, float *d_y_vel, float *d_weight, float *d_seed, int num, float time_sec, float mean_maneuver );



extern void copy_array_host_to_device( float *host, float *device, int num );

extern void copy_array_device_to_host( float *host, float *device, int num );

// allocate memory for num particles on host
extern float* h_init_array_mem( int num );

// allocate memory for num particles on device
extern float*d_init_array_mem( int num );

// free particle memory on host
extern void h_free_particle_mem( float* array );

// free particle memory on device
extern void d_free_particle_mem( float* array );


#endif
