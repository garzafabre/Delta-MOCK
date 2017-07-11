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

#ifndef __MOCK_EVALUATORDELTA_HH__
#define __MOCK_EVALUATORDELTA_HH__

/******************
Dependencies
******************/
#include "mock_EvaluatorDelta.fwd.hh"
#include "mock_Evaluator.hh"
#include <queue>

/******************
Class definition
******************/
class EvaluatorDelta : public Evaluator {
    
	/******************
	Attributes
	******************/

	private:

		/**************************
		The following structures are used to store the results of decoding/evaluation
		This memory is to be reused for all evaluations to remove the computational
		costs involved with memory allocation/deallocation
		***************************/

		ClusterAssignmentDeltaPtr _clusters;
		VectorIntPtr _full_encoding;
		bool * _processed;

	/******************
	Methods
	******************/

	public:

		// Constructor / destructor
		EvaluatorDelta();
		~EvaluatorDelta();

		// Accesor to evaluations counter
		unsigned long int total_evaluations() const { return _total_evaluations; }

		// Evaluation of solution(s)
		void evaluate( SolutionPtr solution );
		void evaluate( PopulationPtr population );

		void delta_decode_evaluate( VectorIntPtr encoding );

};
////////////////////////////////////////////////////////////////////////////////

/******************
Class definition
******************/
class ClusterAssignment{

	friend class ClusterAssignmentDelta;

	/******************
	Attributes
	******************/

	private:
		
		// Used for cluster assignment
		VectorIntPtr _assignment; 				// Cluster assignmnt of each data point
		int _total_clusters; 		  			// Total number of clusters - 1
		MatrixIntPtr _clusters;					// Matrix of clusters
    	std::queue<int> _removed;				// Auxiliar structure to store removed clusters (merge operation)

    	// Used for the Variance measure    	
		VectorDoublePtr _variance;				// Individual cluster variances
		MatrixFloatPtr _centre;					// Cluster centres

    	// Used for the Connectivity measure
    	double _connectivity;
    	VectorDoublePtr _cnn_penalty;			// Pre-computed penalties for the connectivity measure
		const int _knn;							// Number of nearest neighbours to use
		MatrixDoublePtr _cnn_contribution;		// Contribution of each pair of cluster to Connectivity measure
		MatrixIntPtr _cnn_pair;					// List of unique cluster pairs contributing to Connectivity
		unsigned long int _cnn_pairs;			// Number of unique cluster pairs contributing to Connectivity

	/******************
	Methods
	******************/

	public:

		// Constructor
		ClusterAssignment() : _total_clusters( -1 ), _knn( mock_L ) {

    		// Memory allocation
			_assignment = allocate_VectorInt( PROBLEM->ndata() );
		    _clusters = allocate_MatrixInt( PROBLEM->ndata(), PROBLEM->ndata()+1 );
		    _variance = allocate_VectorDouble( PROBLEM->ndata() );
		    _centre = allocate_MatrixFloat( PROBLEM->ndata(), PROBLEM->mdim() );

			// Initialise
			for( int i=0; i<PROBLEM->ndata(); i++ )	_assignment[ i ] = -1; 

		}

		// Destructor
		~ClusterAssignment(){

    		// Deallocate memory
			deallocate_VectorInt( _assignment );
		    deallocate_MatrixInt( _clusters, PROBLEM->ndata() );
		    deallocate_VectorDouble( _variance );
		    deallocate_MatrixFloat( _centre, PROBLEM->ndata() );
		    deallocate_VectorDouble( _cnn_penalty );
		    deallocate_MatrixDouble( _cnn_contribution, _total_clusters+1 );
		    deallocate_MatrixInt( _cnn_pair, _cnn_pairs );

		}

		// New cluster creation
		int create_cluster( int element ){

			int c; 

			if( _removed.empty() ){

                // Increase number of clusters and update assignment
				_total_clusters++;				

				// Initialise centre and variance of this cluster
				_variance[ _total_clusters ] = 0.0;
				for( int j = 0; j < PROBLEM->mdim(); j++ ) _centre[ _total_clusters ][ j ] = 0.0;	

				c = _total_clusters; 			

            }else{

                // Reuse previously created clusters
                c = _removed.front();
                _removed.pop();

            }

            // Insert given element
            _clusters[ c ][ 0 ] = 1;
            _clusters[ c ][ 1 ] = element;
            _assignment[ element ] = c;

            // Return cluster id (index)
	        return c;			

		}

