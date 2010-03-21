#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "sort_util.h"

void serial_sort( int*, int , int , int (*)( int, int ) );
int compare_integers( int , int );

int main( int argc, char** argv )
{
  int *values = generate_random_array( ARRAY_SIZE );

  print_array( ARRAY_SIZE, values );

  serial_sort( values, 0, ARRAY_SIZE-1, (int (*)( int , int )) compare_integers );

  free( values );
}

// "The C Programming Language", Brian W. Kernighan and Dennis M. Ritchie used as reference for
// quicksort implementaion (mainly for puzzling out the types necessary to make the implementation
// general enough to support any comparison function).
void serial_sort( int* values, int min, int max, int (*comp)( int , int ) )
{
  int i;
  for ( i = min ; i <= max-1 ; i++ )
  {
    printf("value %d comp %d\n", values[i], comp( values[i] , values[i+1] ) );
  } 
}

int compare_integers( int i1, int i2 )
{
  if ( i1 < i2 ) return -1;
  else if ( i1 > i2) return 1;
  else return 0;
}
