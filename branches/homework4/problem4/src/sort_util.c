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
