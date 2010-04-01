#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "sort_util.h"

int main( int argc, char** argv )
{
  srand( time( NULL ) );

  int values[ ARRAY_SIZE ];

  read_array( UNSORTED_NAME, values, ARRAY_SIZE );

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

  // write out the sorted array for verification
  write_array( "serial_sorted_list", values, ARRAY_SIZE );
}
