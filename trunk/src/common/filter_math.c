#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "filter_math.h"
#include "convert.h"

// return a random float value evenly distributed between 0 and max
float frand0( float max )
{
  return ( float ) rand( ) / ( float ) RAND_MAX * max ;
}

// return a random float value evenly distributed between min and max
float frand( float min, float max )
{
  float diff = max - min;
  return frand0( diff ) + min;
}

// return an exponentially distributed random float
float erand( float inv_lambda )
{
  return -log( frand0( 1.0 ) ) * inv_lambda;
}

// return a float value chosen from the normal distribution
// algorithm from polar method of G. E. P. Box, M. E. Muller, and G. Marsaglia, 
// as described by Donald E. Knuth in The Art of Computer Programming,
// Volume 2: Seminumerical Algorithms, section 3.4.1, subsection C, algorithm P
// also the implementation used by java.util.Random.nextGaussian
float grand0( )
{
  double v1, v2, s;
  
  do
  { 
    v1 = frand(-1,1);
    v2 = frand(-1,1);
    s = v1 * v1 + v2 * v2;
  }
  while ( s >= 1 || s == 0 );
  
  double multiplier = sqrt(-2 * log(s)/s);
  
  return v1 * multiplier;
}

// returns a random float value from a gaussian distribution
// with the given mean and sigma
float grand( float mean, float sigma )
{
  return mean + sigma * grand0( );
}

// calculates the probability density function for the gaussian
// distribution with given mean and sigma
float gvalue( float value, float mean, float sigma )
{
  float z = ( value - mean ) / sigma ;
  return exp( -0.5 * z * z ) / ( sqrt( 2.0 * PI ) * sigma );
}
