#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NUM_REPS 500000000

float frand( float );
int caseOne( int );
int caseTwo( int );

int main( int argc , char* argv[] )
{
	if ( argc < 2 )
	{
		printf("Usage: problem2 [1|2]\n");
		exit(-1);
	}

	int caseNum = atoi( *++argv );
	int count = 0;

	if ( caseNum == 1 )
	{
		count = caseOne( NUM_REPS );
	}
	else if ( caseNum == 2 )
	{
		count = caseTwo( NUM_REPS );
	}
	else
	{
		printf("Usage: problem2 [1|2]\n");
		exit(-1);
	}

	printf("Count: %d/%d\n", count, NUM_REPS);
	exit(0);
}

// return a random float value between 0 and max
float frand( float max )
{
	return (float) rand( ) / (float) RAND_MAX * max ;
}

// runs reps repetitions and returns a count of the number
// of times the condition was satisfied
int caseOne( int reps )
{
	int count = 0;
	int i;
	
	for ( i = 0 ; i < reps ; i++ )
	{
		float x1 = frand( 10 );
		float y1 = frand( 10 );
		float x2 = frand( 10 );
		float y2 = frand( 10 );

		if( sqrt( x1 * x1 + y1 * y1 ) < sqrt( x2 * x2 + y2 * y2 ) )
		{
			count++;
		}
	}

	return count;
}

// runs reps repetitions and returns a count of the number
// of times the condition was satisfied
int caseTwo( int reps )
{
	int count = 0;
	int i;
	
	for ( i = 0 ; i < reps ; i++ )
	{
		float x1 = frand( 10 );
		float y1 = frand( 10 );
		float x2 = frand( 10 );
		float y2 = frand( 10 );

		if( x1 * x1 + y1 * y1 < x2 * x2 + y2 * y2 )
		{
			count++;
		}
	}

	return count;
}
