#ifndef CUDA_CONSTANTS_H_INCLUDED
#define CUDA_CONSTANTS_H_INCLUDED

#define NUM_PARTICLES 100000
#define MAX_RANGE 20000 // meters
#define MAX_VEL 15 // meters per second

#define MAX_POS_PERTURB 1000.0 // meters
#define MAX_VEL_PERTURB    2.0 // meters per second

#define MEAN_MANEUVER_TIME 3600 // seconds

#define INITIAL_MAX_WAYPOINTS 10

#define OUTPUT_NAME "particles.out"

// particles in host memory
extern float *h_x_pos; // meters
extern float *h_y_pos; // meters
extern float *h_x_vel; // meters/second
extern float *h_y_vel; // meters/second
extern float *h_weight;
extern float *h_seed; // random seed for particle

// particles in device memory
extern float *d_x_pos; // meters
extern float *d_y_pos; // meters
extern float *d_x_vel; // meters/second
extern float *d_y_vel; // meters/second
extern float *d_weight;
extern float *d_seed; // random seed for particle

#endif
