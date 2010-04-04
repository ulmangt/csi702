#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "obs_math.h"
#include "filter_io.h"
#include "convert.h"

#define NUM_PARTICLES 100000

// particles in host memory
float *h_x_pos; // meters
float *h_y_pos; // meters
float *h_x_vel; // meters/second
float *h_y_vel; // meters/second
float *h_weight;
float *h_seed; // random seed for particle

// particles in device memory
float *d_x_pos; // meters
float *d_y_pos; // meters
float *d_x_vel; // meters/second
float *d_y_vel; // meters/second
float *d_weight;
float *d_seed; // random seed for particle

int main( int argc, char** argv )
{
  printf("Hello CUDA\n");

  //h_init_particle_mem( NUM_PARTICLES );

  //h_init_seed( NUM_PARTICLES );

  //d_init_particle_mem( NUM_PARTICLES );

  // copy the seeds generated on the host to the device
  //copy_particles_host_to_device( NUM_PARTICLES );
}
