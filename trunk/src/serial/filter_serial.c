#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "filter_math.h"

#define NUM_PARTICLES 10000
#define MAX_RANGE 8000
#define MAX_VEL 15

#define MEAN_MANEUVER_TIME 3600

#define INITIAL_MAX_WAYPOINTS 10

#define OUTPUT_NAME "particles.out"

// used to describe ownship and target motion
// as a series of time, x, y tuples
struct waypoint
{
  float time;
  float x_pos;
  float y_pos;
};

struct waypoint_list
{
  int size;
  struct waypoint* waypoints;
};

float *x_pos; // meters
float *y_pos; // meters
float *x_vel; // meters/second
float *y_vel; // meters/second
float *weight;

void init_particle_mem( int );
void init_particle_val( int, float, float );

void time_update( int, float, float );
void time_update_index( int, float );
void maneuver_index( int );

void write_particles( char*, int );
struct waypoint_list *read_waypoints( char* );
void print_waypoints( struct waypoint_list* );
void print_waypoint( struct waypoint* );
int interpolate( struct waypoint_list* , float, float*, float* );
int interpolate_waypoints( struct waypoint*, struct waypoint*, float, float*, float*);

int main( int argc, char* argv )
{
  struct waypoint_list *waypoints = read_waypoints( "data/waypoints1.txt" );
  print_waypoints( waypoints );
  float x_pos, y_pos, time;

  for ( time = 0 ; time < 2000.0 ; time += 100 )
  {
    interpolate( waypoints, time, &x_pos, &y_pos );
    printf( "at time %f x_pos %f y_pos %f\n", time, x_pos, y_pos );
  }

  /*
     init_particle_mem( NUM_PARTICLES );
     init_particle_val( NUM_PARTICLES, MAX_RANGE, MAX_VEL );
     time_update( NUM_PARTICLES, 100.0, MEAN_MANEUVER_TIME );
     write_particles( OUTPUT_NAME, NUM_PARTICLES );
   */
}

// allocate memory for num particles
void init_particle_mem( int num )
{
  x_pos  = ( float* ) malloc( sizeof( float ) * num );
  y_pos  = ( float* ) malloc( sizeof( float ) * num );
  x_vel  = ( float* ) malloc( sizeof( float ) * num );
  y_vel  = ( float* ) malloc( sizeof( float ) * num );
  weight = ( float* ) malloc( sizeof( float ) * num );
}

// initialize positions, velocities, and weights of num particles
void init_particle_val( int num, float max_range, float max_vel )
{
  int i;

  for ( i = 0 ; i < num ; i++ )
  {
    x_pos[i]  = frand( -max_range, max_range );
    y_pos[i]  = frand( -max_range, max_range );
    x_vel[i]  = frand( -max_vel, max_vel );
    y_vel[i]  = frand( -max_vel, max_vel );
    weight[i] = 1;
  }
}

// time update the particles
// adjust positions based on velocity
// velocity changes occur exponentially distributed in time
// num number of particles to update
// seconds number of seconds into future to update particles
// mean_maneuver the mean time between particle maneuvers (exponentially distributed)
void time_update( int num, float seconds, float mean_maneuver )
{
  int i;

  float remaining_time;
  float next_maneuver = erand( mean_maneuver );

  for ( i = 0 ; i < num ; i++ )
  {
    printf("Updating particle %d seconds %f\n", i, seconds);

    remaining_time = seconds;

    while ( 1 )
    {
      if ( next_maneuver < remaining_time )
      {
        printf("Particle %d maneuvering update time (next_maneuver) %f remaining_time %f\n", i, next_maneuver, remaining_time);

        time_update_index( i, next_maneuver );
        maneuver_index( i );
        remaining_time -= next_maneuver;
        next_maneuver = erand( mean_maneuver );
      }
      else
      {
        printf("Particle %d finished time update time %f\n", i, remaining_time);

        time_update_index( i, remaining_time );
        next_maneuver -= remaining_time;

        printf("next maneuver %f\n", next_maneuver);
        break;
      }
    }
  }
}

// time updates a single particle with no maneuver
void time_update_index( int i, float seconds )
{
  x_pos[i] = x_pos[i] + x_vel[i] * seconds;
  y_pos[i] = y_pos[i] + y_vel[i] * seconds;
}

