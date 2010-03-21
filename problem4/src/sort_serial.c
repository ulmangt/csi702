#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "sort_util.h"

void serial_sort( int*, int , int , int (*)( int, int ) );
int compare_integers( int , int );

int main( int argc, char** argv )
{
  srand( time( NULL ) );

  int *values = generate_random_array( ARRAY_SIZE , 10 );

  print_array( ARRAY_SIZE, values );

  serial_sort( values, 0, ARRAY_SIZE-1, (int (*)( int , int )) compare_integers );

  printf("partitioning\n");

  print_array( ARRAY_SIZE, values );

  free( values );
}

// "The C Programming Language", Brian W. Kernighan and Dennis M. Ritchie used as reference for
// quicksort implementaion (mainly for puzzling out the types necessary to make the implementation
// general enough to support any comparison function).
void serial_sort( int* values, int min, int max, int (*comp)( int , int ) )
{
  partition( values , min , max , 0 , comp );
}

// O(n) in-place algorithm to divide the array between values larger than the
// element in the pivot index and values less than. Because the pivot index
// may change, partition returns the new pivot index.
int partition( int* values, int min, int max, int pivot, int (*comp)( int , int ) )
{
  while( min < max )
  {
    int min_pivot = comp( values[min] , values[pivot] );
    int max_pivot = comp( values[max] , values[pivot] );
    
    if ( min_pivot > 0 && max_pivot < 0 )
    {
      swap_array( values, min++, max-- );
    }
    if ( min_pivot > 0 )
    {
      swap_array( values, min++, pivot );
    }
    else if ( max_pivot < 0 )
    {
      swap_array( values, max--, pivot );
    }
    else {
      min++;
      max--;
    }
  } 
}

int compare_integers( int i1, int i2 )
{
  if ( i1 < i2 ) return -1;
  else if ( i1 > i2) return 1;
  else return 0;
}
