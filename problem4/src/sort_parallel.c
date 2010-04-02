#include "mpi.h"

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "sort_util.h"

// Data arrays are realloced dynamically because
// each node does not know how much data it will
// end up with. When an array grows, it grows by
// at most this amount.
#define MAX_GROW 10000

int *calculate_bins( int, int*, int, int );

int main( int argc, char** argv )
{
  // set the random seed
  srand( time( NULL ) );

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
    // read data to sort from a file, the commented line generates data randomly
    //all_values = read_array( UNSORTED_NAME, ARRAY_SIZE );
    all_values = generate_random_array( ARRAY_SIZE, MAX_VALUE );
    serial_sort( all_values, 0, ARRAY_SIZE-1, (int (*)( int , int )) compare_integers );
    print_array( ARRAY_SIZE, values );
    exit(0);
  }

  MPI_Request *req = (MPI_Request *) malloc( sizeof(MPI_Request) * numprocs );
  MPI_Status *stat = (MPI_Status *) malloc( sizeof(MPI_Status) * numprocs );

  // number of values on most procs
  int values_per_proc = ARRAY_SIZE / numprocs;
  // if there are any extra values, they go on node 0
  int extra_values = ARRAY_SIZE % numprocs;
  int num_values;

  // divide the data among the processors
  if ( myid == 0 )
  {
    // read data to sort from a file, the commented line generates data randomly
    all_values = read_array( UNSORTED_NAME, ARRAY_SIZE );
    //all_values = generate_random_array( ARRAY_SIZE, MAX_VALUE );

    // no need to send our values to ourself, we simply take the first values
    values = all_values;

    // the actual number of values on this proc (either 
    // values_per_proc or values_per_proc + extra_values
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

    printf( "bin edges:\n" );
    print_array( numprocs - 1 , bin_edges );

    // send the calculated bins to all nodes
    MPI_Bcast( bin_edges, numprocs - 1, MPI_INTEGER, 0, MPI_COMM_WORLD );
  }
  else
  {
    // the actual number of values on this proc (either 
    // values_per_proc or values_per_proc + extra_values
    num_values = values_per_proc;

    // listen for our portion of the array being sent from node 0
    values = (int *) malloc( sizeof(int) * values_per_proc );
    MPI_Recv( values, values_per_proc, MPI_INTEGER, 0, 1, MPI_COMM_WORLD, stat );

    // listen for the bin sizes
    bin_edges = (int *) malloc( sizeof(int) * ( numprocs - 1 ) );
    MPI_Bcast( bin_edges, numprocs - 1, MPI_INTEGER, 0, MPI_COMM_WORLD );
  }

  // go through all our data and sort it into bins to send to the appropriate processor

  // store an array for each processor, these are the bins we will sort our data into
  int **all_bin_values = (int **) malloc( sizeof(int *) * numprocs );

  // the number of data values in each processor's bin
  int *bin_index = (int *) malloc( sizeof(int) * numprocs );

  // the maximum size allocated for each processor's bin
  int *max_bin_size = (int *) malloc( sizeof(int) * numprocs );

  // allocate memory for each bin
  for ( i = 0 ; i < numprocs ; i++ )
  {
    bin_index[i] = 0;
    max_bin_size[i] = values_per_proc;
    all_bin_values[i] = (int *) malloc( sizeof(int) * values_per_proc );
  }

  // iterate through the nodes data values, placing them into the correct bin
  for ( i = 0 ; i < num_values ; i++ )
  {
    // get the next data value
    int value = values[i];

    // perform a binary search through the bins to determine where to place the data value
    int index = binary_search( value, numprocs - 1, bin_edges, compare_integers ) + 1;

    // get the proper bin based on its index
    int *bin_values = all_bin_values[index];
    int free_index = bin_index[index]++;
    int max_size = max_bin_size[index];   

    // if there is not enough space in the bin extend it
    if ( free_index >= max_size )
    {
      int new_size = max_size + MAX_GROW;
      all_bin_values[index] = (int *) realloc( all_bin_values[index] , sizeof(int) * new_size );
      bin_values = all_bin_values[index];
      max_bin_size[index] = new_size;
    }

    // place the data element in the bin
    bin_values[free_index] = value;
  }

  // send binned data to appropriate nodes
  int *my_bin_values = all_bin_values[ myid ];
  int free_index = bin_index[ myid ];
  int max_size = max_bin_size[ myid ];
  
  // loop over nodes, i represents the node each other node is sending to
  for ( i = 0 ; i < numprocs ; i++ )
  {
    // if we are being sent to, listen to each other node in turn for data
    if ( myid != i )
    {
      printf("node %d sending %d values to node %d\n", myid, bin_index[i], i);
      MPI_Isend( all_bin_values[i], bin_index[i], MPI_INTEGER, i, 2, MPI_COMM_WORLD, req + i );
    }
  }

  // receive binned data from other nodes
  int j, count;
  for ( j = 0, count = 0 ; j < numprocs ; j++ )
  {
    if ( myid != j )
    {
      // check how much data is arriving
      MPI_Probe( j, 2, MPI_COMM_WORLD, stat );
      int num_received;
      MPI_Get_count( stat, MPI_INTEGER, &num_received );

      printf("node %d receiving %d values from node %d (max %d free %d)\n", myid, num_received, j, max_size, free_index );
          
      // allocate space for the incoming data if necessary
      if ( num_received >= max_size - free_index )
      {
        max_size = free_index + num_received;
        my_bin_values = (int *) realloc( my_bin_values, sizeof(int) * max_size );
      }

      // receive the data and update the free_index pointer
      MPI_Recv( my_bin_values + free_index, max_size - free_index, MPI_INTEGER, j, 2, MPI_COMM_WORLD, stat );
      free_index += num_received;
    }
  }

  printf("binned data for node %d (size %d)\n", myid, free_index);

  // perform a local serial sort on the data in our bin
  serial_sort( my_bin_values, 0, free_index-1, (int (*)( int , int )) compare_integers );

  // send the sorted bin data back to node 0
  if ( myid == 0 )
  {
    // allocate enough space for all the values
    my_bin_values = (int *) realloc( my_bin_values , sizeof(int) * ARRAY_SIZE );

    for ( i = 1 ; i < numprocs ; i++ )
    {
      MPI_Recv( my_bin_values + free_index, ARRAY_SIZE - free_index, MPI_INTEGER, i, 3, MPI_COMM_WORLD, stat );
      int num_received;
      MPI_Get_count( stat, MPI_INTEGER, &num_received );
      free_index += num_received;

      printf( "got data %d from node %d (total %d)\n", num_received, i, free_index);
    }

    // verify that the array is sorted
    int check = check_sorted( ARRAY_SIZE , my_bin_values );
    if ( check < 0 )
    {
      printf("trouble index %d\n", -check);
    }
    else
    {
      printf("sort successful\n");
    }

    // write out the sorted array for verification
    write_array( "parallel_sorted_list", my_bin_values, ARRAY_SIZE );
  }
  else
  {
    MPI_Send( my_bin_values, free_index, MPI_INTEGER, 0, 3, MPI_COMM_WORLD );
  }

  // free allocated memory
  if ( myid == 0 )
  {
    free( all_values );
  }
  else
  {
    free( values );
  }

  free( bin_index );
  free( max_bin_size );

/*
  for ( i = 0 ; i < numprocs ; i++ )
  {
    free( all_bin_values[i] );
  }

  free( all_bin_values );
*/

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

    // determine the number of elements to choose from the entire array
    int subsample_size = subsample * ( numprocs - 1 );

    if ( subsample_size > size )
      subsample_size = size;

    // to choose subsample_size values from the unsorted array, step through
    // the array, skipping subsample_step elements each step
    int subsample_step = size / subsample_size;

    if ( subsample_step < 1 )
      subsample_step = 1;

    // choose the subsample values
    int *subsample_values = (int *) malloc( sizeof(int) * subsample_size );

    for ( i = 0 ; i < subsample_size ; i++ )
    {
      subsample_values[i] = values[subsample_step*i];
    }

    // sort the subsample values
    serial_sort( subsample_values, 0, subsample_size-1, (int (*)( int , int )) compare_integers );

    // choose the bin edges
    int *low_bin = (int *) malloc( sizeof(int) * ( numprocs - 1 ) );

    for ( i = 0 ; i < numprocs - 1 ; i++ )
    {
      low_bin[i] = subsample_values[subsample/2+subsample*i-1];
    }

    return low_bin;
}
