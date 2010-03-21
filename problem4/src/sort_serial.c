#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "sort_util.h"

int main( int argc, char** argv )
{
  srand( time( NULL ) );

  int *values = generate_random_array( ARRAY_SIZE , 100 );

  print_array( ARRAY_SIZE, values );

  serial_sort( values, 0, ARRAY_SIZE-1, (int (*)( int , int )) compare_integers );

  printf("sorting\n");

  print_array( ARRAY_SIZE, values );

  int check = check_sorted( ARRAY_SIZE , values );
  if ( check < 0 )
  {
    //print_array( ARRAY_SIZE, values );
    printf("trouble index %d\n", -check);
  }

  printf( " index of 50 %d\n", binary_search( 50, ARRAY_SIZE, values, compare_integers ) );

  free( values );
}
