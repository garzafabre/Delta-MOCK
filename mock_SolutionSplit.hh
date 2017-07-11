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

#ifndef __MOCK_SOLUTIONSPLIT_HH__
#define __MOCK_SOLUTIONSPLIT_HH__

/******************
Dependencies
******************/
#include "mock_Solution.hh"
#include "mock_SolutionLocus.hh"

/******************
Class definition
******************/
Define_SolutionType( SolutionSplit ){

	/******************
	Attributes
	******************/

		// All attributes defined in base class (Solution.hh)

	/******************
	Methods
	******************/

	public:

		// Use constructors from base class
		// NOTE: This inherits all constructors from base class, 
		// but gives problems if we define additional constructors here
		// using SolutionCommon::SolutionCommon; 

		// Alternatively, forwarding constructors
		// SolutionSplit() : SolutionCommon( true ) {}
		SolutionSplit( bool initialise = true ) : SolutionCommon( initialise ) {}
		SolutionSplit( VectorIntPtr enc, bool create_new = true ) : SolutionCommon( enc, create_new ) {}

		// Additional constructors
		SolutionSplit( SolutionLocus const & locus );

		// Encoding-specific functions
		static int static_encoding_length();
		static int static_random_encoding( int i, int to_avoid = -1 );			
		ClusteringPtr decode_clustering();
		void update_full_encoding( VectorIntPtr full_encoding );

};

////////////////////////////////////////////////////////////////////////////////
// Returns encoding length
// In the locus-based/adjacency representation, 
// there are as many encoding positions as data points
// This reduced-size representation consist only of the relevant
// encoding positions of the original representation
inline int SolutionSplit::static_encoding_length(){

	return PROBLEM->num_relevant_edges();

}
////////////////////////////////////////////////////////////////////////////////
// Returns a random valid value for the given encoding position
inline int SolutionSplit::static_random_encoding( int i, int to_avoid ){

	// Valid value is either 0 or 1, to indicate whether
	// the corresponding relevant MST edge is absent/present in the partitioning
	return ( to_avoid == -1 ) ? random_int( 0, 1 ) : (( to_avoid == 0 ) ? 1 : 0);

}		
////////////////////////////////////////////////////////////////////////////////

#endif 

