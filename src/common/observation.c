#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "observation.h"
#include "filter_math.h"

/////////////////////////////////////////////////////
// observation.c
//
// This file contains helper functions for dealing
// with observation_list structs. These structures,
// defined in observation.h, contain time ordered
// lists of errored observations generated from a
// target and sensor moving along paths prescribed
// by waypoint_list structures.
/////////////////////////////////////////////////////




// Generates observations based on the given sensor and target positions.
//
// sensor : a list of position waypoints for the sensor making the observations
// target : a list of position waypoints for the target being observed
// type : the type of observations being made (see observation_type enum)
// start_time : the time of the first observation
// interval_time : the time between observations
// end_time : no observations will be recorded after end_time
// error : the random error to add to each observation
struct observation_list *generate_observations(
                               struct waypoint_list *sensor, struct waypoint_list *target,
                               int type, float error, float start_time, float interval_time, float end_time )
{
  int observation_count = (int) ( floor( (end_time - start_time) / interval_time ) + 1 );

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
    
    interpolate( sensor, obs->time, &x_pos_sensor, &y_pos_sensor );
    interpolate( target, obs->time, &x_pos_target, &y_pos_target );

    obs->x_pos = x_pos_sensor;
    obs->y_pos = y_pos_sensor;

    obs->value = generate_observation( type, error, x_pos_sensor, y_pos_sensor, x_pos_target, y_pos_target );
  }

  return observation_list;
}

// Takes two observation_list structures and combines the data contained in both
// into a single combined observation_list.
struct observation_list *combine_observations( struct observation_list *l1, struct observation_list *l2 )
{
  int total_size = l1->size + l2->size;
  struct observation_list *observation_list = malloc( sizeof(struct observation_list) );
  observation_list->observations = malloc( sizeof(struct observation) * total_size );
  observation_list->size = total_size;

  struct observation *observations = observation_list->observations;

  int l1i = 0;
  int l2i = 0;
  int i;
  for ( i = 0 ; i < total_size ; i++ )
  {
    if ( l1i < l1->size && ( l2i >= l2->size || l1->observations[l1i].time < l2->observations[l2i].time ) )
    {
      observations[i] = l1->observations[l1i++];
    }
    else 
    {
      observations[i] = l2->observations[l2i++];
    }
  }

  return observation_list;
}

// Generates a single observation of the specified type for the
// given sensor and target positions. This function is not
// usually called directly, but instead is used by generate_observations()
// to automatically build observation_list structures from two
// waypoint_list structures
float generate_observation( int type, float error, float x_pos_sensor,
                            float y_pos_sensor, float x_pos_target, float y_pos_target )
{
  switch( type )
  {
    case AZIMUTH:
      return generate_azimuth_observation( error, x_pos_sensor, y_pos_sensor, x_pos_target, y_pos_target );
    case RANGE:
      return generate_range_observation( error, x_pos_sensor, y_pos_sensor, x_pos_target, y_pos_target );
    default:
      return -1.0;
  }
}

float generate_azimuth_observation( float error, float x_pos_sensor,
                                    float y_pos_sensor, float x_pos_target, float y_pos_target )
{
  float true_value = azimuth( x_pos_sensor, y_pos_sensor, x_pos_target, y_pos_target );
  float observed_value = grand( true_value, error );
  return observed_value;
}

float generate_range_observation( float error, float x_pos_sensor,
                                  float y_pos_sensor, float x_pos_target, float y_pos_target )
{
  float true_value = range( x_pos_sensor, y_pos_sensor, x_pos_target, y_pos_target );
  float observed_value = grand( true_value, error );
  return observed_value;
}

float apply_observation( struct observation *obs, float x_pos_particle, float y_pos_particle,
                         float x_vel_particle, float y_vel_particle, float weight_particle )
{
  switch( obs->type )
  {
    case AZIMUTH:
      return apply_azimuth_observation( obs, x_pos_particle, y_pos_particle, x_vel_particle, y_vel_particle, weight_particle );
    case RANGE:
      return apply_range_observation( obs, x_pos_particle, y_pos_particle, x_vel_particle, y_vel_particle, weight_particle );
    default:
      return -1.0;
  }
}

float apply_azimuth_observation( struct observation *obs, float x_pos_particle, float y_pos_particle,
                                 float x_vel_particle, float y_vel_particle, float weight_particle )
{
  float particle_azimuth = azimuth( obs->x_pos , obs->y_pos , x_pos_particle , y_pos_particle );
  float observed_azimuth = obs->value;
  float likelihood = gvalue( particle_azimuth - observed_azimuth , 0.0 , obs->error );
  return weight_particle * likelihood;
}

float apply_range_observation( struct observation *obs, float x_pos_particle, float y_pos_particle,
                               float x_vel_particle, float y_vel_particle, float weight_particle )
{
  float particle_range = range( obs->x_pos , obs->y_pos , x_pos_particle , y_pos_particle );
  float observed_range = obs->value;
  float likelihood = gvalue( particle_range - observed_range , 0.0 , obs->error );
  return weight_particle * likelihood;
}

float range( float to_x_pos, float to_y_pos, float from_x_pos, float from_y_pos )
{
  float x_diff = from_x_pos - to_x_pos;
  float y_diff = from_y_pos - to_y_pos;

  return sqrt( x_diff * x_diff + y_diff * y_diff );
}

float azimuth( float to_x_pos, float to_y_pos, float from_x_pos, float from_y_pos )
{
  float x_diff = from_x_pos - to_x_pos;
  float y_diff = from_y_pos - to_y_pos;

  if ( x_diff == 0 && y_diff > 0 ) return M_PI / 2.0;
  if ( x_diff == 0 && y_diff < 0 ) return -M_PI / 2.0;

  return atan2( y_diff, x_diff );
}