// maneuver (adjust the velocity) of a single particle
// this is an instantanious change (no time elapses)
void maneuver_index( int i )
{
  x_vel[i] = frand( -MAX_VEL, MAX_VEL );
  y_vel[i] = frand( -MAX_VEL, MAX_VEL );
}

// writes the current set of particles out to disk as
// a tab delimited text file
void write_particles( char* out_name, int num )
{
  FILE* file = fopen( out_name, "w" );

  int i;

  for ( i = 0 ; i < num ; i++ )
  {
    fprintf( file, "%f\t%f\t%f\t%f\t%f\n", x_pos[i], y_pos[i], x_vel[i], y_vel[i], weight[i] );
  }

  int success = fclose( file );
}

// reads the path of a vessel as a series of time, x, y waypoints from a file
// vessel positions at any time may be derived by interpolating between waypoints
struct waypoint_list *read_waypoints( char* in_name )
{
  FILE* file = fopen( in_name, "r" );

  int max_waypoints = 10;
  int num_waypoints = 0;

  struct waypoint_list *waypoint_list = malloc( sizeof( struct waypoint_list ) );
  waypoint_list->waypoints = malloc( sizeof( struct waypoint ) * max_waypoints );

  while ( 1 )
  {
    struct waypoint *waypoint = (waypoint_list->waypoints) + num_waypoints;    

    int success = fscanf( file, "%f%f%f", &waypoint->time, &waypoint->x_pos, &waypoint->y_pos );

    if ( success == EOF )
      break;

    num_waypoints++;

    if ( num_waypoints == max_waypoints )
    {
      max_waypoints = 2 * max_waypoints;
      waypoint_list->waypoints = realloc( waypoint_list->waypoints , sizeof( struct waypoint ) * max_waypoints );
    }
  }

  waypoint_list->size = num_waypoints;

  return waypoint_list;
}

// prints a waypoint_list structure (for debugging / verification)
void print_waypoints( struct waypoint_list *waypoint_list )
{
  int i;

  for ( i = 0 ; i < waypoint_list->size ; i++ )
  {
    //struct waypoint *waypoint = waypoint_list->waypoints;
    print_waypoint( (waypoint_list->waypoints)+i );
    //printf( "%f %f %f\n", (waypoint+i)->time, (waypoint+i)->x_pos, (waypoint+i)->y_pos );
  }
}

void print_waypoint( struct waypoint *waypoint )
{
  printf( "%f %f %f\n", waypoint->time, waypoint->x_pos, waypoint->y_pos );
}

// interpolates between the waypoints in waypoint list to determine the x_pos and y_pos at the given tim
// values are returned by setting the x_pos and y_pos pointers
// a return value of 1 indicates success, a return value of 0 indicates an error
int interpolate( struct waypoint_list *waypoint_list , float time , float *x_pos, float *y_pos )
{
  int i;

  struct waypoint *previous_waypoint;

  for ( i = 0 ; i < waypoint_list->size ; i++ )
  {
    struct waypoint *current_waypoint = (waypoint_list->waypoints)+i;

    if ( current_waypoint->time > time )
    {
      // the first waypoint timestamp is after the requested time thus
      // we don't have two points to interpolate between
      if ( i == 0 )
        return 0;

      return interpolate_waypoints( previous_waypoint, current_waypoint, time, x_pos, y_pos);
    }

    previous_waypoint = current_waypoint;
  }

  // the last waypoint timestamp is before the requested time
  return 0;
}

// interpolates between two given waypoints
// time should be between start->time and end->time
int interpolate_waypoints( struct waypoint *start, struct waypoint *end, float time, float *x_pos, float *y_pos )
{
  float time_diff = end->time - start->time;
  float weight = ( time - start->time ) / time_diff;

  *x_pos = end->x_pos * ( weight ) + start->x_pos * ( 1 - weight );
  *y_pos = end->y_pos * ( weight ) + start->y_pos * ( 1 - weight );

  return 1;
}

float azimuth( float to_x_pos, float to_y_pos, float from_x_pos, float from_y_pos )
{
  float x_diff = from_x_pos - to_x_pos;
  float y_diff = from_y_pos - to_y_pos;

  if ( x_diff == 0 && y_diff > 0 ) return M_PI / 2.0;
  if ( x_diff == 0 && y_diff < 0 ) return -M_PI / 2.0;
  return atan2( y_diff, x_diff );
}
