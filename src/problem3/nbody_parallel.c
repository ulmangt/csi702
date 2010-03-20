
#include "mpi.h"

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "nbody_util.h"

void copy_buf( int, float*, float* );

int main( int argc, char** argv )
{
  int numprocs, myid;

  // initialize mpi
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
  float *recv_buf = (float *) malloc( sizeof(float) * PARTICLES_PER_PROC * 3 );
  float *send_buf;

  // allocates space for all the particles ( both guest and host particles )
  float *potential = malloc( sizeof( float ) * PARTICLES_PER_PROC );
  float *host_particles  = malloc( sizeof( float ) * PARTICLES_PER_PROC * 3 );
  float *guest_particles = malloc( sizeof( float ) * PARTICLES_PER_PROC * 3 );

  // initialize the potential array
  int i;
  for ( i = 0 ; i < PARTICLES_PER_PROC ; i++ )
  {
    potential[i] = 0.0;
  }

  float *host_x = host_particles;
  float *host_y = host_particles + PARTICLES_PER_PROC;
  float *host_z = host_particles + PARTICLES_PER_PROC * 2;

  float *guest_x = guest_particles;
  float *guest_y = guest_particles + PARTICLES_PER_PROC;
  float *guest_z = guest_particles + PARTICLES_PER_PROC * 2;

  // load particles from file based on id
  load_particles( get_particle_file_name( myid ), host_x, host_y, host_z );

  // at the start, the guest particles are the host particles
  copy_buf( PARTICLES_PER_PROC * 3, host_particles, guest_particles );

  // pass data around numprocs - 1 times, the last time we will
  // be getting our own data back
  int j, k;
  for ( k = 0 ; k < numprocs - 1 ; k++ )
  {
    // place the guest particles in the send buffer
    send_buf = guest_particles;

    // perform a non blocking send and non blocking receive
    MPI_Irecv( recv_buf, PARTICLES_PER_PROC * 3, MPI_FLOAT, bottom, 1, cart_comm, req );
    MPI_Isend( send_buf, PARTICLES_PER_PROC * 3, MPI_FLOAT, top, 1, cart_comm, req + 1 );
  
    // perform the gravitational potential calculation for the
    // host particles against the guest particles
    for ( i = 0 ; i < PARTICLES_PER_PROC ; i++ )
    {
      for( j = 0 ; j < PARTICLES_PER_PROC ; j++ )
      {
        if ( i != j )
        {
          float R_ij = distance( host_x[i], guest_x[j], host_y[i], guest_y[j], host_z[i], guest_z[j] );
          // G, M[i], and M[j] are all assumed to be 1
          potential[i] = potential[i] + 1.0 / R_ij;
        }
      }
    }

    // wait for communications to complete
    MPI_Waitall( 2, req, stat );
    
    // place the received particles in the guest particles array
    copy_buf( PARTICLES_PER_PROC * 3, recv_buf, guest_particles );
  }

  // shutdown
  MPI_Finalize();

  // TEMPORARY TEST CODE
  // write all particle potentials to file
  write_potentials( get_file_name("potential", myid), PARTICLES_PER_PROC, potential );

  return 0;
}

void copy_buf( int size, float *src, float *dest )
{
  int i;
  for ( i = 0 ; i < size ; i++ )
  {
    dest[i] = src[i];
  }
}
