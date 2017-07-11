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

#include "mock_EvaluatorDelta.hh"

////////////////////////////////////////////////////////////////////////////////
// Constructor
EvaluatorDelta::EvaluatorDelta() :

    Evaluator()

{   

    // Allocate memory
    _clusters = new ClusterAssignmentDelta();
    _processed = new bool [ PROBLEM->ndata() ];
    _full_encoding = allocate_VectorInt( PROBLEM->ndata() );    

    // Initialise aux. structures
    for( int f=0; f<PROBLEM->num_fixed_edges(); f++ ){
        int fixed = PROBLEM->fixed_edge( f );
        _processed[ fixed ] = true;
        _full_encoding[ fixed ] = PROBLEM->mst_edge( fixed );
    }

}
////////////////////////////////////////////////////////////////////////////////
// Destructor
EvaluatorDelta::~EvaluatorDelta(){

    // Deallocate memory
    delete _clusters;    
    delete[] _processed;
    deallocate_VectorInt( _full_encoding );

}
////////////////////////////////////////////////////////////////////////////////
// Evaluates the given solution
void EvaluatorDelta::evaluate( SolutionPtr solution ){

    // Increase evaluations counter
    _total_evaluations++;

    // Reset cluster assignment based on precomputed information
    _clusters->reset();   

    // Set all non-fixed/relevant positions as unprocessed
    for( int r=0; r<PROBLEM->num_relevant_edges(); r++ ) _processed[ PROBLEM->relevant_edge( r ) ] = false;
        
    // Re-construct full encoding from given reduced-encoding solution
    solution->update_full_encoding( _full_encoding );

    // Process all non-fixed/relevant positions
    for( int r=0, mst_index, neighbour; r<PROBLEM->num_relevant_edges(); r++ ){

        mst_index = PROBLEM->relevant_edge( r );

        // Unprocessed?
        if( !_processed[ mst_index ] ){

            do{
                // Mark as processed
                _processed[ mst_index ] = true;

                // Process
                neighbour = _full_encoding[ mst_index ];

                // Merge connected components
                _clusters->merge( mst_index, neighbour );

                // Move processing to neighbour
                mst_index = neighbour;

            }while( !_processed[ mst_index ] );

        }

    }
    
    // Save evaluation information in solution object
    solution->objective( 0 ) = _clusters->total_variance();
    solution->objective( 1 ) = _clusters->total_connectivity();
    solution->kclusters() = _clusters->total_clusters(); 
    solution->evaluation() = _total_evaluations;

}
////////////////////////////////////////////////////////////////////////////////
// Evaluates the given population of solutions
void EvaluatorDelta::evaluate( PopulationPtr population ){

    // Evaluate each individual in the given population
    for( int i=0; i<population->size(); i++ ) evaluate( (*population)[ i ] );

}
////////////////////////////////////////////////////////////////////////////////

