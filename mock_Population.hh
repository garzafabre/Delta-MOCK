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

#ifndef __MOCK_POPULATION_HH__
#define __MOCK_POPULATION_HH__

/******************
Dependencies
******************/
#include "mock_Population.fwd.hh"
#include "mock_Global.hh"
#include "mock_Solution.hh"

/******************
Class definition
******************/
class Population{
    
	/******************
	Attributes
	******************/

	private:

		SolutionPtr * _solution;	// Array of pointers to solution objects
		int _size;					// Current size of the population
		int _max_size;				// Maximum size of the population

	/******************
	Methods
	******************/

	public:

		// Constructor / destructor
		Population( int max );
		~Population();

		// Accesors
 		SolutionPtr operator[]( const int i );
		int size(){ return _size; }
		int max_size(){ return _max_size; }

		// To add solutions to the population
		void add( SolutionPtr sol );
		// void add_copy( SolutionPtr sol );

		// Empty population
		void clear();

		// void print_population();
		
};

////////////////////////////////////////////////////////////////////////////////
// Direct access to population members
inline SolutionPtr Population::operator[]( const int i ){

	return _solution[ i ];

}
////////////////////////////////////////////////////////////////////////////////

#endif



