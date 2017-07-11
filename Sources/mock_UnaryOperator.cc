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

#include "mock_UnaryOperator.hh"

////////////////////////////////////////////////////////////////////////////////
// Uniform mutation
// Mutation occurs at each encoding position with a given fixed probability
// void UnaryOperator::uniform_mutation( SolutionPtr const sol, double prob ){

// 	// Given 'prob' defines the total number of alleles expected to mutate
// 	// Compute mutation probability as "prob / encoding_length"
// 	prob = prob / sol->encoding_length();

// 	// Mutate each allele with given probability
// 	for( int i = 0; i < sol->encoding_length(); i++ ){

// 		if( random_real(0, 1) < prob ){

// 			// Replace allele with a randomly selected alternative
// 			(*sol)[ i ] = sol->random_encoding( i, (*sol)[ i ] );

// 		}

// 	}

// }
////////////////////////////////////////////////////////////////////////////////
// Uniform mutation
// If k does not exceed k_user, mutation occurs at each encoding position with a given fixed probability
// Otherwise, mutation occurs only at "0"-value positions with a prob. which depends on the extent of this excess
// void UnaryOperator::krepair_uniform_mutation( SolutionPtr const sol, double prob ){

// 	// Determine excess in k value (based on number of 0-value positions)
// 	int zeros = 0;
// 	for( int i = 0; i < sol->encoding_length(); i++ ){

// 		if( (*sol)[ i ] == 0 ) zeros++;

// 	}
// 	int excess = zeros + 1 - k_user;

// 	// Is k exceeding the upper bound k_user?
// 	if( excess > 0 ){

// 		// Repair-based mutation
// 		// Set probability of mutation according to the magnitude of excess
// 		prob = double( excess ) / double( zeros );

// 		// Mutate only 0-value positions in order to re-introduce MST edges and reduce K
// 		for( int i = 0; i < sol->encoding_length(); i++ ){

// 			if( (*sol)[ i ] == 0 && random_real(0, 1) < prob ){

// 				// Re-introduce MST edge
// 				(*sol)[ i ] = 1;

// 			}

// 		}

// 	}else{

// 		// Apply conventional uniform mutation
// 		uniform_mutation( sol, prob );

// 	}

// }
////////////////////////////////////////////////////////////////////////////////
// Relevant edge mutation operator
// Mutation occurs at each "relevant" encoding position with a fixed probability 
// void UnaryOperator::relevant_edge_mutation( SolutionPtr const sol, double prob ){

// 	// Given 'prob' defines the total number of "relevant" alleles expected to mutate
// 	// Compute mutation probability as "prob / num_relevant_edges"
// 	prob = prob / PROBLEM->num_relevant_edges();

// 	// Mutate each allele
// 	for( int i = 0; i < sol->encoding_length(); i++ ){

// 		// Only consier relevant edges
// 		if( PROBLEM->is_relevant( i ) ){

// 			if( random_real(0, 1) < prob ){

// 				// If this relevant MST edge is present, remove it (split is introduced)
// 				// Otherwise, re-introduce relevant edge
// 				if( (*sol)[ i ] == PROBLEM->mst_edge( i ) ){
					
// 					// Replace allele with a randomly selected alternative
// 					int allele;
// 					do{
// 						allele = sol->random_encoding( i );
// 					}while( allele == (*sol)[ i ] );
// 					(*sol)[ i ] = allele;


// 				}else{

// 					// Restore relevant MST edge
// 					(*sol)[ i ] = PROBLEM->mst_edge( i );

// 				}				

// 			}

// 		}

// 	}

// }
////////////////////////////////////////////////////////////////////////////////
// Neighbourhood-biased mutation (original, full-length locus encoding)
// Mutation occurs at each encoding position with a probability which depends on its current value
void UnaryOperator::neighbourhood_biased_mutation( SolutionPtr const sol, double prob ){

	// Given 'prob' defines the total number of alleles expected to mutate
	// Compute mutation probability as "prob / encoding_length"
	prob = prob / sol->encoding_length();

	// Mutate each allele
	for( int i = 0; i < sol->encoding_length(); i++ ){

		// Adjust mutation probability based on ranking of current link
		double rank = PROBLEM->neighbour_rank( i, (*sol)[ i ] );
		double allele_prob = prob + pow( rank / sol->encoding_length(), 2 );

		if( random_real(0, 1) < allele_prob ){

			// Replace allele with a randomly selected alternative
			(*sol)[ i ] = sol->random_encoding( i, (*sol)[ i ] );

		}

	}

}
////////////////////////////////////////////////////////////////////////////////
// Neighbourhood-biased mutation (short locus encoding)
// Mutation occurs at each encoding position with a probability which depends on its current value
void UnaryOperator::neighbourhood_biased_mutation_short( SolutionPtr const sol, double prob ){

	// Given 'prob' defines the total number of alleles expected to mutate
	// Compute mutation probability as "prob / encoding_length"
	prob = prob / sol->encoding_length();

	// Mutate each allele
	for( int i = 0; i < sol->encoding_length(); i++ ){

		// Adjust mutation probability based on ranking of current link
		double rank = PROBLEM->neighbour_rank( PROBLEM->relevant_edge(i), (*sol)[ i ] );
		double allele_prob = prob + pow( rank / sol->encoding_length(), 2 );

		if( random_real(0, 1) < allele_prob ){

			// Replace allele with a randomly selected alternative
			(*sol)[ i ] = sol->random_encoding( i, (*sol)[ i ] );

		}

	}

}
////////////////////////////////////////////////////////////////////////////////
// Neighbourhood-biased mutation (short binary encoding)
// Mutation occurs at each encoding position with a probability which depends on its current value
void UnaryOperator::neighbourhood_biased_mutation_split( SolutionPtr const sol, double prob ){

	// Given 'prob' defines the total number of alleles expected to mutate
	// Compute mutation probability as "prob / encoding_length"
	prob = prob / sol->encoding_length();

	// Mutate each allele
	for( int i = 0; i < sol->encoding_length(); i++ ){

		double allele_prob = prob;

		// Adjust mutation probability based on ranking of current link
		if( (*sol)[ i ] == 1 ){
			int edge = PROBLEM->relevant_edge(i);
			double rank = PROBLEM->neighbour_rank( edge, PROBLEM->mst_edge(edge) );
			allele_prob += pow( rank / sol->encoding_length(), 2 );
		}

		if( random_real(0, 1) < allele_prob ){

			// Replace allele with a randomly selected alternative
			(*sol)[ i ] = sol->random_encoding( i, (*sol)[ i ] );

		}

	}

}
////////////////////////////////////////////////////////////////////////////////


