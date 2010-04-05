
#ifndef CUDA_UTIL_H_INCLUDED
#define CUDA_UTIL_H_INCLUDED

extern void copy_particles_host_to_device( int num );

extern void copy_particles_device_to_host( int num );

// allocate memory for num particles on device
extern void d_init_particle_mem( int num );

// free particle memory on device
extern void d_free_particle_mem( );

// function adapted from CUDA Technical Training
extern void checkCUDAError(const char *msg);

#endif
