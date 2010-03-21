#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NUM_REPS 40000

void caseOne( int );
void caseTwo( int );

int main( int argc , char* argv[] )
{
	if ( argc < 2 )
	{
		printf("Usage: problem2 [1|2]\n");
		exit(-1);
	}

	int caseNum = atoi( *++argv );

	if ( caseNum == 1 )
	{
		caseOne( NUM_REPS );
	}
	else if ( caseNum == 2 )
	{
		caseTwo( NUM_REPS );
	}
	else
	{
		printf("Usage: problem2 [1|2]\n");
		exit(-1);
	}

	exit(0);
}

void caseOne( int reps )
{
	// Using a[reps][reps] causes the array to be stored on
	// the call stack and we can't make the array large enough.
	// Using malloc initializes the array on the heap.
	float *a = (float*) malloc( reps * reps * sizeof(float) );

	int i,j;

	for ( i = 0 ; i < reps ; i++ )
	{
		for ( j = 0 ; j < reps ; j++ )
		{
			*( a + i * reps + j ) = i * exp( j );
		}
	}

	free( a );
}

void caseTwo( int reps )
{
	float *a = (float*) malloc( reps * reps * sizeof(float) );

	int i,j;

	for ( j = 0 ; j < reps ; j++ )
	{
		for ( i = 0 ; i < reps ; i++ )
		{
			// j * exp(i) used to prevent compiler from
			// moving expression outside of inner loop
			*( a + i * reps + j ) = j * exp( i );
		}
	}

	free( a );
}
