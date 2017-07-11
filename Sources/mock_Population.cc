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

#include "mock_Population.hh"

////////////////////////////////////////////////////////////////////////////////
// Constructor
Population::Population( int max ) : 

	_size( 0 ),
	_max_size( max )
	
{

	// Allocate memory and initialise
	_solution = (SolutionPtr *)( new SolutionPtr [ _max_size ] );
	for( int i=0; i<_max_size; i++ ) _solution[ i ] = nullptr; 

}
////////////////////////////////////////////////////////////////////////////////
// Destructor
Population::~Population(){

	// Deallocate memory
	for( int i=0; i<_max_size; i++ ) delete _solution[ i ];
	delete[] _solution;
	_size = -1;
	
}
////////////////////////////////////////////////////////////////////////////////
// Adds the given solution to the population (assign pointer)
void Population::add( SolutionPtr sol ){

	// Verify available capacity
	if( _size >= _max_size){
		error_message_exit( "Cannot add more solutions to the population. Maximum capacity reached!!!" );
	}

	// Include solution
	_solution[ _size++ ] = sol;

}
////////////////////////////////////////////////////////////////////////////////
// Adds COPY of the given solution to the population
// void Population::add_copy( SolutionPtr sol ){

// 	// Create copy and use add method above
// 	add( SolutionPtr( new Solution( sol ) ) );

// }
///////////////////////////////////////////////////////////////////
// Clears list of solutions
void Population::clear(){

	// Set each of the solution pointers to nullptr
	for( int i=0; i<_size; i++ ) _solution[ i ] = nullptr;

	// Set size to zero
	_size = 0;

	// This method is only intended to be used in auxiliary (temporal) populations of variable 
	// size, therefore should not delete (deallocate memory) of pointed solution objects

}
///////////////////////////////////////////////////////////////////
// void Population::print_population(){

// 	cout << "Current population size: " << _size << endl;
// 	cout << "------------------------------------------------------------------------------------------" << endl;
// 	for(int i=0; i<_size; i++){
// 		cout << i << ":   ";
// 		_solution[ i ]->print_solution();
// 	}
// }
////////////////////////////////////////////////////////////////////////////////

