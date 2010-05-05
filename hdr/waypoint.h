#ifndef FILTER_IO_H_INCLUDED
#define FILTER_IO_H_INCLUDED

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

struct waypoint_list *read_waypoints( char* );
void print_waypoints( struct waypoint_list* );
void print_waypoint( struct waypoint* );

#endif
