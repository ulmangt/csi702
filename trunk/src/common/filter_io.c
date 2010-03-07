#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "obs_math.h"
#include "filter_io.h"

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
