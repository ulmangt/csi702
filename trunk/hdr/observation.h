#ifndef OBS_MATH_H_INCLUDED
#define OBS_MATH_H_INCLUDED

#include "waypoint.h"

enum observation_type
{
  AZIMUTH = 1,
  RANGE,
  SPEED,
};

struct observation
{
  // observation_type (determines how value is interpreted)
  int type;
  // the timestamp of the observation
  float time;
  // x position of sensor
  float x_pos;
  // y position of sensor
  float y_pos;
  // observation value
  float value;
  // observation error
  float error;
};

struct observation_list
{
  int size;
  struct observation* observations;
};

void print_observations( struct observation_list *observation_list );
void print_observation( struct observation *obs );

struct observation_list *generate_observations( struct waypoint_list*, struct waypoint_list*, int, float, float, float, float );
float generate_observation( int, float, float, float, float, float );
float generate_azimuth_observation( float, float, float, float, float );
float generate_range_observation( float, float, float, float, float );

struct observation_list *combine_observations( struct observation_list *, struct observation_list*v);

float apply_observation( struct observation*, float, float, float, float, float );
float apply_azimuth_observation( struct observation*, float, float, float, float, float );
float apply_range_observation( struct observation*, float, float, float, float, float );

float range( float, float, float, float );
float azimuth( float, float, float, float );

#endif
