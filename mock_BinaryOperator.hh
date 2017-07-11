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

#ifndef __MOCK_BINARYOPERATOR_HH__
#define __MOCK_BINARYOPERATOR_HH__

/******************
Dependencies
******************/
#include "mock_Global.hh"
#include "mock_Solution.hh"

/******************
Class definition
******************/
class BinaryOperator{
    
	/******************
	Attributes
	******************/

	private:

	public:

	/******************
	Methods
	******************/

	private:

	public:

		// Constructor / destructor
		BinaryOperator(){}
		~BinaryOperator(){}		

		// Crossover operators
		static void uniform_crossover( SolutionPtr const parent1, SolutionPtr const parent2, SolutionPtr child1, SolutionPtr child2, double const prob );

};

#endif

