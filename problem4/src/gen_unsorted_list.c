#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "sort_util.h"

int main( int argc, char** argv )
{
  srand( time( NULL ) );
  int *values = generate_random_array( ARRAY_SIZE , MAX_VALUE );
  write_array( UNSORTED_NAME, values, ARRAY_SIZE );
}
