
#ifndef FILTER_CUDA_KERNELS_H_INCLUDED
#define FILTER_CUDA_KERNELS_H_INCLUDED

#include "obs_math.h"
#include "filter_cuda_data.h"

extern void init_particles( struct particles *device_particles, int num );

extern float sum_weight( float *d_weights, float *d_temp_weight1, float *d_temp_weight2, int num );

extern float sum_weight_thrust( float *d_weights, int num );

extern void information_update( struct observation *obs, struct particles *device_particles, int num );

extern void time_update( struct particles *device_particles, int num, float time_sec, float mean_maneuver );

extern void resample( struct particles *device_particles, struct particles *device_particles_swap, int num );

extern void init_array( float *array, float value, int num );

extern void multiply( float *array, float factor, int num );

extern void copy_array_host_to_device( float *host, float *device, int num );

extern void copy_array_device_to_host( float *host, float *device, int num );

// allocate memory for num particles on host
extern float* h_init_array_mem( int num );

// allocate memory for num particles on device
extern float*d_init_array_mem( int num );

extern struct particles* h_init_particles_mem( );

extern struct particles* d_init_particles_mem( );

// free particle memory on host
extern void h_free_mem( void * );

// free particle memory on device
extern void d_free_mem( void * );

#endif