		// Insert given element into cluster
		void insert( int cluster, int element ){

			_clusters[ cluster ][ ++_clusters[ cluster ][ 0 ] ] = element;
			_assignment[ element ] = cluster;

		}		
		
		// Merge two given clusters c1 and c2
		void merge( int c1, int c2 ){			 

			// Empty c2 and move all elements to c1
			for( int j=1; j<=_clusters[ c2 ][ 0 ]; j++ ){

				_clusters[ c1 ][ ++_clusters[ c1 ][ 0 ] ] = _clusters[ c2 ][ j ];
				_assignment[ _clusters[ c2 ][ j ] ] = c1;

			}
			_clusters[ c2 ][ 0 ] = 0;
			_removed.push( c2 );

		}		

		// PRECOMPUTATION of Variance measure
		// Compute variance of all individual clusters
		void precompute_variance(){

			// Compute centres
			for( int i=0; i<PROBLEM->ndata(); i++ )
				sum_VectorFloat( _centre[ _assignment[ i ] ], (*PROBLEM)[ i ], _centre[ _assignment[ i ] ], PROBLEM->mdim() );

			for( int i=0; i<=_total_clusters; i++)
		    	if( _clusters[ i ][ 0 ] > 1 )
		    		divide_VectorFloat_by( _centre[ i ], float(_clusters[ i ][ 0 ]), _centre[ i ], PROBLEM->mdim() );

		    // Compute variances
		    for( int i = 0; i < PROBLEM->ndata(); i++ ){

		        double diff = PROBLEM->distance_measure( (*PROBLEM)[ i ],  _centre[ _assignment[ i ] ], PROBLEM->mdim() );
		        _variance[ _assignment[ i ] ] += ( diff * diff );

		    }		

		}

		// PRECOMPUTATION of Connectivity measure
		void precompute_connectivity(){

			// Pre-compute penalty values used by the connectivity measure
		    // Penalty values are defined according to positions in NN list
		    _cnn_penalty = allocate_VectorDouble( _knn );
		    for( int j=0; j<_knn; j++ ) _cnn_penalty[ j ] = 1.0/(double(j)+1.0);

		    // Prepare structures for computing pairwise connectivity contributions
			_cnn_pairs = 0;	
			_cnn_pair = MatrixIntPtr( new VectorIntPtr [ int((_total_clusters+1.0)*(_total_clusters)/2.0) ] );				
			_cnn_contribution = allocate_MatrixDouble( _total_clusters+1, _total_clusters+1 );	
			for( int i=0; i<=_total_clusters; i++){
				for( int j=0; j<=_total_clusters; j++){
					_cnn_contribution[ i ][ j ] = 0.0;
				}
			}

			// Compute connectivity measure
		    _connectivity = 0.0;
		    for( int i = 0; i < PROBLEM->ndata(); i++ ){
		        
		        // Label (cluster id) of i-th solution
		        int label = _assignment[ i ];

		        // Verify labels of nearest neighbours
		        for( int j = 0; j < _knn; j++ ){

		            int nn_label = _assignment[ PROBLEM->neighbour( i, j ) ];

		            if (label != nn_label) {

		            	// Update pre-computed Connectivity measure
		                _connectivity += _cnn_penalty[ j ];

		                // New identified cluster pair contributing to Connectivity
		                if( _cnn_contribution[ label ][ nn_label ] < 0.1 ){
		                	_cnn_pair[ _cnn_pairs ] = allocate_VectorInt( 2 );
		                	_cnn_pair[ _cnn_pairs ][ 0 ] = label;
		                	_cnn_pair[ _cnn_pairs ][ 1 ] = nn_label;
		                	_cnn_pairs++;
		                }

		                // Update Connectivity contributions for this particular pair of clusters
		                _cnn_contribution[ label ][ nn_label ] += _cnn_penalty[ j ];
		                _cnn_contribution[ nn_label ][ label ] = _cnn_contribution[ label ][ nn_label ];

		            }
		            
		        }

		    }  

		}

};
////////////////////////////////////////////////////////////////////////////////

