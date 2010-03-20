
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

  printf( "id %d of %d\n", myid, numprocs );

  // create a 1 dimensional periodic cartesian grid
  // with numprocs nodes and store in cart_comm
  MPI_Comm cart_comm;
  int periods = 1;
  MPI_Cart_create( MPI_COMM_WORLD, 1, &numprocs, &periods, 1, &cart_comm );

  // get rank in the new comm group
  int mycartid;
  MPI_Comm_rank( cart_comm, &mycartid );

  // get our neighbors ( 1 space away in the 0th cartesian dimension)
  int bottom, top;
  MPI_Cart_shift( cart_comm, 0, 1, &bottom, &top );

  // get our coordinates in the cartesian grid
  int coords;
  MPI_Cart_coords( cart_comm, mycartid, 1, &coords );

  // intitialize request, status, send, and receive buffers
  MPI_Request *req = (MPI_Request *) malloc( sizeof(MPI_Request) * 2 );
  MPI_Status *stat = (MPI_Status *) malloc( sizeof(MPI_Status) * 2 );
  int *recv_buf = (int *) malloc( sizeof(int) );
  int *send_buf = (int *) malloc( sizeof(int) );

  int data = mycartid;

  int i;
  for ( i = 0 ; i < numprocs ; i++ )
  {
    // place the message in the send buffer
    *send_buf = data;

    // perform a non blocking send and non blocking receive
    MPI_Irecv( recv_buf, 1, MPI_INT, bottom, 1, cart_comm, req );
    MPI_Isend( send_buf, 1, MPI_INT, top, 1, cart_comm, req + 1 );

    printf(" node %d send/recv complete\n", mycartid ); 
  
    // wait for communications to complete
    MPI_Waitall( 2, req, stat );

    data = *recv_buf;
  
    printf(" node %d got message: %d\n", mycartid, data );
  }

  // shutdown
  MPI_Finalize();

  return 0;
}
