#ifndef FILTER_CUDA_DATA_H_INCLUDED
#define FILTER_CUDA_DATA_H_INCLUDED

struct particles {
  float *x_pos;
  float *y_pos;
  float *x_vel;
  float *y_vel;
  float *weight;
  float *seed;
}

#endif
