#ifndef OBS_MATH_H_INCLUDED
#define OBS_MATH_H_INCLUDED



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

struct observation_list *generate_observations( struct waypoint_list*, struct waypoint_list*, int, float, float, float, float );
float generate_observation( int, float, float, float, float, float );
float generate_azimuth_observation( float, float, float, float, float );
float generate_range_observation( float, float, float, float, float );

struct observation_list *combine_observations( struct observation_list *, struct observation_list*v);

float apply_observation( struct observation*, float, float, float, float, float );
float apply_azimuth_observation( struct observation*, float, float, float, float, float );
float apply_range_observation( struct observation*, float, float, float, float, float );

int interpolate( struct waypoint_list* , float, float*, float* );
int interpolate_waypoints( struct waypoint*, struct waypoint*, float, float*, float*);

float range( float, float, float, float );
float azimuth( float, float, float, float );

#endif
