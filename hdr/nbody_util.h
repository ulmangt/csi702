#ifndef NBODY_UTIL_H_INCLUDED
#define NBODY_UTIL_H_INCLUDED

#define PARTICLES_PER_PROC 1
#define NUM_PROC 10

float frand( );
float distance( float, float, float, float, float, float );

char* get_particle_file_name( int );
char* get_file_name( char*, int );

void write_particles( char*, int );
void write_potentials( char*, int num_particles, float* );

void load_particles ( char*, float*, float*, float* );

#endif
