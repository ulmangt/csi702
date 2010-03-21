#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NUM_REPS 1000000000

float frand( float );

void caseOne( long );
void caseTwo( long );

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

// return a random float value between 0 and max
float frand( float max )
{
        return (float) rand( ) / (float) RAND_MAX * max ;
}

void caseOne( long reps )
{
	long i;
	long count[] = { 0, 0, 0 };

	for ( i = 0 ; i < reps ; i++ )
	{
		if ( i % 999997 == 0 )
		{
			count[0]++;
		}
		else if ( i % 999 == 0 )
		{
			count[1]++;
		}
		else if ( i % 2 == 0 )
		{
			count[2]++;
		}
	}

	printf("%ld %ld %ld\n", count[0], count[1], count[2]);
}

void caseTwo( long reps )
{
	long i;
	long count[] = { 0, 0, 0};

	for ( i = 0 ; i < reps ; i++ )
	{
		if ( i % 2 == 0 )
		{
			count[0]++;
		}
		else if ( i % 999 == 0 )
		{
			count[1]++;
		}
		else if ( i % 999997 == 0 )
		{
			count[2]++;
		}
	}

	printf("%ld %ld %ld\n", count[0], count[1], count[2]);
}
