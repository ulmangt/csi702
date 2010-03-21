#include <stdio.h>
#include <math.h>
#include <stdlib.h>

int *generate_random_array( int size )
{
  int* r = (int *) malloc( sizeof(int) * size );
  
  int i;
  for ( i = 0 ; i < size ; i++ )
  {
    r[i] = rand( );
  }

  return r;
}

void print_array( int size, int *values )
{
  int i;
  for ( i = 0 ; i < size ; i++ )
  {
    printf( "%d\n", values[i] );
  }
}

void swap( int* a, int* b )
{
  int temp = *a;
  *a = *b;
  *b = temp;
}
