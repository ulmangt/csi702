#include <stdio.h>
#include <math.h>
#include <stdlib.h>

// returns a psudo-random int from 0 to n-1
int randn( int n )
{
  return rand( ) % n;
}

// generates an array of psudo-random int from 0 to n-1
int *generate_random_array( int size , int n )
{
  int* r = (int *) malloc( sizeof(int) * size );
  
  int i;
  for ( i = 0 ; i < size ; i++ )
  {
    r[i] = randn( n );
  }

  return r;
}

// prints the values in the provided array to stdout
void print_array( int size, int *values )
{
  int i;
  for ( i = 0 ; i < size ; i++ )
  {
    printf( "%d\n", values[i] );
  }
}

// determines whether an array is sorted
// returns 1 for a sorted array, a negative number
// for a non-sorted array
int check_sorted( int size, int *values )
{
  int i;
  for ( i = 0 ; i < size-1 ; i++ )
  {
    if ( values[i] > values[i+1] )
      return -i;
  }

  return 1;
}

// swaps two integers in an array based on their index
void swap_array( int* array, int index1, int index2 )
{
  int temp = array[index1];
  array[index1] = array[index2];
  array[index2] = temp;
}

// swaps two integers
void swap( int* a, int* b )
{
  int temp = *a;
  *a = *b;
  *b = temp;
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

// return the index of the key in the values array, if it exists, or
// the index of the next smallest number in the array
int binary_search( int key, int size, int* values , int (*comp)( int , int ) )
{
  int min = 0; // inclusive
  int max = size; // exclusive
  int index = (max - min) / 2 + min;

  while ( max > min )
  {
    int comparison = comp( values[index], key );
    if ( comparison == 0 )
    {
      return index;
    }
    else if ( comparison < 0 )
    {
      min = index + 1;
      index = (max - min) / 2 + min;
    }
    else
    {
      max = index;
      index = (max - min) / 2 + min;
    }
  }

  while ( index >= 0 )
  {
    if ( comp( values[index], key ) > 0 )
      index--;
    else
      return index;
  }

  return index;
}
