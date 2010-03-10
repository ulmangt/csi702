#ifndef FILTER_IO_H_INCLUDED
#define FILTER_IO_H_INCLUDED

struct waypoint_list *read_waypoints( char* );
void print_waypoints( struct waypoint_list* );
void print_waypoint( struct waypoint* );

void print_observations( struct observation_list *observation_list );
void print_observation( struct observation *obs );

#endif
