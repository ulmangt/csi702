#include "mpi.h"

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "sort_util.h"

int *calculate_bins( int, int*, int, int );

int main( int argc, char** argv )
{
  int i;
  int *all_values;
  int *values;
  int *bin_edges;
  int numprocs, myid;

  // initialize mpi
  MPI_Init( &argc, &argv );
  MPI_Comm_size( MPI_COMM_WORLD, &numprocs );
  MPI_Comm_rank( MPI_COMM_WORLD, &myid );

  // special case the 1 processor (i.e. serial) case
  if ( numprocs == 1 )
  {
    srand( time( NULL ) );
    all_values = generate_random_array( ARRAY_SIZE , MAX_VALUE );
    serial_sort( all_values, 0, ARRAY_SIZE-1, (int (*)( int , int )) compare_integers );
    print_array( ARRAY_SIZE, values );
    exit(0);
  }

  //printf( "id %d of %d\n", myid, numprocs );

  MPI_Request *req = (MPI_Request *) malloc( sizeof(MPI_Request) * numprocs );
  MPI_Status *stat = (MPI_Status *) malloc( sizeof(MPI_Status) * numprocs );

  // number of values on most procs
  int values_per_proc = ARRAY_SIZE / numprocs;
  // if there are any extra values, they go on node 0
  int extra_values = ARRAY_SIZE % numprocs;
  // the actual number of values on this proc (either 
  // values_per_proc or values_per_proc + extra_values
  int num_values;

  // divide the data among the processors
  if ( myid == 0 )
  {
    // set the random seed and generate the random array
    srand( time( NULL ) );
    all_values = generate_random_array( ARRAY_SIZE , MAX_VALUE );

    // no need to send our values to ourself, we simply take the first values
    values = all_values;

    num_values = values_per_proc + extra_values;

    // send sections of the array to each proc
    for ( i = 1 ; i < numprocs ; i++ )
    {
      int index = i * values_per_proc + extra_values;
      MPI_Isend( all_values + index , values_per_proc, MPI_INTEGER, i, 1, MPI_COMM_WORLD, req + i - 1 );
    }
    
    // wait for all the nodes to receive their data
    MPI_Waitall( numprocs-1, req, stat );

    // calculate bin edges
    bin_edges = calculate_bins( ARRAY_SIZE, all_values, numprocs, SUBSAMPLE );

    printf( "bin edges\n" );
    print_array( numprocs - 1 , bin_edges );

    // send the calculated bins to all nodes
    MPI_Bcast( bin_edges, numprocs - 1, MPI_INTEGER, 0, MPI_COMM_WORLD );

  }
  else
  {
    num_values = values_per_proc;

    // listen for our portion of the array being sent from node 0
    values = (int *) malloc( sizeof(int) * values_per_proc );
    MPI_Recv( values, values_per_proc, MPI_INTEGER, 0, 1, MPI_COMM_WORLD, stat );

    // listen for the bin sizes
    bin_edges = (int *) malloc( sizeof(int) * ( numprocs - 1 ) );
    MPI_Bcast( bin_edges, numprocs - 1, MPI_INTEGER, 0, MPI_COMM_WORLD );
  }

  // go through all our data and sort it into bins to send to the appropriate processor
  // for simplicity, create enough space in each processor's bin to hold all the data
  int **all_bin_values = (int **) malloc( sizeof(int *) * numprocs );
  // the current free index into bin_values for each processor
  int *bin_index = (int *) malloc( sizeof(int) * numprocs );
  for ( i = 0 ; i < numprocs ; i++ )
  {
    bin_index[i] = 0;
    all_bin_values[i] = (int *) malloc( sizeof(int) * ARRAY_SIZE );
  }

  // iterate through the nodes data values, placing them into the correct bin
  for ( i = 0 ; i < num_values ; i++ )
  {
    int value = values[i];
    int index = binary_search( value, numprocs - 1, bin_edges, compare_integers ) + 1;
    int *bin_values = all_bin_values[index];
    int free_index = bin_index[index]++;
    bin_values[free_index] = value;

    //printf( " node %d put value %d in bin %d (free index %d) \n", myid, value, index, free_index );
  }

  // send binned data to appropriate nodes
  int *my_bin_values = all_bin_values[ myid ];
  int free_index = bin_index[ myid ];
  int remaining_size = ARRAY_SIZE - free_index;
  // loop over nodes, i represents the node each other node is sending to
  for ( i = 0 ; i < numprocs ; i++ )
  {
    // if we are being sent to, listen to each other node in turn for data
    if ( myid == i )
    {
      int j, count;
      for ( j = 0, count = 0 ; j < numprocs ; j++ )
      {
        if ( i != j )
        { 
          MPI_Recv( my_bin_values + free_index, remaining_size, MPI_INTEGER, j, 1, MPI_COMM_WORLD, stat );
          int num_received;
          MPI_Get_count( stat, MPI_INTEGER, &num_received );
          free_index += num_received;
          remaining_size -= num_received;
          //printf( "node: %d received from: %d num values: %d\n", i , j , num_received );
        }
      }
    }
    // otherwise, send our binned data for node i
    else
    {
      //printf( "node: %d sent to: %d num values: %d\n", myid , i , bin_index[i] );
      MPI_Send( all_bin_values[i], bin_index[i], MPI_INTEGER, i, 1, MPI_COMM_WORLD );
    }
  }

  printf("binned data for node %d (size %d)\n", myid, free_index);
  //print_array( free_index, my_bin_values );
  
  //TODO combine data from all other procs and do local sort

  //TODO send sorted data back to node 0

  if ( myid == 0 )
  {
    free( all_values );
  }
  else
  {
    free( values );
  }

  // free allocated memory
  free( req );
  free( stat );
  free( bin_edges );

  // shutdown
  MPI_Finalize();
}

// choose bins
// so that we get some idea of the distribution of the array,
// choose SUBSAMPLE * (numprocs - 1) values, sort them, and
// use SUBSAMPLE, 2 * SUBSAMPLE, ... as the bins
int *calculate_bins( int size, int* values, int numprocs, int subsample )
{
    int i;

    int subsample_size = subsample * ( numprocs - 1 );

    if ( subsample_size > size )
      subsample_size = size;

    int subsample_step = size / subsample_size;

    if ( subsample_step < 1 )
      subsample_step = 1;

    printf( "size %d step %d\n", subsample_size, subsample_step );    

    int *subsample_values = (int *) malloc( sizeof(int) * subsample_size );

    for ( i = 0 ; i < subsample_size ; i++ )
    {
      subsample_values[i] = values[subsample_step*i];
    }

    serial_sort( subsample_values, 0, subsample_size-1, (int (*)( int , int )) compare_integers );

    //printf("subsample\n");
    //print_array( subsample_size , subsample );

    int *low_bin = (int *) malloc( sizeof(int) * ( numprocs - 1 ) );

    for ( i = 0 ; i < numprocs - 1 ; i++ )
    {
      low_bin[i] = subsample_values[subsample*(i+1)-1];
    }

    //printf("bin low edges\n");
    //print_array( numprocs - 1 , low_bin );

    return low_bin;
}
