#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "obs_math.h"

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
