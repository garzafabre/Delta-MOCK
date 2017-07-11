/*******************************************************************************
Copyright (C) 2017 Mario Garza-Fabre, Julia Handl, Joshua Knowles

This file is part of Delta-MOCK.

Delta-MOCK is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Delta-MOCK is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Delta-MOCK. If not, see <http://www.gnu.org/licenses/>.

--------------------------------------------------------------------------------

Author: Mario Garza-Fabre (garzafabre@gmail.com)
Last updated: 10 July 2017

*******************************************************************************/

#ifndef __MOCK_UTIL_HH__
#define __MOCK_UTIL_HH__

/******************
Dependencies
******************/
#include "mock_Global.hh"

/******************
Prototypes/globals
******************/

// Message functions
void error_message( string msg );
void error_message_exit( string msg );

// Comparison functions
int compare_ascending( const void *a, const void *b ); 			// for qsort
int compare_ascending_tuple( const void *a, const void *b );	// for qsort
int compare_descending( const void *a, const void *b );			// for qsort
int compare_descending_tuple( const void *a, const void *b );	// for qsort
int max( int a, int b );
double max( double a, double b );
int min( int a, int b );
double min( double a, double b );

// Allocation/deallocation of float-type vectors and matrices
VectorFloatPtr allocate_VectorFloat( int size );
void deallocate_VectorFloat( VectorFloatPtr vector );
MatrixFloatPtr allocate_MatrixFloat( int rows, int cols );
void deallocate_MatrixFloat( MatrixFloatPtr matrix, int rows );

// Allocation/deallocation of int-type vectors and matrices
VectorIntPtr allocate_VectorInt( int size );
void deallocate_VectorInt( VectorIntPtr vector );
MatrixIntPtr allocate_MatrixInt( int rows, int cols );
void deallocate_MatrixInt( MatrixIntPtr matrix, int rows );

// Allocation/deallocation of double-type vectors and matrices
VectorDoublePtr allocate_VectorDouble( int size );
void deallocate_VectorDouble( VectorDoublePtr vector );
MatrixDoublePtr allocate_MatrixDouble( int rows, int cols );
void deallocate_MatrixDouble( MatrixDoublePtr matrix, int rows );

// Array operations
void sum_VectorFloat( VectorFloatPtr a, VectorFloatPtr b, VectorFloatPtr c, int size );
void sub_VectorFloat( VectorFloatPtr a, VectorFloatPtr b, VectorFloatPtr c, int size );
void divide_VectorFloat_by( VectorFloatPtr vector, float value, VectorFloatPtr result, int size );
void multiply_VectorFloat_by( VectorFloatPtr vector, float value, VectorFloatPtr result, int size );
void shuffle( VectorIntPtr vector, int size );
void shuffle( VectorIntPtr vector, int first, int last );

// Distance functions
float euclidean_distance( VectorFloatPtr v1, VectorFloatPtr v2, int size );

// Generation of random numbers
void initialise_random( unsigned long int seed = 0 );
double random_real( double min, double max );
int random_int( int min, int max );

// MOO functions
int pareto_dominance( VectorDoublePtr vec1, VectorDoublePtr vec2, int size );

// Math functions
long double factorial( int start, int end = 2 );
unsigned long combinations( int n, int k );

#endif
