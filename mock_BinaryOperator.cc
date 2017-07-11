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

#include "mock_BinaryOperator.hh"

////////////////////////////////////////////////////////////////////////////////
// Uniform crossover operator
// Offspring get each allele from either one or the other parent with equal probability
// NOTE: Assumes memory of children individuals is already allocated
void BinaryOperator::uniform_crossover( SolutionPtr const parent1, SolutionPtr const parent2, SolutionPtr child1, SolutionPtr child2, double prob ){

	// Crossover is applied based on a given probability
	if( random_real(0, 1) < prob ){

		for( int i = 0; i < parent1->encoding_length(); i++ ){

			// Get allele from parent1 or from parent2?
			if( random_real(0, 1) < 0.5 ){

				// Child 1 takes i-th allele from Parent 1
				(*child1)[ i ] = (*parent1)[ i ];

				// Child 2 takes i-th allele from Parent 2
				(*child2)[ i ] = (*parent2)[ i ];

			}else{

				// Child 1 takes i-th allele from Parent 2
				(*child1)[ i ] = (*parent2)[ i ];

				// Child 2 takes i-th allele from Parent 1
				(*child2)[ i ] = (*parent1)[ i ];

			}

		}

	}else{

		// Offspring get exact copies of parents' encoding
		for( int i = 0; i < parent1->encoding_length(); i++ ){

				(*child1)[ i ] = (*parent1)[ i ];
				
				(*child2)[ i ] = (*parent2)[ i ];

		}

	}

}
////////////////////////////////////////////////////////////////////////////////

