
#ifndef FILTER_CUDA_UTIL_H_INCLUDED
#define FILTER_CUDA_UTIL_H_INCLUDED

// writes the position along the waypoint_list at the given time to a file
void write_positions( char* out_name, struct waypoint_list *waypoints, float time );

// prints a single particle's information to the console
void print_particle( float x_pos, float y_pos, float x_vel, float y_vel, float weight, float seed );

// prints information on all particles to the console
void print_particles( struct particles *host_particles, int num, int downsample );

// writes information on all particles to a file named out_name
void write_particles( struct particles *host_particles, char* out_name, int num, int downsample );

#endif
