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

#include "mock_SolutionShort.hh"
#include "mock_Global.hh"
#include "mock_Clustering.hh"

////////////////////////////////////////////////////////////////////////////////
// Constructor - a solution with Locus-based encoding is given
SolutionShort::SolutionShort( SolutionLocus const & locus ) :
	
	SolutionCommon( false ) // Allocates memory, but does not initialise

{

	// For each relevant encoding position, 
	// get its value from the original full-length solution
	for( int i = 0; i < encoding_length(); i++ ){

		_encoding[ i ] = locus[ PROBLEM->relevant_edge( i ) ];

	}

}
////////////////////////////////////////////////////////////////////////////////
// Decodes the adjacency-based encoding to a clustering object
ClusteringPtr SolutionShort::decode_clustering(){
	
	// Re-construct full encoding from MST 
	VectorIntPtr full_encoding = allocate_VectorInt( PROBLEM->ndata() );
	for( int i = 0; i < PROBLEM->ndata(); i++ ){ 

		full_encoding[ i ] = PROBLEM->mst_edge( i );
		
	}	

	// Update full encoding based on current reduced-encoding solution
	update_full_encoding( full_encoding );

	// Create clustering object based on full-length encoding
	ClusteringPtr clustering = ClusteringPtr( new Clustering( full_encoding ) );

	// Free memory
	deallocate_VectorInt( full_encoding );

	// Return clustering object
	return clustering;

}
////////////////////////////////////////////////////////////////////////////////
// Reconstruct full encoding based on current reduced-encoding solution
// Assumes all fixed-positions in the given encoding vector were previously set
void SolutionShort::update_full_encoding( VectorIntPtr full_encoding ){

	// Update full encoding based on current reduced-encoding solution
	for( int i = 0; i < encoding_length(); i++ ){

		full_encoding[ PROBLEM->relevant_edge( i ) ] = _encoding[ i ];

	}

}
////////////////////////////////////////////////////////////////////////////////


