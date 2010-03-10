#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "obs_math.h"
#include "filter_math.h"

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

// generates observations based on the given sensor and target positions
// sensor : a list of position waypoints for the sensor making the observations
// target : a list of position waypoints for the target being observed
// type : the type of observations being made (see observation_type enum)
// start_time : the time of the first observation
// interval_time : the time between observations
// end_time : no observations will be recorded after end_time
// error : the random error to add to each observation
struct observation_list *generate_observations( struct waypoint_list *sensor, struct waypoint_list *target, int type, float error, float start_time, float interval_time, float end_time )
{
  int observation_count = (int) ( floor( (end_time - start_time) / interval_time ) + 1 );

  //printf("obs count %d\n", observation_count );

  struct observation_list *observation_list = malloc( sizeof(struct observation_list) );
  observation_list->observations = malloc( sizeof(struct observation) * observation_count );
  observation_list->size = observation_count;

  int i;

  float x_pos_target, y_pos_target, x_pos_sensor, y_pos_sensor;

  for ( i = 0 ; i < observation_count ; i++ )
  {
    struct observation *obs = (observation_list->observations) + i;

    obs->time = start_time + i * interval_time;
    obs->error = error;
    obs->type = type;

    printf( "generating obs %d %f\n", i, obs->time );
    
    interpolate( sensor, obs->time, &x_pos_sensor, &y_pos_sensor );
    interpolate( target, obs->time, &x_pos_target, &y_pos_target );

    obs->x_pos = x_pos_sensor;
    obs->y_pos = y_pos_sensor;

    obs->value = generate_observation( type, error, x_pos_sensor, y_pos_sensor, x_pos_target, y_pos_target );
  }

  return observation_list;
}

float generate_observation( int type, float error, float x_pos_sensor, float y_pos_sensor, float x_pos_target, float y_pos_target )
{
  switch( type )
  {
    case AZIMUTH:
      return generate_azimuth_observation( error, x_pos_sensor, y_pos_sensor, x_pos_target, y_pos_target );
    default:
      return -1.0;
  }
}

float generate_azimuth_observation( float error, float x_pos_sensor, float y_pos_sensor, float x_pos_target, float y_pos_target )
{
  float true_value = azimuth( x_pos_sensor, y_pos_sensor, x_pos_target, y_pos_target );
  float observed_value = grand( true_value, error );
  return observed_value;
}

float azimuth( float to_x_pos, float to_y_pos, float from_x_pos, float from_y_pos )
{
  float x_diff = from_x_pos - to_x_pos;
  float y_diff = from_y_pos - to_y_pos;

  if ( x_diff == 0 && y_diff > 0 ) return M_PI / 2.0;
  if ( x_diff == 0 && y_diff < 0 ) return -M_PI / 2.0;

  return atan2( y_diff, x_diff );
}
