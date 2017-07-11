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

#include "mock_SolutionLocus.hh"
#include "mock_Global.hh"
#include "mock_Clustering.hh"

////////////////////////////////////////////////////////////////////////////////
// Decodes the adjacency-based encoding to a clustering object
ClusteringPtr SolutionLocus::decode_clustering(){	

	// Create clustering object based on solution's encoding
	ClusteringPtr clustering = ClusteringPtr( new Clustering( _encoding ) );
	
	// Return clustering object
	return clustering;

}
////////////////////////////////////////////////////////////////////////////////
// This method is not required for this particular encoding, 
// but must be implemented for the reduced-size encoding
// in order to reconstruct the full encoding  
void SolutionLocus::update_full_encoding( VectorIntPtr full_encoding ){

	// Simply copy the current solution's encoding
	for( int i = 0; i < encoding_length(); i++ ){

		full_encoding[ i ] = _encoding[ i ];

	}

}
////////////////////////////////////////////////////////////////////////////////


