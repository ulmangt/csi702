#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "sort_util.h"

int main( int argc, char** argv )
{
  srand( time( NULL ) );

  int *values = generate_random_array( ARRAY_SIZE , MAX_VALUE );

  serial_sort( values, 0, ARRAY_SIZE-1, (int (*)( int , int )) compare_integers );

  int check = check_sorted( ARRAY_SIZE , values );
  if ( check < 0 )
  {
    printf("trouble index %d\n", -check);
  }
  else
  {
    printf("sort successful\n" );
  }

  free( values );
}
