#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "observation.h"
#include "waypoint.h"

/////////////////////////////////////////////////////
// waypoint.c
//
// This file contains helper functions for dealing
// with waypoint lists. In order to generate observations
// for the particle filters, we must know the true
// paths the target and sensor follow over time. These
// paths are defined in waypoint text files as (time,x,y)
// tuples. These functions read such files into a
// struct waypoint_list which is defined in waypoint.h.
//
// Functions in waypoint.c can interpolate between
// waypoints to find the position at any given time.
//
// See data/waypoint1.txt for an example waypoint file.
/////////////////////////////////////////////////////


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
    print_waypoint( (waypoint_list->waypoints)+i );
  }
}

void print_waypoint( struct waypoint *waypoint )
{
  printf( "%f %f %f\n", waypoint->time, waypoint->x_pos, waypoint->y_pos );
}

void print_observations( struct observation_list *observation_list )
{
  int i;

  for ( i = 0 ; i < observation_list->size ; i++ )
  {
    print_observation( (observation_list->observations)+i );
  }
}

void print_observation( struct observation *obs )
{
  printf( "%d %f %f %f %f %f\n", obs->type, obs->time, obs->x_pos, obs->y_pos, obs->value, obs->error );
}
