
#include "mpi.h"

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "nbody_util.h"

int main( int argc, char** argv )
{
  int numprocs, myid, mycartid;

  MPI_Init( &argc, &argv );
  MPI_Comm_size( MPI_COMM_WORLD, &numprocs );
  MPI_Comm_rank( MPI_COMM_WORLD, &myid );

  printf( "id %d of %d\n", myid, numprocs );

  MPI_Comm cart_comm;

  // create a 1 dimensional periodic cartesian grid
  // with numprocs nodes and store in cart_comm
  MPI_Cart_create( MPI_COMM_WORLD, 1, numprocs, 1, 1, &cart_comm );

  // get rank in the new comm group
  MPI_Comm_rank( cart_comm, &mycartid );
  
  int source, topid, bottomid;

  // get our top neighbor
  MPI_Cart_shift( cart_comm, 0, 1, &source, &topid );

  // get our bottom neighbor
  MPI_Cart_shift( cart_comm, 0, -1, &source, &bottomid );  

  void * buf = malloc( sizeof(int) );

  printf( "id %d top neighbor %d bottom neighbor %d\n", source, topid, bottomid );

  MPI_Finalize();

  return 0;
}
