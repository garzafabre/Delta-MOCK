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

#include "mock_Clustering.hh"

////////////////////////////////////////////////////////////////////////////////
// Constructor - Receives a full-length locus-based encoding
// Computes cluster assignment, determines number of clusters and creates clustering object
Clustering::Clustering( VectorIntPtr locus_encoding ){

	// Allocate memory and initialise
	_cluster_assignment = allocate_VectorInt( PROBLEM->ndata() );
	for( int i=0; i<PROBLEM->ndata(); i++ ){ _cluster_assignment[ i ] = -1; }
	_total_clusters = 0; // Total number of clusters found	
	int previous[ PROBLEM->ndata() ];

	// Get assinment of each data element to a cluster
	// Each connected component of the graph (as encoded in the adjacency-base encoding)
	// represents a different cluster
	for( int i=0; i<PROBLEM->ndata(); i++ ){

		int ctr = 0; 

		// Unassigned element found, assign cluster
		if( _cluster_assignment[ i ] == -1 ){

			// Assign to cluster, keep track of it, 
			// and identify neighbour (adjacent element)
			_cluster_assignment[ i ] = _total_clusters;
			previous[ ctr++ ] = i;
			int neighbour = locus_encoding[ i ]; 

			// Repeat operation for consecutive neighboring elements
			// and assign to the same cluster
			while( _cluster_assignment[ neighbour ] == -1 ){

				_cluster_assignment[ neighbour ] = _total_clusters;
				previous[ ctr++ ] = neighbour;
				neighbour = locus_encoding[ neighbour ]; 

			}      

			// If a previously assigned neighbour is reached,
			// and this element was assigned to a different cluster X, 
			// re-assign all elements in 'previous' list to cluster X
			if( _cluster_assignment[ neighbour ] != _total_clusters ){

				while( --ctr >= 0 ){

					_cluster_assignment[ previous[ ctr ] ] = _cluster_assignment[ neighbour ]; 
					
				}

			}else{

				// Increase cluster counter
				_total_clusters++;

			}

		}		

	}

	// Centroid computation
	compute_cluster_centres();

}
////////////////////////////////////////////////////////////////////////////////
// Constructor - Receives a cluster assignment, and corresponding number of clusters
// if create_copy == true, creates a new vector and copies the given assignment (default)
// otherwise, the given pointer to the assignment vector is used
Clustering::Clustering( VectorIntPtr assignment, const int clusters, bool create_copy ) :  

	_total_clusters( clusters ) 
	
{

	// Create copy or re-use memory?
	if( create_copy ){

		// Allocate memory
		_cluster_assignment = allocate_VectorInt( PROBLEM->ndata() );

		// Copy assignment
		for( int i=0; i<PROBLEM->ndata(); i++ ){
			_cluster_assignment[ i ] = assignment[ i ];
		}

	}else{

    	_cluster_assignment = assignment; 

    }

	// Centroid computation
	compute_cluster_centres();

}
////////////////////////////////////////////////////////////////////////////////
// Destructor
Clustering::~Clustering(){

	deallocate_VectorInt( _cluster_assignment );
	deallocate_MatrixFloat( _centre, _total_clusters );
	deallocate_VectorInt( _member_ctr );

}
////////////////////////////////////////////////////////////////////////////////
// Compute the centroid of each of the clusters
void Clustering::compute_cluster_centres(){	

	// Allocate memory and initialise
	_centre = allocate_MatrixFloat( _total_clusters, PROBLEM->mdim() );
	_member_ctr = allocate_VectorInt( _total_clusters );	
	for( int i=0; i<_total_clusters; i++){	

		for( int j=0; j<PROBLEM->mdim(); j++) _centre[ i ][ j ] = 0.0;
		_member_ctr[ i ] = 0;

	}

    // Average data elements of each cluster
    for( int i=0; i<PROBLEM->ndata(); i++ ){

    	sum_VectorFloat( _centre[ _cluster_assignment[ i ] ], (*PROBLEM)[ i ], _centre[ _cluster_assignment[ i ] ], PROBLEM->mdim() );
    	
    	_member_ctr[ _cluster_assignment[ i ] ]++;

    }

    for( int i=0; i<_total_clusters; i++){

    	if( _member_ctr[ i ] > 0 ){

    		divide_VectorFloat_by( _centre[ i ], float(_member_ctr[ i ]), _centre[ i ], PROBLEM->mdim() );

    	}

    }    

}
////////////////////////////////////////////////////////////////////////////////
// Compute the centroid of each of the clusters
void Clustering::update_cluster_centres(){	
	
	for( int i=0; i<_total_clusters; i++){	

		for( int j=0; j<PROBLEM->mdim(); j++) _centre[ i ][ j ] = 0.0;

	}

    // Average data elements of each cluster
    for( int i=0; i<PROBLEM->ndata(); i++ ){

    	sum_VectorFloat( _centre[ _cluster_assignment[ i ] ], (*PROBLEM)[ i ], _centre[ _cluster_assignment[ i ] ], PROBLEM->mdim() );

    }

    for( int i=0; i<_total_clusters; i++){

    	if( _member_ctr[ i ] > 0 ){

    		divide_VectorFloat_by( _centre[ i ], float(_member_ctr[ i ]), _centre[ i ], PROBLEM->mdim() );

    	}

    } 

}
////////////////////////////////////////////////////////////////////////////////
// Changes the assignment of a given data element, to a given cluster
void Clustering::update_assignment( int element, int cluster ){

	// Decrease member counter of original cluster	
	_member_ctr[ _cluster_assignment[ element ] ]--;

	// Update assignment
	_cluster_assignment[ element ] = cluster;

	// Increase member counter of new cluster
	_member_ctr[ cluster ]++;

}
////////////////////////////////////////////////////////////////////////////////
