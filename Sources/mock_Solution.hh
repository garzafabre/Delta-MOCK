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

#ifndef __MOCK_SOLUTION_HH__
#define __MOCK_SOLUTION_HH__

/******************
Dependencies
******************/
#include "mock_Solution.fwd.hh"
#include "mock_Global.hh"
#include "mock_Clustering.fwd.hh"

/******************
Class definition
******************/

// Solution interface
class Solution{

	/******************
	Attributes
	******************/

	protected:

		VectorIntPtr _encoding;			// Genotype

		VectorDoublePtr _objective;		// Objective value(s)

		int _kclusters;					// Number of clusters in the encoded solution

		unsigned long int _evaluation;	// Evaluation number at which this solution was found

		int _rank;						// Ranking of the solution with respect to the entire population

		double _crowding_distance;		// Crowding distance measure used by NSGA-II

	/******************
	Methods
	******************/

	public:

		// Constructor / destructor
		Solution() : 
			_encoding( nullptr ),
			_objective( nullptr ),
			_kclusters( 0 ), 
			_evaluation( 0 ),
			_rank( 0 ),
			_crowding_distance( 0 )
		{ /* do nothing */ }
		virtual ~Solution(){ /* do nothing */ }

		// Read/write accesors to solution atributes
    	virtual int & operator[]( const int pos ) const = 0;
		virtual int & encoding( const int pos ) const = 0;
		virtual double & objective( const int obj ) = 0;
		virtual VectorDoublePtr objective() = 0;
		virtual int & kclusters() = 0;
		virtual unsigned long int & evaluation() = 0;
		virtual int & rank() = 0;
		virtual double & crowding_distance() = 0;

		// Encoding-specific functions (must be implemented in derived classes)
		virtual int encoding_length() const = 0; 
		virtual int random_encoding( int i, int to_avoid = -1 ) const = 0; 
		virtual ClusteringPtr decode_clustering() = 0;
		virtual void update_full_encoding( VectorIntPtr full_encoding ) = 0;

}; 

////////////////////////////////////////////////////////////////////////////////

/******************
Class definition
******************/

// Implementation of common methods (some of them derived-class dependent)
// Curiously Recurring Template Pattern (CRTP)
template < typename SolutionType >
class SolutionCommon : public Solution {

	/******************
	Attributes
	******************/

		// All attributes defined above in base class 

	/******************
	Methods
	******************/

	public:

		// Constructor / destructor
		SolutionCommon( bool initialise = true );
		SolutionCommon( VectorIntPtr enc, bool create_new = true );
		virtual ~SolutionCommon();

		// Read/write accesors to solution atributes
    	int & operator[]( const int pos ) const;
		int & encoding( const int pos ) const;
		double & objective( const int obj );
		VectorDoublePtr objective();		
		int & kclusters();
		unsigned long int & evaluation();
		int & rank();
		double & crowding_distance();

		// Encoding-specific functions
		int encoding_length() const; 
		int random_encoding( int i, int to_avoid = -1 ) const;
};

////////////////////////////////////////////////////////////////////////////////
// Constructor 1: 
// No initial encoding is provided; a new initialised one is created
// This is a generic procedure which depends on the specific SolutionType used
template < typename SolutionType >
inline SolutionCommon< SolutionType >::SolutionCommon( bool initialise ){

	_encoding = allocate_VectorInt( encoding_length() );
	_objective = allocate_VectorDouble( num_objectives );

	// Initialise: randomly choose a valid value for each encoding position
	if( initialise ){

		for( int i = 0; i < encoding_length(); i++ ){ 

			_encoding[ i ] = random_encoding( i ); 

		}	

	}

}
////////////////////////////////////////////////////////////////////////////////
// Constructor 2
// A pointer to an existing encoding is given:
// 	if create_new == true, a new copy of this encoding is created (default)
//	otherwise, the given pointer is assigned (memory of the given existing encoding is used)
// This is a generic procedure which depends on the specific SolutionType used
template < typename SolutionType >
inline SolutionCommon< SolutionType >::SolutionCommon( VectorIntPtr enc, bool create_new ){

	if( create_new ){ 

		// Allocate memory
		_encoding = allocate_VectorInt( encoding_length() );
		_objective = allocate_VectorDouble( num_objectives );

		// Initialise encoding based on the given one
		for( int i = 0; i < encoding_length(); i++ ){ 

			_encoding[ i ] = enc[ i ]; 

		}

	}else{

			// Allocate memory	
		_objective = allocate_VectorDouble( num_objectives );

		// Assign given pointer
		_encoding = enc;

	}

}
////////////////////////////////////////////////////////////////////////////////
// Destructor
template < typename SolutionType >
inline SolutionCommon< SolutionType >::~SolutionCommon(){

	// Deallocate memory
	deallocate_VectorInt( _encoding );
	deallocate_VectorDouble( _objective );
	
}
////////////////////////////////////////////////////////////////////////////////
// Direct access to solution's encoding (same as encoding method)
template < typename SolutionType >
inline int & SolutionCommon< SolutionType >::operator[]( const int pos ) const {

    return _encoding[ pos ];
    
}
////////////////////////////////////////////////////////////////////////////////
// Direct access to solution's encoding (same as [] operator)
template < typename SolutionType >
inline int & SolutionCommon< SolutionType >::encoding( const int pos ) const {

	return _encoding[ pos ];

}
////////////////////////////////////////////////////////////////////////////////
// Direct access to individual objectives
template < typename SolutionType >
inline double & SolutionCommon< SolutionType >::objective( const int obj ){

	return _objective[ obj ];

}
////////////////////////////////////////////////////////////////////////////////
// Direct access to objective vector
template < typename SolutionType >
inline VectorDoublePtr SolutionCommon< SolutionType >::objective(){

	return _objective;

}
////////////////////////////////////////////////////////////////////////////////
// Direct access to _kclusters attribute
template < typename SolutionType >
inline int & SolutionCommon< SolutionType >::kclusters(){

	return _kclusters;

}
////////////////////////////////////////////////////////////////////////////////
// Direct access to _evaluation attribute
template < typename SolutionType >
inline unsigned long int & SolutionCommon< SolutionType >::evaluation(){

	return _evaluation;

}
////////////////////////////////////////////////////////////////////////////////
// Direct access to rank attribute
template < typename SolutionType >
inline int & SolutionCommon< SolutionType >::rank(){

	return _rank;

}
////////////////////////////////////////////////////////////////////////////////
// Direct access to cwd attribute
template < typename SolutionType >
inline double & SolutionCommon< SolutionType >::crowding_distance(){

	return _crowding_distance;

}
////////////////////////////////////////////////////////////////////////////////
// Gets encoding length, based on specific encoding
template < typename SolutionType >
inline int SolutionCommon< SolutionType >::encoding_length() const {

	return SolutionType::static_encoding_length();

}
////////////////////////////////////////////////////////////////////////////////
// Gets random encoding value, based on specific encoding
template < typename SolutionType >
inline int SolutionCommon< SolutionType >::random_encoding( int i, int to_avoid ) const {

	return SolutionType::static_random_encoding( i, to_avoid );

}
////////////////////////////////////////////////////////////////////////////////

#endif

