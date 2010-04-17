
#ifndef FILTER_CUDA_KERNELS_H_INCLUDED
#define FILTER_CUDA_KERNELS_H_INCLUDED

#include "filter_cuda_data.h"

extern void sum_weight( struct particles *host, int num );

extern void information_update( struct observation *obs, struct particles *host, int num );

extern void time_update( struct particles *list, int num, float time_sec, float mean_maneuver );

extern void init_particles( struct particles *host, int num );

extern void copy_particles_host_to_device( struct particles *host, struct particles *device, int num );

extern void copy_particles_device_to_host( struct particles *host, struct particles *device, int num );

// allocate memory for num particles on host
extern struct particles *h_init_particle_mem( int num );

// allocate memory for num particles on device
extern struct particles *d_init_particle_mem( int num );

// free particle memory on host
extern void h_free_particle_mem( struct particles *list );

// free particle memory on device
extern void d_free_particle_mem( struct particles *list );


#endif
