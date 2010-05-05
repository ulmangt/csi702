#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "observation.h"
#include "waypoint.h"
#include "convert.h"
#include "filter_cuda_kernels.h"

/////////////////////////////////////////////////////
// filter_cuda_util.c
//
// Basic IO helper functions for writing output files.
/////////////////////////////////////////////////////



// writes the interpolated x and y positions at the given time along the waypoints list
// this is used to record the position of the sensor and ownship at the end of the scenario
// for graphical output purposes 
void write_positions( char* out_name, struct waypoint_list *waypoints, float time )
{
  float x_pos, y_pos;
  interpolate( waypoints , time , &x_pos, &y_pos );
  
  FILE* file = fopen( out_name, "w" );
  fprintf( file, "%f\t%f\n", x_pos, y_pos );

  int success = fclose( file );
}

// prints a single particle's information to the console
void print_particle( float x_pos, float y_pos, float x_vel, float y_vel, float weight, float seed )
{
  printf( "%f\t%f\t%f\t%f\t%f\t%f\n", x_pos, y_pos, x_vel, y_vel, weight, seed );
}

// prints information on all particles to the console
void print_particles( struct particles *host_particles, int num, int downsample )
{
  int i;

  for ( i = 0 ; i < num ; i = i + downsample )
  {
    print_particle( host_particles->x_pos[i], host_particles->y_pos[i], host_particles->x_vel[i],
                    host_particles->y_vel[i], host_particles->weight[i], host_particles->seed[i] );
  }
}

// writes the current set of particles out to disk as
// a tab delimited text file
void write_particles( struct particles *host_particles, char* out_name, int num, int downsample )
{
  FILE* file = fopen( out_name, "w" );

  int i;

  for ( i = 0 ; i < num ; i = i + downsample )
  {
    fprintf( file, "%f\t%f\t%f\t%f\t%f\n", host_particles->x_pos[i], host_particles->y_pos[i],
                                           host_particles->x_vel[i], host_particles->y_vel[i],
                                           host_particles->weight[i] );
  }

  int success = fclose( file );
}
