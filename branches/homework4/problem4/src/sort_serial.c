#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "sort_util.h"

void serial_sort( int*, int , int , int (*)( int, int ) );
int partition( int*, int, int, int (*)( int , int ) );
int compare_integers( int , int );

int main( int argc, char** argv )
{
  srand( time( NULL ) );

  int *values = generate_random_array( ARRAY_SIZE , 100000000 );

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

  free( values );
}

// "The C Programming Language", Brian W. Kernighan and Dennis M. Ritchie used as reference for
// quicksort implementaion (mainly for puzzling out the types necessary to make the implementation
// general enough to support any comparison function).
void serial_sort( int* values, int min, int max, int (*comp)( int , int ) )
{
  if ( max - min <= 0 )
    return;

  if ( max - min == 1 )
  {
    if ( comp( values[min] , values[max] ) > 0 )
    {
      swap_array( values, min, max );
    }
    return;
  }

  int pivot = partition( values, min, max, comp );

  serial_sort( values, min, pivot, comp );
  serial_sort( values, pivot+1, max, comp );
}

  // O(n) in-place algorithm to divide the array between values
  // larger than the element in the pivot index and values less than.
  // Choose the first element in the array as the pivot value.
  // The variable pivot stores the eventual pivot index.
  // Iterate through the array comparing each value with
  // the first entry in the array. Whenever we find a value
  // which is less than the first element (the pivot), increment
  // pivot (since there will be at least 1 element in the
  // less-than portion of the array) and swap the current
  // element with the element at the current pivot index (since
  // the element that we just found should be to the left of
  // the pivot index. Finally, since we've been keeping the
  // pivot value in the first entry, we swap it into its correct
  // place as the last step.
int partition( int* values, int low, int high, int (*comp)( int , int ) )
{
  int i;
  int pivot = low;
  for ( i = low+1 ; i <= high ; i++ )
  {
    if ( comp( values[i] , values[low] ) < 0 )
    {
      swap_array( values, i, ++pivot );
    }
  }

  swap_array( values, pivot, low );

  return pivot;
}

int compare_integers( int i1, int i2 )
{
  if ( i1 < i2 ) return -1;
  else if ( i1 > i2) return 1;
  else return 0;
}
