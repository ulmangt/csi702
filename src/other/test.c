#include <stdio.h>

// linear congruential random number generator constants
// these values are used in java.util.Random.next()
#define a 0x5DEECE66DL
#define c 0xB
#define mask_bits 48
// generate only 31 bits so that we always generate positive integers
#define result_bits 31

// bit mask the least significant m bits
const long mask = ( 1L << mask_bits ) - 1;

const int LCG_RAND_MAX = ( 1 << 31 ) - 1;

// implementation based on examples from:
// http://en.wikipedia.org/wiki/Linear_congruential_generator
// java.util.Random.next()
__device__ int lcg_rand( int x )
{
  // bitwise and with mask is equivalent to mod 2^mask_bits
  long xl = (x * a + c) & mask;
  // we have generated mask_bits but only need result_bits
  // use the highest order bits because they have longer periods
  return (int) ( xl >> ( mask_bits - result_bits ) );
}

// test routine for lcg_rand function
int main( )
{
  int i;
  int x = 23214312;
  float min = 1.0;
  float max = 0.0;
  for ( i = 0 ; i < 10000000 ; i++ )
  {
    float f = (float) x / (float) LCG_RAND_MAX;

    if ( f < min ) min = f;
    if ( f > max ) max = f;

    if ( f < 0 || f >= 1 ) printf("warning! value out of bounds:%f\n", f);
    
    x = lcg_rand(x);
  }

  printf("min %f max %f\n", min, max);
}
