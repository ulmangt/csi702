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

int interpolate( struct waypoint_list* , float, float*, float* );
int interpolate_waypoints( struct waypoint*, struct waypoint*, float, float*, float*);
float azimuth( float, float, float, float );

#endif
