#ifndef NBODY_UTIL_H_INCLUDED
#define NBODY_UTIL_H_INCLUDED

float frand( );
char *get_particle_file_name( int );
void write_particles( char*, int );
void load_particles ( char*, float**, float**, float** );

#endif
