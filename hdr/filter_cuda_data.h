
#ifndef FILTER_CUDA_DATA_H_INCLUDED
#define FILTER_CUDA_DATA_H_INCLUDED


struct particles {
float x_pos; // meters
float y_pos; // meters
float x_vel; // meters/second
float y_vel; // meters/second
float weight;
float seed; // random seed for particle
};


#endif
