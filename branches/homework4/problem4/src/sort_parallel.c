#include "mpi.h"

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "sort_util.h"

int main( int argc, char** argv )
{
  int numprocs, myid;

  // initialize mpi
  MPI_Init( &argc, &argv );
  MPI_Comm_size( MPI_COMM_WORLD, &numprocs );
  MPI_Comm_rank( MPI_COMM_WORLD, &myid );

  //printf( "id %d of %d\n", myid, numprocs );

  MPI_Request *req = (MPI_Request *) malloc( sizeof(MPI_Request) * numprocs );
  MPI_Status *stat = (MPI_Status *) malloc( sizeof(MPI_Status) * numprocs );

  int values_per_proc = ARRAY_SIZE / numprocs;
  int *values = (int *) malloc( sizeof(int) * values_per_proc );

  if ( myid == 0 )
  {
    // set the random seed and generate the random array
    srand( time( NULL ) );
    int *all_values = generate_random_array( ARRAY_SIZE , 100000000 );

    // if there are any extra values, they go on node 0
    int extra_values = ARRAY_SIZE % numprocs;

    // send sections of the array to each proc
    int i;
    for ( i = i ; i < numprocs ; i++ )
    {
      int index = i * values_per_proc + extra_values;
      MPI_Isend( all_values + index , values_per_proc, MPI_INTEGER, i, 1, MPI_COMM_WORLD, req + i - 1 );
    }
    
    // wait for all the nodes to receive their data
    MPI_Waitall( numprocs-1, req, stat );
  }
  else
  {
    // listen for our portion of the array being sent from node 0
    MPI_Recv( values, values_per_proc, MPI_INTEGER, 0, 1, MPI_COMM_WORLD, stat );
  }

  printf( "id %d of %d\n", myid, numprocs ); 
}