/******************
Class definition
******************/
class ClusterAssignmentDelta{

	/******************
	Attributes
	******************/

	private:

    	ClusterAssignmentPtr _precomputed;		// Pre-computed cluster assignment and performance measures
    	MatrixIntPtr _cluster_members;			// List of pre-computed clusters that are members of a final cluster
		VectorIntPtr _cluster_membership;		// Membership (parent cluster) of each pre-computed cluster
		VectorIntPtr _cluster_size;				// Final cluster sizes
		MatrixFloatPtr _centre;					// Final cluster centres
    	int _total_clusters;					// Total number of resulting clusters

	/******************
	Methods
	******************/

	public:

		// Constructor
		ClusterAssignmentDelta() {

			// Precompute cluster assignment and evaluation measures
			precomputations();
    		
			// Memory allocation
			_cluster_members = allocate_MatrixInt( _precomputed->_total_clusters + 1, _precomputed->_total_clusters + 2 );
			_cluster_membership = allocate_VectorInt( _precomputed->_total_clusters+1 );
			_cluster_size = allocate_VectorInt( _precomputed->_total_clusters+1 );
			_centre = allocate_MatrixFloat( _precomputed->_total_clusters+1, PROBLEM->mdim() );

			// Initialise cluster members
			for( int i=0; i<=_precomputed->_total_clusters; i++) _cluster_members[ i ][ 1 ] = i;

		}

		// Destructor
		~ClusterAssignmentDelta(){

			// Deallocate memory
			deallocate_MatrixInt( _cluster_members, _precomputed->_total_clusters + 1 );
			deallocate_VectorInt( _cluster_membership );
			deallocate_VectorInt( _cluster_size ); 
			deallocate_MatrixFloat( _centre, _precomputed->_total_clusters+1 );
			delete _precomputed;

		}

		// Precomputation of cluster assignment and measures
		// based on fixed encoding positions
		void precomputations(){			
			
		    _precomputed = new ClusterAssignment();

		    for( int f=0; f<PROBLEM->num_fixed_edges(); f++ ){

		        int fixed = PROBLEM->fixed_edge( f );

		        // Unassigned element found?
		        if( _precomputed->_assignment[ fixed ] == -1 ){

		            // Create new cluster (connected component)
		            int c = _precomputed->create_cluster( fixed );            

		            // Assign consecutive non-assigned neighours to the same cluster
		            int neighbour = PROBLEM->mst_edge( fixed );            
		            while( _precomputed->_assignment[ neighbour ] == -1 ){

		                _precomputed->insert( c, neighbour );

		                // Avoid processing non-fixed edges
		                if( PROBLEM->is_fixed( neighbour ) ){

		                    neighbour = PROBLEM->mst_edge( neighbour ); 

		                }else{

		                    break;

		                }

		            }  

		            // If a previously assigned neighbour is reached, merge clusters
		            if( _precomputed->_assignment[ neighbour ] != c ){  

		                _precomputed->merge( _precomputed->_assignment[ neighbour ], c );

		            }

		        }

		    }

		    // At this point, all fixed edges were processed
		    // Is it possible, however, that some non-fixed edges remain unasigned since they were not
		    // directly connected by a fixed one. Create a cluster for each unasigned edge.
		    for( int r=0; r<PROBLEM->num_relevant_edges(); r++ ){

		        int relevant = PROBLEM->relevant_edge( r );

		        // Unassigned element found?
		        if( _precomputed->_assignment[ relevant ] == -1 ){

		            // Create new cluster (connected component)
		            _precomputed->create_cluster( relevant );  

		        }

		    }

		    // Compute variance of precomputed clusters
		    _precomputed->precompute_variance();
		    _precomputed->precompute_connectivity();

		}

