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

#ifndef __MOCK_CLUSTERINGPROBLEM_HH__
#define __MOCK_CLUSTERINGPROBLEM_HH__

/******************
Dependencies
******************/
#include "mock_ClusteringProblem.fwd.hh"
#include "mock_Global.hh"

/******************
Class definition
******************/
class ClusteringProblem{

	/******************
	Attributes
	******************/

	private:

		MatrixFloatPtr _data;				// Collection of data elements (items/points)	

		string _filename;					// Data filename (input parameter)

		int _ndata;							// Number of data elements

		int _mdim;							// Number of dimensions (features/variables)

		bool _labels_provided;				// Flag to indicate if original labels were provided

		VectorIntPtr _label;				// Real cluster label of each data element (if labels provided)

		int _num_real_clusters;				// Number of real clusters (if labels provided)

		bool _normalise;					// Normalise data? (input parameter)

		string _distance;					// Distance measure to use

		// ----------------------
		// Pre-computed information
		// ----------------------

		MatrixFloatPtr _distance_matrix;	// Pre-computed distance matrix (pairwise distances between data elements)
		
		MatrixIntPtr _nearest_neighbours;	// Pre-computed nearest neighbour matrix

		MatrixIntPtr _neighbour_rank;		// Pre-computed neighbour rank (position in nearest neighbour list) matrix

		VectorDoublePtr _priority_edges;	// Sorted list of tuples (idx, interesingness) of interesting edges

		int _num_priority_edges;			// Total number of interesting edges found

		VectorIntPtr _relevant_edges;		// For improved mutation
		int _num_relevant_edges;			// For improved mutation
		VectorIntPtr _mst;					// Alternative representation of _MST
		double _delta;

		VectorIntPtr _fixed_edges;			// For improved mutation
		int _num_fixed_edges;				// For improved mutation
		bool *_is_fixed;					// For improved mutation
		VectorIntPtr _relevant_index;

	/******************
	Methods
	******************/

	private:

		// Set-up
		void initialise();
		void configure();

		// Data loading
		void load_data();

		// Pre-computations
		void compute_distance_matrix();
		void compute_nearest_neighbours();
		void compute_mst();

	public:

		// Constructor / destructor
		ClusteringProblem();
		~ClusteringProblem();

		// Basic accesors
		int ndata();
		int mdim();
		bool labels_provided();
		int num_real_clusters();
		int label( int i );

		// Accesor to distance matrix
		float distance( const int i, const int j );

		// Accessor to data elements
		VectorFloatPtr operator[]( int i );

		// Accesor to nearest neighbours information
		int neighbour( const int i, const int j );
		int neighbour_rank( const int i, const int j );

		// Pointer to distance function
		float (*distance_measure)( VectorFloatPtr v1, VectorFloatPtr v2, int size );

		// MST information
		int mst_edge( int  i );
		int priority_edge( int i );	
		int num_priority_edges();

		void add_relevant_edge( int i );
		int relevant_edge( int i );	
		int num_relevant_edges();
		void determine_relevant_edges();

		int fixed_edge( int i );
		int num_fixed_edges();
		bool is_fixed( int i );

		int relevant_index( int i );

};

/******************
Inline methods
******************/

////////////////////////////////////////////////////////////////////////////////
// Returns total number of data elements
inline int ClusteringProblem::ndata(){ 

	return _ndata; 

}
////////////////////////////////////////////////////////////////////////////////
// Returns number of dimensions (variables) defining each data element
inline int ClusteringProblem::mdim(){ 

	return _mdim; 

}
////////////////////////////////////////////////////////////////////////////////
// Where real labels provided?
inline bool ClusteringProblem::labels_provided(){ 

	return _labels_provided; 

}
////////////////////////////////////////////////////////////////////////////////
// Returns number of real clusters
inline int ClusteringProblem::num_real_clusters(){ 

	return _num_real_clusters; 

}
////////////////////////////////////////////////////////////////////////////////
// Returns real label of i-th element
inline int ClusteringProblem::label( int i ){ 

	return _label[ i ]; 

}
////////////////////////////////////////////////////////////////////////////////
// Read-only access to distance/dissimilarity matrix
inline float ClusteringProblem::distance( const int i, const int j ){        

    return ( i >= j ) ? _distance_matrix[ i ][ j ] : _distance_matrix[ j ][ i ]; 

}
////////////////////////////////////////////////////////////////////////////////
// Direct access to individual data items
inline VectorFloatPtr ClusteringProblem::operator[]( int i ) {

    return _data[ i ];

}
////////////////////////////////////////////////////////////////////////////////
// Direct access to nearest neighbour lists
inline int ClusteringProblem::neighbour( const int i, const int j ){ 

	return _nearest_neighbours[ i ][ j ]; 

}
////////////////////////////////////////////////////////////////////////////////
// Direct access to neighbour rank matrix
inline int ClusteringProblem::neighbour_rank( const int i, const int j ){ 

	return _neighbour_rank[ i ][ j ]; 

}
////////////////////////////////////////////////////////////////////////////////
// Returns node to which the i-th element is linked in the MST 
inline int ClusteringProblem::mst_edge( int  i ){

	return _mst[ i ];

}
////////////////////////////////////////////////////////////////////////////////
// Returns pointer to the i-th most interesting edge of the MST
inline int ClusteringProblem::priority_edge( int i ){

	return int( _priority_edges[ i * 2 ] );

}
////////////////////////////////////////////////////////////////////////////////
// Return total number of interesting edges
inline int ClusteringProblem::num_priority_edges(){

	return _num_priority_edges;

}
////////////////////////////////////////////////////////////////////////////////
// Returns the i-th identified relevant edge
inline int ClusteringProblem::relevant_edge( int i ){

	return _relevant_edges[ i ];

}	
////////////////////////////////////////////////////////////////////////////////
// How many relevant edges?
inline int ClusteringProblem::num_relevant_edges(){

	return _num_relevant_edges;

}
////////////////////////////////////////////////////////////////////////////////
// Returns the i-th identified fixed edge
inline int ClusteringProblem::fixed_edge( int i ){

	return _fixed_edges[ i ];

}
////////////////////////////////////////////////////////////////////////////////
// How many fixed edges?
inline int ClusteringProblem::num_fixed_edges(){

	return _num_fixed_edges;

}
////////////////////////////////////////////////////////////////////////////////
// Is the i-th MST edge fixed?
inline bool ClusteringProblem::is_fixed( int i ){

	return _is_fixed[ i ];

}
////////////////////////////////////////////////////////////////////////////////
// Given the i-th mst edge (normal encoding position), which position it has in the relevant list?
inline int ClusteringProblem::relevant_index( int i ){

	return _relevant_index[ i ];

}
////////////////////////////////////////////////////////////////////////////////

#endif
