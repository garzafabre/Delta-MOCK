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

#ifndef __MOCK_CLUSTERING_HH__
#define __MOCK_CLUSTERING_HH__

/******************
Dependencies
******************/
#include "mock_Clustering.fwd.hh"
#include "mock_Global.hh"

/******************
Class definition
******************/
class Clustering{
    
	/******************
	Attributes
	******************/

	private:

		VectorIntPtr _cluster_assignment;	// Assignment of each data element to a cluster

		int _total_clusters;				// Total number of clusters

		MatrixFloatPtr _centre;				// Matrix of cluster centres (each row denotes the centre of a cluster)

		VectorIntPtr _member_ctr;			// Counters of cluster members 

	public:

	/******************
	Methods
	******************/

	private:

		void compute_cluster_centres();

	public:

		// Constructor / destructor
		Clustering( VectorIntPtr assignment, const int clusters, bool create_copy = true );
		Clustering( VectorIntPtr locus_encoding );
		~Clustering();

		// Accesors
		int assignment( const int i ){ return _cluster_assignment[ i ]; }
		int total_clusters(){ return _total_clusters; }
		VectorFloatPtr centre( const int i ){ return _centre[ i ]; }
		int member_ctr( const int i ){ return _member_ctr[ i ]; }

		void update_assignment( int element, int cluster );
		void update_cluster_centres();

};

#endif

