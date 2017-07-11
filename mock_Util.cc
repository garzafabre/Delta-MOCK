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

#include "mock_Util.hh"


////////////////////////////////////////////////////////////////////////////////
// Displays the provided error message
void error_message( string msg ){

	cerr << "\n\n***************************************************\n";
	cerr << "ERROR:\n---------------\n" << msg << "\n";
	cerr << "***************************************************\n\n";

}
////////////////////////////////////////////////////////////////////////////////
// Displays the provided error message and stop execution
void error_message_exit( string msg ){
	
	error_message( msg );
	exit( 1 );

}
////////////////////////////////////////////////////////////////////////////////
// Compare function used by std::qsort method - single values, ascending
int compare_ascending( const void *a, const void *b ){

    double *pta = (double *)a;
    double *ptb = (double *)b;    
    if( *pta < *ptb ) return -1;
    if( *pta > *ptb ) return 1;      
    return 0;
    
}
////////////////////////////////////////////////////////////////////////////////
// Compare function used by std::qsort method - idx-value tuples, ascending
int compare_ascending_tuple( const void *a, const void *b ){

    double *pta = (double *)a;
    double *ptb = (double *)b;    
    if( pta[1] < ptb[1] ) return -1;
    if( pta[1] > ptb[1] ) return 1;      
    return 0;
    
}
////////////////////////////////////////////////////////////////////////////////
// Compare function used by std::qsort method - single values, descending
int compare_descending( const void *a, const void *b ){

    double *pta = (double *)a;
    double *ptb = (double *)b;    
    if( *pta > *ptb ) return -1;
    if( *pta < *ptb ) return 1;    
    return 0;

}
////////////////////////////////////////////////////////////////////////////////
// Compare function used by std::qsort method - idx-value tuples, descending
int compare_descending_tuple( const void *a, const void *b ){

    double *pta = (double *)a;
    double *ptb = (double *)b;    
    if( pta[1] > ptb[1]) return -1;
    if( pta[1] < ptb[1]) return 1;    
    return 0;

}
////////////////////////////////////////////////////////////////////////////////
// Returns the maximum of two given values
int max( int a, int b ){

    return ( a > b ) ? a : b;
    
}
////////////////////////////////////////////////////////////////////////////////
// Returns the maximum of two given values
double max( double a, double b ){

    return ( a > b ) ? a : b;

}
////////////////////////////////////////////////////////////////////////////////
// Returns the minimum of two given values
int min( int a, int b ){

    return ( a < b ) ? a : b;

}
////////////////////////////////////////////////////////////////////////////////
// Returns the minimum of two given values
double min( double a, double b ){

    return ( a < b ) ? a : b;

}
////////////////////////////////////////////////////////////////////////////////
// Allocates memory for a float-type array of given size, and returns pointer
VectorFloatPtr allocate_VectorFloat( int size ){

    return VectorFloatPtr( new float [ size ] );

}
////////////////////////////////////////////////////////////////////////////////
// Frees memory of the given float-type vector
void deallocate_VectorFloat( VectorFloatPtr vector ){

    delete[] vector;

}
////////////////////////////////////////////////////////////////////////////////
// Allocates memory for a float-type matrix of given size, and returns pointer
MatrixFloatPtr allocate_MatrixFloat( int rows, int cols ){

    MatrixFloatPtr matrix = MatrixFloatPtr( new VectorFloatPtr [ rows ] );

    for( int i=0; i<rows; i++ ) 
        matrix[ i ] = allocate_VectorFloat( cols );

    return matrix;

}
////////////////////////////////////////////////////////////////////////////////
// Frees memory of the given float-type matrix
void deallocate_MatrixFloat( MatrixFloatPtr matrix, int rows ){

    for( int i=0; i<rows; i++ ) 
        deallocate_VectorFloat( matrix[ i ] );

    delete[] matrix;

}
////////////////////////////////////////////////////////////////////////////////
// Allocates memory for a int-type array of given size, and returns pointer
VectorIntPtr allocate_VectorInt( int size ){

    return VectorIntPtr( new int [ size ] );

}
////////////////////////////////////////////////////////////////////////////////
// Frees memory of the given int-type vector
void deallocate_VectorInt( VectorIntPtr vector ){

    delete[] vector;

}
////////////////////////////////////////////////////////////////////////////////
// Allocates memory for a int-type matrix of given size, and returns pointer
MatrixIntPtr allocate_MatrixInt( int rows, int cols ){

    MatrixIntPtr matrix = MatrixIntPtr( new VectorIntPtr [ rows ] );

    for( int i=0; i<rows; i++ ) 
        matrix[ i ] = allocate_VectorInt( cols );

    return matrix;

}
////////////////////////////////////////////////////////////////////////////////
// Frees memory of the given int-type matrix
void deallocate_MatrixInt( MatrixIntPtr matrix, int rows ){

    for( int i=0; i<rows; i++ ) 
        deallocate_VectorInt( matrix[ i ] );

    delete[] matrix;

}
////////////////////////////////////////////////////////////////////////////////
// Allocates memory for a double-type array of given size, and returns pointer
VectorDoublePtr allocate_VectorDouble( int size ){

    VectorDoublePtr x;
    try{

        x = new double [ size ];

    }catch( std::bad_alloc & ba ) {

        std::cerr << "bad_alloc caught: " << ba.what() << '\n';
        exit(1);

    }


    // // VectorDoublePtr x = VectorDoublePtr( new double [ size ] );
    //  = new (nothrow) double [ size ];
    // if(x == nullptr){
    //     cout << "Problem when allocating memory!!!" << endl;
 
    //     exit(0);

    // }

    return x;

}
////////////////////////////////////////////////////////////////////////////////
// Frees memory of the given double-type vector
void deallocate_VectorDouble( VectorDoublePtr vector ){

    delete[] vector;

}
////////////////////////////////////////////////////////////////////////////////
// Allocates memory for a double-type matrix of given size, and returns pointer
MatrixDoublePtr allocate_MatrixDouble( int rows, int cols ){

    MatrixDoublePtr matrix = MatrixDoublePtr( new VectorDoublePtr [ rows ] );

    for( int i=0; i<rows; i++ ) 
        matrix[ i ] = allocate_VectorDouble( cols );

    return matrix;

}
////////////////////////////////////////////////////////////////////////////////
// Frees memory of the given double-type matrix
void deallocate_MatrixDouble( MatrixDoublePtr matrix, int rows ){

    for( int i=0; i<rows; i++ ) 
        deallocate_VectorDouble( matrix[ i ] );

    delete[] matrix;

}
////////////////////////////////////////////////////////////////////////////////
// Sums float-type vectors a and b, and saves the result in c
void sum_VectorFloat( VectorFloatPtr a, VectorFloatPtr b, VectorFloatPtr c, int size ){

    for( int i=0; i<size; i++ ) c[ i ] = ( a[ i ] + b[ i ] );

}
////////////////////////////////////////////////////////////////////////////////
// Sums float-type vectors a and b, and saves the result in c
void sub_VectorFloat( VectorFloatPtr a, VectorFloatPtr b, VectorFloatPtr c, int size ){

    for( int i=0; i<size; i++ ) c[ i ] = ( a[ i ] - b[ i ] );

}
////////////////////////////////////////////////////////////////////////////////
// Divides each element of the given float-type vector by the given value
void divide_VectorFloat_by( VectorFloatPtr vector, float value, VectorFloatPtr result, int size ){

    for( int i=0; i<size; i++ ) result[ i ] = ( vector[ i ] / value );

}
////////////////////////////////////////////////////////////////////////////////
// Multiplies each element of the given float-type vector by the given value
void multiply_VectorFloat_by( VectorFloatPtr vector, float value, VectorFloatPtr result, int size ){

    for( int i=0; i<size; i++ ) result[ i ] = ( vector[ i ] * value );

}
////////////////////////////////////////////////////////////////////////////////
// Computes Euclidean distance between the two given float-type vectors of given size
float euclidean_distance( VectorFloatPtr v1, VectorFloatPtr v2, int size ){

    float distance = 0.0;

    for( int i=0; i<size; i++ ){

        float diff = v1[ i ] - v2[ i ];
        distance += ( diff * diff );
        
    }        

    return sqrt( distance );

}
////////////////////////////////////////////////////////////////////////////////
// Initialises random number generator based on the given seed
// If seed is not given, then 'time' is used as seed
void initialise_random( unsigned long int seed ){

    // Has a seed been provided? Use time if not.
    if( seed == 0 ){
        seed = std::chrono::system_clock::now().time_since_epoch().count();
    }

    // Instantiate generator
    rnd = new default_random_engine( seed );

    // Some examples:
    random_int(0, 10);      // produces integer in [0,10]
    random_real(0, 1);      // produces real in [0,1)
    random_real(20, 111);   // produces real in [20, 111)
    
}
////////////////////////////////////////////////////////////////////////////////
// Real random numbers within range [min, max)
double random_real( double min, double max ){

    uniform_real_distribution< double > distribution( min, max );
    return distribution( *rnd );
}
////////////////////////////////////////////////////////////////////////////////
// Integer random numbers within range [min, max]
int random_int( int min, int max ){

    uniform_int_distribution< int > distribution( min , max );
    return distribution( *rnd );

}
////////////////////////////////////////////////////////////////////////////////
// Pareto dominance relation (NOTE: ASSUMES MINIMISATION)
// Returns:
//  1   if vec1 dominates vec2, 
//  2   if vec2 dominates vec1, 
//  3   if vectors are indifferent, 
//  0   otherwise (incomparable)
int pareto_dominance( VectorDoublePtr vec1, VectorDoublePtr vec2, int size ){
    
    int i, vec1ctr = 0, vec2ctr = 0, equalctr = 0;

    for( i=0; i<size; i++ ){     

        if( vec1[ i ] < vec2[ i ] )        vec1ctr++;        
        else if( vec1[ i ] > vec2[ i ] )   vec2ctr++;     
        else                               equalctr++;

    }
    
    if( vec2ctr == 0 && vec1ctr > 0 )   return 1; // vec1 dominates
    if( vec1ctr == 0 && vec2ctr > 0 )   return 2; // vec2 dominates
    if( equalctr == size )              return 3; // equal vectors

    return 0;
}
////////////////////////////////////////////////////////////////////////////////
// Shuffles (generates a permutation of) the elements of a given integer vector
void shuffle( VectorIntPtr vector, int size ){

    for( int i=0, temp=0, r=0; i<size; i++ ){

        r = random_int( i, size-1 );
        temp = vector[i];
        vector[i] = vector[r];
        vector[r] = temp;

    }

}
////////////////////////////////////////////////////////////////////////////////
// Shuffles (generates a permutation of) the elements of a given integer vector
void shuffle( VectorIntPtr vector, int first, int last ){

    for( int i=first, temp=0, r=0; i<=last; i++ ){

        r = random_int( i, last );
        temp = vector[i];
        vector[i] = vector[r];
        vector[r] = temp;

    }

}
////////////////////////////////////////////////////////////////////////////////
// Computes start! (can stop in a given 'end' value if required)
long double factorial( int start, int end ){

    long double sum = 1.0;

    while (start >= end) {

        sum *= double(start);
        start--;

    }

    return sum; 

}
////////////////////////////////////////////////////////////////////////////////
// Binomial coefficient n over k (combinations of n in k)
unsigned long combinations( int n, int k ){ 

    if( k > n ) return 0;
    if( k == n ) return 1;
    // return (unsigned long)( factorial( n, k+1 ) / factorial( n-k ) );
    return (unsigned long)( factorial( n, n-k+1 ) / factorial( k ) );

}
////////////////////////////////////////////////////////////////////////////////