		// Restore data structures 
		// (to be applied before each solution evaluation)
		void reset(){
			
			for( int i=0; i<=_precomputed->_total_clusters; i++ ){

				_cluster_members[ i ][ 0 ] = 1;
				_cluster_membership[ i ] = i;
				_cluster_size[ i ] = 0;
				for( int d=0; d<PROBLEM->mdim(); d++ ) _centre[ i ][ d ] = 0.0;

			}
			_total_clusters = _precomputed->_total_clusters + 1;

		}

		// Merge clusters of two given data elements c1 and c2
		void merge( int c1, int c2 ){		

			// Get cluser assignment 
			c1 = _cluster_membership[ _precomputed->_assignment[ c1 ] ];
			c2 = _cluster_membership[ _precomputed->_assignment[ c2 ] ];

			// Are we really merging different (non-previously merged) clusters?
			if( c1 != c2 ){
			
				// Try to remove always the smallest cluster		
				if( _cluster_members[ c1 ][ 0 ] < _cluster_members[ c2 ][ 0 ] ){

					// Remove cluster c1, move all cluster members to c2
					for( int i=1; i<=_cluster_members[ c1 ][ 0 ]; i++ ){
						_cluster_members[ c2 ][ ++_cluster_members[ c2 ][ 0 ] ] = _cluster_members[ c1 ][ i ];
						_cluster_membership[ _cluster_members[ c1 ][ i ] ] = c2;
					}
					_cluster_members[ c1 ][ 0 ] = 0;

				}else{

					// Remove cluster c2, move all cluster members to c1
					for( int i=1; i<=_cluster_members[ c2 ][ 0 ]; i++ ){
						_cluster_members[ c1 ][ ++_cluster_members[ c1 ][ 0 ] ] = _cluster_members[ c2 ][ i ];
						_cluster_membership[ _cluster_members[ c2 ][ i ] ] = c1;
					}
					_cluster_members[ c2 ][ 0 ] = 0;

				}	

				// Update number of clusters
				_total_clusters--;			

			}			

		}				

		// Compute Variance measure using partial pre-computed data
		double total_variance(){	

			double variance = 0.0;		

			// Determine size of resulting clusters and accoumulate variance and centroids
			for( int c=0; c<=_precomputed->_total_clusters; c++ ){

				int m = _cluster_membership[ c ];

				// Update size
				_cluster_size[ m ] += _precomputed->_clusters[ c ][ 0 ];

				// Update sum of variances
				variance += _precomputed->_variance[ c ];

				// Update centroid
				for( int d=0; d<PROBLEM->mdim(); d++ ){ 

					_centre[ m ][ d ] += ( _precomputed->_clusters[ c ][ 0 ] * _precomputed->_centre[ c ][ d ] );

				}

			}

			// Average centroids
			for( int c=0; c<=_precomputed->_total_clusters; c++ ){

		    	if( _cluster_size[ c ] > 1 )
		    		divide_VectorFloat_by( _centre[ c ], float(_cluster_size[ c ]), _centre[ c ], PROBLEM->mdim() );

			}

			// Compute final cluster variances
			for( int c=0; c<=_precomputed->_total_clusters; c++ ){

				int m = _cluster_membership[ c ];

				double diff = PROBLEM->distance_measure( _precomputed->_centre[ c ],  _centre[ m ], PROBLEM->mdim() );
		        variance += _precomputed->_clusters[ c ][ 0 ] * ( diff * diff );

			}

			return ( variance / PROBLEM->ndata());

		}

		// Compute Connectivity measure using partial pre-computed data
		double total_connectivity(){

			double cnn = _precomputed->_connectivity;

			for( int i=0; i<_precomputed->_cnn_pairs; i++ ){

				int c1 = _precomputed->_cnn_pair[ i ][ 0 ], c2 = _precomputed->_cnn_pair[ i ][ 1 ];

				if( _cluster_membership[ c1 ] == _cluster_membership[ c2 ] )
					cnn -= _precomputed->_cnn_contribution[ c1 ][ c2 ]; 

			}

			return ( cnn > 0.00001 ) ? cnn : 0.0;

		}

		// Total number of (final) clusters
		int total_clusters(){

			return _total_clusters;

		}

};
////////////////////////////////////////////////////////////////////////////////

#endif

