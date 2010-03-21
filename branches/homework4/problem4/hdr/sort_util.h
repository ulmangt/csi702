#ifndef SORT_UTIL_H_INCLUDED
#define SORT_UTIL_H_INCLUDED

#define ARRAY_SIZE 10

int randn( int );

int *generate_random_array( int, int );
void print_array( int, int* );
int check_sorted( int, int* );

void swap_array( int*, int, int );
void swap( int*, int* );

#endif
