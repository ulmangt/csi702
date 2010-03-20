
#include "mpi.h"

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "nbody_util.h"

int main( int argc, char** argv )
{
  int numprocs, myid;

  MPI_Init( &argc, &argv );
  MPI_Comm_size( MPI_COMM_WORLD, &numprocs );
  MPI_Comm_rank( MPI_COMM_WORLD, &myid );

  printf( "Hello world %d %d\n", numprocs, myid );

  MPI_Finalize();

  return 0;
}
