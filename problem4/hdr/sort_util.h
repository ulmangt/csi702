#ifndef SORT_UTIL_H_INCLUDED
#define SORT_UTIL_H_INCLUDED

#define ARRAY_SIZE 1000000
#define SUBSAMPLE 64
#define MAX_VALUE 10000000
#define UNSORTED_NAME "unsorted_list"

int randn( int );

int *generate_random_array( int, int );
int check_sorted( int, int* );

void print_array( int, int* );

void swap_array( int*, int, int );
void swap( int*, int* );

void serial_sort( int*, int , int , int (*)( int, int ) );
int partition( int*, int, int, int (*)( int , int ) );
int compare_integers( int , int );

int binary_search( int, int, int* , int (*)( int , int ) );

void copy_buf( int, int*, int* );

void write_array( char*, int*, int );
int* read_array( char*, int );

#endif
