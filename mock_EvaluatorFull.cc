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

#include "mock_EvaluatorFull.hh"

////////////////////////////////////////////////////////////////////////////////
// Constructor
EvaluatorFull::EvaluatorFull() : 
    
    Evaluator(),
    _knn( mock_L ) 

{

    // Pre-compute penalty values used by the connectivity measure
    // Penalty values are defined according to positions in NN list
    _connectivity_penalty = allocate_VectorDouble( _knn );
    for( int j=0; j<_knn; j++ ) _connectivity_penalty[ j ] = 1.0/(double(j)+1.0);

}
////////////////////////////////////////////////////////////////////////////////
// Destructor
EvaluatorFull::~EvaluatorFull(){

    // Free memory
    deallocate_VectorDouble( _connectivity_penalty );

}
////////////////////////////////////////////////////////////////////////////////
// Evaluates the given solution
void EvaluatorFull::evaluate( SolutionPtr solution ){

    // Increase evaluations counter
    _total_evaluations++;

    // Decode given solution and create clustering object
    ClusteringPtr clustering = solution->decode_clustering();

    // Evaluate and save evaluation information in solution object
    solution->objective( 0 ) = variance( clustering ); // overall_deviation( clustering );
    solution->objective( 1 ) = connectivity( clustering );
    solution->kclusters() = clustering->total_clusters();
    solution->evaluation() = _total_evaluations;

    // Free memory - delete clustering object
    delete clustering;
    
}
////////////////////////////////////////////////////////////////////////////////
// Evaluates the given population of solutions
void EvaluatorFull::evaluate( PopulationPtr population ){

    // Evaluate each individual in the given population
    for( int i=0; i<population->size(); i++ ) evaluate( (*population)[ i ] );

}
////////////////////////////////////////////////////////////////////////////////
// Computes the Connectivity measure for unsupervised clustering
double EvaluatorFull::connectivity( ClusteringPtr clustering ){

    // Compute connectivity measure
    double conn = 0.0;
    for( int i = 0; i < PROBLEM->ndata(); i++ ){
        
        // Label (cluster id) of i-th solution
        int label = clustering->assignment( i );

        // Verify labels of nearest neighbours
        double partial = 0.0;        
        for( int j = 0; j < _knn; j++ ){

            int nn_label = clustering->assignment( PROBLEM->neighbour( i, j ) );

            if (label != nn_label) {

                partial += _connectivity_penalty[ j ];

            }
            
        }
        conn += partial;
    }  

    return conn;

}
////////////////////////////////////////////////////////////////////////////////
// Computes the Overall Deviation measure for unsupervised clustering
double EvaluatorFull::overall_deviation( ClusteringPtr clustering ){

    double odev = 0.0;

    // Compute overall deviation 
    for( int i=0; i<PROBLEM->ndata(); i++ ){
        
        // Distance from data element to centre of its cluster
        odev += PROBLEM->distance_measure( (*PROBLEM)[ i ],  clustering->centre( clustering->assignment( i ) ), PROBLEM->mdim() );

    }   

    return odev;
    
}
////////////////////////////////////////////////////////////////////////////////
// Computes intra-cluster variance
double EvaluatorFull::variance( ClusteringPtr clustering ){

    double total_variance = 0.0;

    for( int i = 0; i < PROBLEM->ndata(); i++ ){

        double diff = PROBLEM->distance_measure( (*PROBLEM)[ i ],  clustering->centre( clustering->assignment( i ) ), PROBLEM->mdim() );
        total_variance += ( diff * diff );

    }

    // return sqrt( total_variance / double(PROBLEM->ndata()) ); // std dev
    return ( total_variance / double(PROBLEM->ndata()) );

}
////////////////////////////////////////////////////////////////////////////////
// Adjusted Rand Index (clustering object is passed)
// Computed for given candidate cluster assignment with respect to real classes
double EvaluatorFull::adjusted_rand_index( ClusteringPtr clustering ){

    // Can ARI be computed?
    if( !PROBLEM->labels_provided() ){

        cerr << "WARNING: Attempting to compute ADJUSTED RAND INDEX but real classes are not available!" << endl;
        return 0.0;
        
    }

    // Allocate memory
    VectorIntPtr cluster_size_real = allocate_VectorInt( PROBLEM->num_real_clusters() );
    VectorIntPtr cluster_size = allocate_VectorInt( clustering->total_clusters() );
    MatrixIntPtr contingency = allocate_MatrixInt( clustering->total_clusters(), PROBLEM->num_real_clusters() );

    // Initialise
    for( int i = 0; i < clustering->total_clusters(); i++ ){

        cluster_size[ i ] = 0;        

        for( int j = 0; j < PROBLEM->num_real_clusters(); j++ ){

            contingency[ i ][ j ] = 0;

            if( i == 0 ) cluster_size_real[ j ] = 0;

        }

    }

    // Compute contingency table
    for( int i = 0; i < PROBLEM->ndata(); i++ ){

        // Obtained assignment for i-th element
        int obtained = clustering->assignment( i );
        cluster_size[ obtained ]++;

        // Real assignment for i-th element
        int real = PROBLEM->label( i );
        cluster_size_real[ real ]++;

        // Update contingency table
        contingency[ obtained ][ real ]++;

    }

    // Compute different terms of ARI equation
    double sum_cells = 0.0;
    double sum_rows = 0.0;
    double sum_cols = 0.0;

    for( int i = 0; i < clustering->total_clusters(); i++ ){

        sum_rows += combinations( cluster_size[ i ], 2 );

        for( int j = 0; j < PROBLEM->num_real_clusters(); j++ ){

            sum_cells += combinations( contingency[ i ][ j ], 2 );

            if( i == 0 ) sum_cols += combinations( cluster_size_real[ j ], 2 );
        }
    }

    // Top part of the equation
    double top = sum_cells - ( sum_rows * sum_cols ) / combinations( PROBLEM->ndata(), 2 );

    // Bottom part of the equation
    double bottom = 0.5 * ( sum_rows + sum_cols ) - ( sum_rows * sum_cols ) / combinations( PROBLEM->ndata(), 2 );

    // Compute ARI
    double ari = ( top / bottom );    

    // Free memory
    deallocate_VectorInt( cluster_size_real );
    deallocate_VectorInt( cluster_size );
    deallocate_MatrixInt( contingency, clustering->total_clusters() );

    // Return result
    return ari;

}
////////////////////////////////////////////////////////////////////////////////
// Adjusted Rand Index (solution object is passed, decoding is needed)
// Computed for given candidate cluster assignment with respect to real classes
double EvaluatorFull::adjusted_rand_index( SolutionPtr solution ){

    // Decode given solution
    ClusteringPtr clustering = solution->decode_clustering();

    // Compute ARI
    double ari = adjusted_rand_index( clustering );

    // Free memory
    delete clustering;

    // Return results
    return ari;

}
////////////////////////////////////////////////////////////////////////////////




