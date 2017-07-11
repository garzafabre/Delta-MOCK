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

#include "mock_ClusteringProblem.hh"

////////////////////////////////////////////////////////////////////////////////
// Constructor
ClusteringProblem::ClusteringProblem() : 

	_data( nullptr ),
	_filename( "" ), 
	_ndata( -1 ),
	_mdim( -1 ),
	_labels_provided( false ), 
	_label( nullptr ),
	_num_real_clusters( -1 ),
	_normalise( true ),
	_distance_matrix( nullptr ),
	_nearest_neighbours( nullptr ),
    _distance( "" ),
    _delta( 0 )

{

	initialise();

}
////////////////////////////////////////////////////////////////////////////////
// Destructor
ClusteringProblem::~ClusteringProblem(){

	deallocate_MatrixFloat( _data, _ndata );
	deallocate_VectorInt( _label ); 
    deallocate_MatrixFloat( _distance_matrix, _ndata );
    deallocate_MatrixInt( _nearest_neighbours, _ndata );
	deallocate_MatrixInt( _neighbour_rank, _ndata );
    deallocate_VectorInt( _mst );
    deallocate_VectorDouble( _priority_edges );
    deallocate_VectorInt( _relevant_edges );
    delete[] _is_fixed;
    deallocate_VectorInt( _fixed_edges );    
    deallocate_VectorInt( _relevant_index );

}
////////////////////////////////////////////////////////////////////////////////
// Initialises based on command-line input parameters
// and invokes configuration of problem
void ClusteringProblem::initialise(){

	#ifdef DISPLAY_PROGRESS_MESSAGES
		cout << "\tLoading problem settings" << endl;
	#endif

	// Load and set input parameters
	for( int i=0; i<input_parameters.size(); i++ ){

		// Retrieve (option, value) tuple
		string option = input_parameters[ i ][ 0 ];
		string value = input_parameters[ i ][ 1 ];

		// Set relevant options
		if( (option == "--file") ){

			// Input data file
			_filename = value;

		}else if( (option == "--normalise") ){

            // Normalise data elements
            _normalise = ( value == "false" || value == "f" || value == "0") ? false : true;

        }else if( (option == "--delta") ){

            // DELTA parameter of Delta-MOCK
			_delta = stof( value );

            // Ensure in range [0,100]
            _delta = min( _delta, 100.0 );
            _delta = max( _delta, 0.0 );

		}

	}

	// Validate that required attributes were correctly set
	if( _filename.empty() ) error_message_exit( "-f,--filename option was not correctly set!" );

	// Load data and configure
	configure();

}
////////////////////////////////////////////////////////////////////////////////
// Loads data and configure problem 
// Invokes pre-computations required
void ClusteringProblem::configure(){

	#ifdef DISPLAY_PROGRESS_MESSAGES
		cout << "\tConfiguring problem" << endl;
	#endif

	// Load and prepare data
	load_data();

    // Pre-computation of distance matrix
    compute_distance_matrix(); 

    // Pre-computation of nearest neighbours
    compute_nearest_neighbours();

    // Pre-computation of the minimum spanning tree (MST)
    compute_mst();

}
////////////////////////////////////////////////////////////////////////////////
// Loads data from input file and applies normalisation
void ClusteringProblem::load_data(){

    #ifdef DISPLAY_PROGRESS_MESSAGES
        cout << "\t\tLoading and preparing data" << endl;
    #endif

    // Open input file
	ifstream input( _filename );
    if( !input ){
    	error_message_exit( "Error while trying to open file: " + _filename );
    }

    // Read ndata
    input >> _ndata;

    // Read mdim
    input >> _mdim;

    // Read labels_provided
    input >> _labels_provided;

    // Read labels_provided
    input >> _num_real_clusters;

    // Allocate memory for data
    _data = allocate_MatrixFloat( _ndata, _mdim );
    if( _labels_provided ) _label = allocate_VectorInt( _ndata );

    // Allocate memory for Min, Max, Avg, and Std, and initialise
    VectorFloatPtr _maximum = allocate_VectorFloat( _mdim );
    VectorFloatPtr _minimum = allocate_VectorFloat( _mdim );
    VectorFloatPtr _mean    = allocate_VectorFloat( _mdim );
    VectorFloatPtr _stdev   = allocate_VectorFloat( _mdim );
    for( int i=0; i<_mdim; i++ ){

        _maximum[ i ] = -INF;
        _minimum[ i ] = INF;
        _mean[ i ] = 0.0;
        _stdev[ i ] = 0.0;

    }

    // Load all data and compute Min, Max, Avg, and Std
    float value;
    for( int i=0; i<_ndata; i++ ){
     
     	// Read features/variables        
        for( int j=0; j<_mdim; j++ ){

        	if( input.eof() ){

        		error_message_exit( "EOF reached while reading file: '" + _filename + "'\nline " + to_string(i) + ", column " + to_string(j) );

        	}else{

        		// Read value
        		input >> value;

        		// Store value in data matrix
        		_data[ i ][ j ] = value;

                // Update max, min, mean
                _mean[ j ] += value;
                if( !_normalise ){

                    if( value < _minimum[ j ] ) _minimum[ j ] = value;
                    if( value > _maximum[ j ] ) _maximum[ j ] = value;

                }

        	}

        }

        // Read label 
        if( _labels_provided ){

        	if( input.eof() ){

        		error_message_exit( "EOF reached while reading file: '" + _filename + "'\nline " + to_string(i) + ", column " + to_string(_mdim) );

        	}else{

        		input >> _label[ i ];

        	}

        }

    }

    // Close file
    input.close();

    // Compute mean and std. dev. for each dimension
    for( int j=0; j<_mdim; j++ ){

        _mean[ j ] /= _ndata;

        for( int i=0; i<_ndata; i++ ){

            float diff = _data[ i ][ j ] - _mean[ j ];
            _stdev[ j ] += ( diff * diff );

        }

        _stdev[ j ] = sqrt( _stdev[ j ] / _ndata );

    }

    // Normalise, and compute min and max normalised values
    if( _normalise ){
        
        for( int i=0; i<_ndata; i++ ){    

            for( int j=0; j<_mdim; j++ ){

                // Normalise
                _data[ i ][ j ] -= _mean[ j ];
                if( _stdev[ j ] > 0 ) _data[ i ][ j ] /= _stdev[ j ];

                // Update max, min
                if( _data[ i ][ j ] < _minimum[ j ] ) _minimum[ j ] = _data[ i ][ j ];
                if( _data[ i ][ j ] > _maximum[ j ] ) _maximum[ j ] = _data[ i ][ j ];

            }

        }

        // Set mean and stdev
        for( int j=0; j<_mdim; j++ ){

            _mean[ j ] = 0.0;
            _stdev[ j ] = 1.0;

        }

    }   

    // Free memory
    deallocate_VectorFloat( _maximum );
    deallocate_VectorFloat( _minimum );
    deallocate_VectorFloat( _mean );
    deallocate_VectorFloat( _stdev );

}
////////////////////////////////////////////////////////////////////////////////
// Pre-computes (lower triangular) distance matrix 
void ClusteringProblem::compute_distance_matrix(){

    #ifdef DISPLAY_PROGRESS_MESSAGES
        cout << "\t\tPre-computing dissimilarity matrix" << endl;
    #endif

    // Define distance measure to use
    #if DISTANCE_MEASURE == EUCLIDEAN

        distance_measure = &euclidean_distance;
        _distance = "Euclidean distance";

    #elif DISTANCE_MEASURE == COSINE
        error_message_exit( "COSINE distance has not been implemented!" );
    #elif DISTANCE_MEASURE == CORRELATION
        error_message_exit( "CORRELATION distance has not been implemented!" );
    #elif DISTANCE_MEASURE == GAUSSIAN
        error_message_exit( "GAUSSIAN distance has not been implemented!" );
    #elif DISTANCE_MEASURE == JACCARD
        error_message_exit( "JACCARD distance has not been implemented!" );
    #else
        error_message_exit( "Undefined distance measure!" );
    #endif

    // Allocate memory - LOWER TRIANGULAR MATRIX (include diagonal)
    _distance_matrix = MatrixFloatPtr( new VectorFloatPtr [ _ndata ] );
    for( int i=0; i<_ndata; i++){

        _distance_matrix[ i ] = allocate_VectorFloat( i+1 );

    }

    // Compute distance matrix, and min, max, avg values
    float _min_distance = INF;     
    float _max_distance = -INF;    
    // float _avg_distance = 0.0; 

    for( int i=0; i<_ndata; i++ ){   

    	_distance_matrix[ i ][ i ] = 0.0;

        for( int j=i+1; j<_ndata; j++ ){

            // Compute distance and update matrix
            float dist = (*distance_measure)( _data[ i ], _data[ j ], _mdim );
            _distance_matrix[ j ][ i ] = dist;
            
            // Update avg, min, max
            // _avg_distance += dist;
            if( dist > _max_distance ) _max_distance = dist;
            if( dist < _min_distance ) _min_distance = dist;

        }

    }

    // Normalise distance matrix based on min and max distance values  
    for( int i=0; i<_ndata-1; i++ ){    

        for( int j=i+1; j<_ndata; j++ ){

            float dist = ( _distance_matrix[ j ][ i ] - _min_distance ) / ( _max_distance - _min_distance );
            _distance_matrix[ j ][ i ] = dist;

        }

    }

}
////////////////////////////////////////////////////////////////////////////////
// Computes matrix of nearest neighbours based on dissimilarities
void ClusteringProblem::compute_nearest_neighbours(){

    #ifdef DISPLAY_PROGRESS_MESSAGES
        cout << "\t\tPre-computing nearest neighbours" << endl;
    #endif

    // Allocate memory
    int size = _ndata-1;
    _nearest_neighbours = allocate_MatrixInt( _ndata, _ndata );
    _neighbour_rank = allocate_MatrixInt( _ndata, _ndata );
    VectorDoublePtr idx_val_tuples = allocate_VectorDouble( 2*size );

    // Compute distance-based sorted list of neighbours for each data element    
    for( int i=0; i<_ndata; i++ ){

        // Get (idx, val) tuples
        for( int j=0, ctr=0; j<_ndata; j++ ){

            if( i != j ){

                // Insert idx
                idx_val_tuples[ ctr++ ] = double( j );

                // Insert value
                idx_val_tuples[ ctr++ ] = double( distance( i, j ) );

            }

        }

        // Sort tuples
        qsort( (void *)idx_val_tuples, size, (2*sizeof(double)), compare_ascending_tuple );       

        //Save nn list and ranks (positions in nn list)
        for( int j=0; j<size; j++ ){

            _nearest_neighbours[ i ][ j+1 ] = int( idx_val_tuples[ j*2 ] );
            _neighbour_rank[ i ][ int(idx_val_tuples[ j*2 ]) ] = j+1;

        }

        // Set i is the closest neighborg of i
        _nearest_neighbours[ i ][ 0 ] = i;
        _neighbour_rank[ i ][ i ] = 0;

    }    

    // Free memory
    deallocate_VectorDouble( idx_val_tuples );

}
////////////////////////////////////////////////////////////////////////////////
// Pre-computation of the minimum spanning tree (MST)
// Prim's algorithm is empoyed
void ClusteringProblem::compute_mst(){

    #ifdef DISPLAY_PROGRESS_MESSAGES
        cout << "\t\tPre-computing MST" << endl;
    #endif

    // Memory allocation of structures to store results
    _mst = allocate_VectorInt( _ndata ); 
    _priority_edges = allocate_VectorDouble( 2*(_ndata) );
    _num_priority_edges = 0; 
    _relevant_edges = allocate_VectorInt( _ndata );
    _fixed_edges = allocate_VectorInt( _ndata );
    _num_relevant_edges = 0;
    _num_fixed_edges = 0;
    _is_fixed = (bool *)(new bool [ _ndata ]);
    _relevant_index = allocate_VectorInt( _ndata );

    // Mark all nodes (items) as "unselected"
    VectorIntPtr node = allocate_VectorInt( _ndata );
    int total_nodes = 0;
    bool *selected = (bool *)(new bool [ _ndata ]);
    for( int i=0; i<_ndata; i++ ){
        selected[ i ] = false;
    }

    // Include initial node, mark as "selected"
    // Any randomly selected item can be used as the starting point
    int r = random_int( 0, _ndata-1 );
    // r = 0;
    node[ total_nodes++ ] = r;
    selected[ r ] = true;

    // Apply Prim's method to compute the MST
    while( total_nodes < _ndata ){

        // Find edge (n1, n2) of minimal distance such that it 
        // connects a "selected" node n1 to an "unselected" node n2
        int n1, n2;
        double min_dist = INF;
        for( int i = 0, k = 0; i < total_nodes; i++ ){

            // Consider the closest "unselected" neighbour of i-th node
            for( k = 0; selected[ neighbour( node[ i ], k ) ] != false; k++ );

            // Update min. distance edge
            if( distance( node[ i ], neighbour( node[ i ], k ) ) < min_dist ){

                n1 = node[ i ];
                n2 = neighbour( n1, k );
                min_dist = distance( n1, n2 );

            }

        }

        // Save new edge found (n1, n2)
        _mst[ n2 ] = n1;
        if( total_nodes == 1 ){
            _mst[ n1 ] = n2;

            _fixed_edges[ _num_fixed_edges++ ] = n1; 
            _is_fixed[ n1 ] = true;   

        }

        // Include node n2 and mark as "selected"
        node[ total_nodes++ ] = n2;
        selected[ n2 ] = true;     

        /////////////////////////////////
        // Compute list priority
        /////////////////////////////////

        // Get ranks in NN list
        int mock_l = neighbour_rank( n1, n2 );
        int mock_k = neighbour_rank( n2, n1 );

        // Priority is defined in terms of interestingness + distance/length/weigth
        _priority_edges[ _num_priority_edges * 2        ] = n2;
        _priority_edges[ _num_priority_edges * 2 + 1    ] = min( mock_l, mock_k ) + distance( n1, n2 );
        _num_priority_edges++;

        if( total_nodes == 2 ){

            _priority_edges[ _num_priority_edges * 2        ] = n1;
            _priority_edges[ _num_priority_edges * 2 + 1    ] = min( mock_l, mock_k ) + distance( n1, n2 );
            _num_priority_edges++;

        }

    }   

    // Sort edges in descending order of priority
    qsort( (void *)_priority_edges, _num_priority_edges, (2*sizeof(double)), compare_descending_tuple );

    // Free memory
    delete[] selected;
    deallocate_VectorInt( node );
    
}
////////////////////////////////////////////////////////////////////////////////
// Defines which MST edges are to be considered relevant
void ClusteringProblem::determine_relevant_edges(){

    int i, edge, n;

    // _delta is in range [0,100]
    // _delta = 80, means 80% of the encoding will be fixed and pre-computed
    // Thus, the 20% top MST edges are to be considered as relevant
    n = max( 1, int((100.0 - _delta) / 100.0 * (_num_priority_edges)) );
    
    // Mark top n priority MST edges as relevant
    for( i = 0; i < n; i++ ){

        edge = int(_priority_edges[ i * 2 ]); // PROBLEM->priority_edge( i ); 
        _relevant_index[ edge ] = _num_relevant_edges;
        _relevant_edges[ _num_relevant_edges++ ] = edge;     
        _is_fixed[ edge ] = false;                               

    }

    // Mark remaining MST edges as fixed / non-relevant
    for( ; i < _num_priority_edges; i++ ){

        edge = int(_priority_edges[ i * 2 ]); // PROBLEM->priority_edge( i ); 
        _fixed_edges[ _num_fixed_edges++ ] = edge; 
        _is_fixed[ edge ] = true;                                    

    }

}
////////////////////////////////////////////////////////////////////////////////


