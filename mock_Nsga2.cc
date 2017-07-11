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

#include "mock_Nsga2.hh"

////////////////////////////////////////////////////////////////////////////////
// Constructor
Nsga2::Nsga2() : 

	Algorithm( "nsga2" )

{

	initialise();

}
////////////////////////////////////////////////////////////////////////////////
// Destructor
Nsga2::~Nsga2(){

	// Deallocate memory
	delete _population;
	delete _offspring;
	delete _auxiliary;
	delete _evaluator;
	deallocate_MatrixInt( _nds_front, _max_solutions );
	deallocate_MatrixInt( _nds_s, _max_solutions );
	deallocate_VectorInt( _nds_n );
	deallocate_VectorDouble( idx_val_tuples );
	deallocate_VectorInt( parents );
	
}
////////////////////////////////////////////////////////////////////////////////
// Initialises based on command-line input parameters
// and invokes configuration of search algorithm
void Nsga2::initialise(){

	#ifdef DISPLAY_PROGRESS_MESSAGES
		cout << "\tInitialising " + _algorithm_name << endl;
	#endif

	// Set default parameter values
	_population_size = 100;
	_crossover_prob = 1.0;
	_mutation_prob = 1.0; // This means: 1.0 / double( Solution::encoding_length() );
	_max_generations = 101;
	_max_evaluations = 0;
	_out_filename = "";
	_frequency = 0;
	_representation = "locus";

	// Load and set input parameters 
	// (these override default settings if provided)
	for( int i=0; i<input_parameters.size(); i++ ){

		// Retrieve (option, value) tuple
		string option = input_parameters[ i ][ 0 ];
		string value = input_parameters[ i ][ 1 ];

		// Set relevant options
		if( (option == "--population") ){

			// Population size
			_population_size = stoi( value );

		}else if( (option == "--generations") ){

			// Number of generations
			_max_generations = stoi( value );

		}else if( (option == "--evaluations") ){

			// Number of evaluations
			_max_evaluations = stoul( value );

		}else if( (option == "--crossover") ){

			// Crossover probability
			_crossover_prob = stof( value );

		}else if( (option == "--mutation") ){

			// Mutation probability
			_mutation_prob = stof( value );

		}else if( (option == "--output") ){

			// Output filename
			_out_filename = value;

		}else if( (option == "--frequency") ){

			// How often to report results (generations)
			_frequency = stoi( value );

		}else if( (option == "--representation") ){

			// Solutions' encoding
			_representation = value;

		}

	}	

	// Configure method
	configure();

}
////////////////////////////////////////////////////////////////////////////////
// Configuration of search algorithm based on input and default parameters
void Nsga2::configure(){

	#ifdef DISPLAY_PROGRESS_MESSAGES
		cout << "\tConfiguring " + _algorithm_name << endl;
	#endif

	// Total number of solutions to be generated during initialisation (ensure TOTAL_INITIAL_SOLUTIONS >= _population_size)
	if( TOTAL_INITIAL_SOLUTIONS < _population_size)	TOTAL_INITIAL_SOLUTIONS = _population_size; 

	// Max K value to be considered during initialisation
	// If this parameter is not defined (or < 2), set it to the total number of initial solutions
	if( mock_Kmax < 2 ) mock_Kmax = TOTAL_INITIAL_SOLUTIONS;
	if( mock_Kmax > PROBLEM->ndata() ) mock_Kmax = PROBLEM->ndata();

	// Determine maximum number of solutions that are to be processed
	// This allows to allocate sufficient amount of memory for the data structures	
	_max_solutions = ( TOTAL_INITIAL_SOLUTIONS > 2 * _population_size ) ? TOTAL_INITIAL_SOLUTIONS : 2 * _population_size;

	// Allocate memory for populations 
	if( _population_size % 4 != 0 ) error_message_exit( "Population size needs to be a multiple of 4! (-p,--population)" );	
	_population = PopulationPtr( new Population( _population_size ) );
	_offspring = PopulationPtr( new Population( _population_size ) );
	_auxiliary = PopulationPtr( new Population( _max_solutions ) );	

	// Allocate memory for auxiliary data strucrures, initialise
	_nds_front = allocate_MatrixInt( _max_solutions, _max_solutions + 1 );
	for( int i=0; i<_max_solutions; i++ ) _nds_front[ i ][ 0 ] = 0;		
	_nds_s = allocate_MatrixInt( _max_solutions, _max_solutions + 1 );
	_nds_n = allocate_VectorInt( _max_solutions );
	idx_val_tuples = allocate_VectorDouble( 2 * _max_solutions );
	parents = allocate_VectorInt( _population_size );
	for( int i=0; i<_population_size; i++ ) parents[ i ] = i;

	// Set stopping condition
	_max_evaluations = ( _population_size * (_max_generations-1) + TOTAL_INITIAL_SOLUTIONS ); // Since TOTAL_INITIAL_SOLUTIONS >= _population_size

	// Configure search method according to chosen representation
	if( _representation == "locus" ){

		initialisation = &Nsga2::hybrid_initialisation; 
		crossover_operator = &BinaryOperator::uniform_crossover;
		mutation_operator = &UnaryOperator::neighbourhood_biased_mutation;

	}else if( _representation == "short" ){

		initialisation = &Nsga2::hybrid_initialisation_short;		
		crossover_operator = &BinaryOperator::uniform_crossover;
		mutation_operator = &UnaryOperator::neighbourhood_biased_mutation_short;

	}else if( _representation == "split" ){

		initialisation = &Nsga2::hybrid_initialisation_split;		
		crossover_operator = &BinaryOperator::uniform_crossover;
		mutation_operator = &UnaryOperator::neighbourhood_biased_mutation_split;

	}else{
	 	
		error_message_exit( "Unrecognised encoding scheme (-r, --representation): " + _representation );

	}  

}
////////////////////////////////////////////////////////////////////////////////
// Main execution routine
void Nsga2::run(){

	#ifdef DISPLAY_PROGRESS_MESSAGES
		cout << "Running " + _algorithm_name << endl;
	#endif	

	// Generate initial set of solutions (specialised initialisation based on MST, interestingness, and kmeans)	
	(this->*initialisation)( true ); 

	// Instantiate Evaluator object
	// This depends on the particular encoding scheme used
	// NOTE: Instantiation of EvaluatorDelta must be after initialisation
	// since it requires first identifying the relevant and fixed edges
	if( _representation == "locus" ){

		_evaluator = EvaluatorPtr( new EvaluatorFull() ); 

	}else{
	 	
	 	_evaluator = EvaluatorPtr( new EvaluatorDelta() ); 

	} 

	// Construct initial parent population based on the solutions obtained during initialisation
	// Evaluate and rank population
	generate_evaluate_initial_population();
	_generation = 1;

	#ifdef DISPLAY_PROGRESS_MESSAGES
		cout << "\tGeneration: " << _generation << " ( " << _evaluator->total_evaluations() << " evaluations )" << endl;
	#endif

	// Report initial population
	if( _frequency > 0 ) generate_output( _generation );	
	
	// Main loop
	// for( _generation = 2; _generation <= _max_generations; _generation++ ){ // Stop condition is based on number of generations
	for( _generation = 2; _evaluator->total_evaluations() < _max_evaluations; _generation++ ){ // Stop condition is based on number of evaluations
		
		// Selection (binary tournament) and variation (crossover, mutation)
		selection_variation();

		// Evaluate produced offspring
		_evaluator->evaluate( _offspring );

		// Replacement (survival selection)
		replacement();		

		#ifdef DISPLAY_PROGRESS_MESSAGES
			cout << "\tGeneration: " << _generation << " ( " << _evaluator->total_evaluations() << " evaluations )" << endl;
		#endif	

	}	

}
////////////////////////////////////////////////////////////////////////////////
// Creates interesting solution by removing the 'n' most interesting MST links
SolutionPtr Nsga2::create_priority_solution( int n ){

	// Create new MST solution
	SolutionPtr sol = SolutionPtr( new SolutionLocus( false ) );
	for( int i = 0; i < PROBLEM->ndata(); i++ )	(*sol)[ i ] = PROBLEM->mst_edge( i );

	// Remove the n highest priority MST edges
	for( int i = 0; i < n; i++ ){

		// Get i-th most interesting edge
		int edge = PROBLEM->priority_edge( i );

		// Remove edge, replace with edge to one of the nearest neighbours
		(*sol)[ edge ] = SolutionLocus::static_random_encoding( edge, (*sol)[ edge ] );			

	}

	return sol;

}
////////////////////////////////////////////////////////////////////////////////
void Nsga2::hybrid_initialisation( bool instantiate_offspring ){

	// *******************************************************
	// 1- Add full MST solution
	// *******************************************************

	SolutionPtr sol = SolutionPtr( new SolutionLocus( false ) );
	for( int i = 0; i < PROBLEM->ndata(); i++ )	(*sol)[ i ] = PROBLEM->mst_edge( i );	
	_auxiliary->add( sol );

	// *******************************************************
	// 2- Fill repository with K-means or interesting solutions with equal probability
	// *******************************************************

	// List of all possible k values
	VectorIntPtr possible_k = allocate_VectorInt( mock_Kmax-1 );
	for( int k = 2; k <= mock_Kmax; k++ ) possible_k[ k-2 ] = k;

	do{

		// Get permutation of all possible k values
		shuffle( possible_k, mock_Kmax-1 ); 

		for( int k = 2; k <= mock_Kmax && _auxiliary->size() < TOTAL_INITIAL_SOLUTIONS; k++ ){

			// Actual value of k
			int target_k = possible_k[ k-2 ];			

			// Create priority solution
			sol = create_priority_solution( target_k-1 );

			// Add solution to repository
			_auxiliary->add( sol );

		}

	}while( _auxiliary->size() < TOTAL_INITIAL_SOLUTIONS );	

	// Clean memory
	deallocate_VectorInt( possible_k );

	// *******************************************************
	// Instantiate offspring population?
	// *******************************************************
	if( instantiate_offspring ){

		for( int i = 0; i < _population_size; i++ ){ 
			
			_offspring->add( SolutionPtr( new SolutionLocus( false ) ) );

		} 

	}

}		
////////////////////////////////////////////////////////////////////////////////
// Creates interesting solution by removing the 'n' most interesting MST links
SolutionPtr Nsga2::create_priority_solution_short( int n ){

	// Create new MST solution
	SolutionPtr sol = SolutionPtr( new SolutionShort( false ) );

	// Define encoding
	for( int i = 0, edge; i < SolutionShort::static_encoding_length(); i++ ){

		edge = PROBLEM->relevant_edge( i );
		(*sol)[ i ] = PROBLEM->mst_edge( edge );

		if( PROBLEM->relevant_index( edge ) < n )
			(*sol)[ i ] = SolutionShort::static_random_encoding( i, (*sol)[ i ] );

	}

	return sol;

}
////////////////////////////////////////////////////////////////////////////////
// Specialised initialisation of MOCK based on MST and K-means
// Uses Reduced Locus-based representation
void Nsga2::hybrid_initialisation_short( bool instantiate_offspring ){

	// Define relevant edges
	PROBLEM->determine_relevant_edges();

	// *******************************************************
	// 1- Add full MST solution
	// *******************************************************

	SolutionPtr sol = SolutionPtr( new SolutionShort( false ) );
	for( int i = 0; i < SolutionShort::static_encoding_length(); i++ )	(*sol)[ i ] = PROBLEM->mst_edge( PROBLEM->relevant_edge( i ) );	
	_auxiliary->add( sol );

	// *******************************************************
	// 2- Fill repository with K-means or interesting solutions with equal probability
	// *******************************************************

	// List of all possible k values
	VectorIntPtr possible_k = allocate_VectorInt( mock_Kmax-1 );
	for( int k = 2; k <= mock_Kmax; k++ ) possible_k[ k-2 ] = k;

	do{

		// Get permutation of all possible k values
		shuffle( possible_k, mock_Kmax-1 ); 

		for( int k = 2; k <= mock_Kmax && _auxiliary->size() < TOTAL_INITIAL_SOLUTIONS; k++ ){

			// Actual value of k
			int target_k = possible_k[ k-2 ];			

			// Create priority solution
			sol = create_priority_solution_short( target_k-1 );

			// Add solution to repository
			_auxiliary->add( sol );

		}

	}while( _auxiliary->size() < TOTAL_INITIAL_SOLUTIONS );	

	// Clean memory
	deallocate_VectorInt( possible_k );

	// *******************************************************
	// Instantiate offspring population?
	// *******************************************************
	if( instantiate_offspring ){

		for( int i = 0; i < _population_size; i++ ){ 
			
			_offspring->add( SolutionPtr( new SolutionShort( false ) ) );

		} 

	}

}
////////////////////////////////////////////////////////////////////////////////
// Creates interesting solution by removing the 'n' most interesting MST links
SolutionPtr Nsga2::create_priority_solution_split( int n ){

	// Create new MST solution
	SolutionPtr sol = SolutionPtr( new SolutionSplit( false ) );

	// Define encoding
	for( int i = 0, edge; i < SolutionSplit::static_encoding_length(); i++ ){

		edge = PROBLEM->relevant_edge( i );
		(*sol)[ i ] = ( PROBLEM->relevant_index( edge ) < n ) ? 0 : 1;

	}

	return sol;

}
////////////////////////////////////////////////////////////////////////////////
// Specialised initialisation of MOCK based on MST and K-means
// Uses (binary) Split-based representation
void Nsga2::hybrid_initialisation_split( bool instantiate_offspring ){

	// Define relevant edges
	PROBLEM->determine_relevant_edges();

	// *******************************************************
	// 1- Add full MST solution
	// *******************************************************

	SolutionPtr sol = SolutionPtr( new SolutionSplit( false ) );
	for( int i = 0; i < SolutionSplit::static_encoding_length(); i++ )	(*sol)[ i ] = 1;	
	_auxiliary->add( sol );

	// *******************************************************
	// 2- Fill repository with K-means or interesting solutions with equal probability
	// *******************************************************

	// List of all possible k values
	VectorIntPtr possible_k = allocate_VectorInt( mock_Kmax-1 );
	for( int k = 2; k <= mock_Kmax; k++ ) possible_k[ k-2 ] = k;

	do{

		// Get permutation of all possible k values
		shuffle( possible_k, mock_Kmax-1 ); 

		for( int k = 2; k <= mock_Kmax && _auxiliary->size() < TOTAL_INITIAL_SOLUTIONS; k++ ){

			// Actual value of k
			int target_k = possible_k[ k-2 ];			

			// Create priority solution
			sol = create_priority_solution_split( target_k-1 );

			// Add solution to repository
			_auxiliary->add( sol );

		}

	}while( _auxiliary->size() < TOTAL_INITIAL_SOLUTIONS );	

	// Clean memory
	deallocate_VectorInt( possible_k );

	// *******************************************************
	// Instantiate offspring population?
	// *******************************************************
	if( instantiate_offspring ){

		for( int i = 0; i < _population_size; i++ ){ 
			
			_offspring->add( SolutionPtr( new SolutionSplit( false ) ) );

		} 

	}

}
////////////////////////////////////////////////////////////////////////////////
// Constructs initial parent population based on the set of solutions generated during initialisation
// Evaluate and rank population
void Nsga2::generate_evaluate_initial_population(){

	// Given the set of initial solutions (_auxiliary), define initial parent population (_population)
	if( _auxiliary->size() >= _population_size ){

		// Evaluate initial solutions
    	_evaluator->evaluate( _auxiliary );

		// Allocate aux memory to store nonselected individuals
		PopulationPtr toremove = PopulationPtr( new Population( _auxiliary->size() - _population_size ) );	

		// Apply NSGA-II replacement strategy based on nondominated sorting and crowding distance
		_population->clear();

		nsga2_replecement( _auxiliary, _population, toremove );

		// Clear auxiliary population
		_auxiliary->clear();
		
		// Free aux memory
		delete toremove;

	}else{

		// This should not happen
		error_message_exit( "Problem: _auxiliary->size() < _population_size" );

	}		

	// Validate population size
	if( _population->size() != _population_size || _offspring->size() != _population_size ){

		error_message_exit( "Population size is not consistent with actual size of the generated/initialised populations!" );

	}

}
////////////////////////////////////////////////////////////////////////////////
// Mating selection, crossover and mutation
void Nsga2::selection_variation(){	

	// Two repetitions are required in order to produce all offspring
	for( int rep=0, ctr=0; rep<2; rep++ ){

		// Generate permutation of the parents
		shuffle( parents, _population_size );

		// Apply selection, crossover and mutation
		for( int i=0; i<_population_size; i+=4 ){

			// Decide parents by means of tournament selection
			SolutionPtr parent1 = binary_tournament(	(*_population)[ parents[ i   ] ]	,
														(*_population)[ parents[ i+1 ] ] 	);
			SolutionPtr parent2 = binary_tournament(	(*_population)[ parents[ i+2 ] ]	,
														(*_population)[ parents[ i+3 ] ] 	);

			// Pointers to children to be created
			SolutionPtr child1 = (*_offspring)[ ctr++ ];
			SolutionPtr child2 = (*_offspring)[ ctr++ ];

			// Apply crossover
			(*crossover_operator)( parent1, parent2, child1, child2, _crossover_prob );

			// Apply mutation
			(*mutation_operator)( child1, _mutation_prob ); 
			(*mutation_operator)( child2, _mutation_prob ); 

		}

	}

}
////////////////////////////////////////////////////////////////////////////////
// Binary tournament selection
SolutionPtr Nsga2::binary_tournament( SolutionPtr p1, SolutionPtr p2 ){

	// First criterion: dominance
	int dominance = pareto_dominance( p1->objective(), p2->objective(), num_objectives );
	if( dominance == 1 ) return p1;
	if( dominance == 2 ) return p2;

	// Second criterion: crowding
	if( p1->crowding_distance() > p2->crowding_distance() ) return p1;
	if( p1->crowding_distance() < p2->crowding_distance() ) return p2;

	// Otherwise: random decision
	return ( random_real(0, 1) < 0.5 ) ? p1 : p2;

}
////////////////////////////////////////////////////////////////////////////////
// Nondominated Sorting procedure
void Nsga2::nondominated_sorting( PopulationPtr pop ){

	int p, q, i, dominance, size = pop->size();	
	
	// Initialise
	for( p=0; p<size; p++ ){

		_nds_n[ p ] = 0;		
		_nds_s[ p ][ 0 ] = 0;
		_nds_front[ p ][ 0 ] = 0;

	}
	
	// Compute first front
	for( p=0; p<size; p++ ){

		for( q=0; q<size; q++ ){
			
			if( p!=q ){
				
				dominance = pareto_dominance( (*pop)[ p ]->objective(), (*pop)[ q ]->objective(), num_objectives );
				
				if( dominance == 1 ){ // p dominates q   ->  include q in _nds_s[p]

					_nds_s[ p ][ 0 ]++;
					_nds_s[ p ][ _nds_s[ p ][ 0 ] ] = q;	

				}else if(dominance == 2){ // q dominates p   ->  increase rank

					_nds_n[ p ] += 1;

				}	

			}	

		}
		
		if( _nds_n[ p ] == 0 ){ // p is a member of the first front

			_nds_front[ 0 ][ 0 ]++;
			_nds_front[ 0 ][ _nds_front[ 0 ][ 0 ] ] = p;		
			(*pop)[ p ]->rank() = 1;

		}

	}
		
	// Compute all other fronts
	for( i=0; _nds_front[ i ][ 0 ]>0  && i<size-1; i++ ){
		
		_nds_front[ i+1 ][ 0 ] = 0;

		for( p=1; p <= _nds_front[ i ][ 0 ]; p++ ){
			
			for( q=1; q <= _nds_s[ _nds_front[ i ][ p ] ][ 0 ]; q++ ){
				
				_nds_n[ _nds_s[ _nds_front[ i ][ p ] ][ q ] ]--;
				
				if( _nds_n[ _nds_s[ _nds_front[ i ][ p ] ][ q ] ] == 0 ){	

					_nds_front[ i+1 ][ 0 ]++;
					_nds_front[ i+1 ][ _nds_front[ i+1 ][ 0 ] ] = _nds_s[ _nds_front[ i ][ p ] ][ q ];
					(*pop)[ _nds_s[ _nds_front[ i ][ p ] ][ q ] ]->rank() = i+2;	

				}

			}		

		}

	}			

}
////////////////////////////////////////////////////////////////////////////////
// Compute the crowding distance for solutions in each front
void Nsga2::crowding_distance_fronts( PopulationPtr pop ){
	
	for( int f = 0; f < pop->size() && _nds_front[ f ][ 0 ] > 0; f++ ){ 		

		// Compute crowding distance of the f-th front
		crowding_distance( pop, _nds_front[ f ] );	

	}

}
////////////////////////////////////////////////////////////////////////////////
// Compute the crowding distance of the given population 
// Only consider solutions in the given front
void Nsga2::crowding_distance( PopulationPtr pop, VectorIntPtr front ){
	
	// Initialise
	for( int i = 1; i <= front[ 0 ]; i++ ) (*pop)[ front[ i ] ]->crowding_distance() = 0.0;

	// Compute crowding distance
	for( int m = 0; m < num_objectives; m++ ){
		
		// Get (idx, val) tuples
		for( int i = 1, ctr = 0; i <= front[ 0 ]; i++ ){	

			// Insert idx
            idx_val_tuples[ ctr++ ] = double( front[ i ] );

            // Insert objective value
            idx_val_tuples[ ctr++ ] = (*pop)[ front[ i ] ]->objective( m );
			
		}
				
		// Sort tuples
        qsort( (void *)idx_val_tuples, front[ 0 ], (2*sizeof(double)), compare_ascending_tuple );       

		if( idx_val_tuples[ 1 ] != idx_val_tuples[ 2*front[ 0 ]-1 ] ){
		
			// Assign INF to extreme solutions
			(*pop)[ idx_val_tuples[ 0 ] ]->crowding_distance() = INF;	// Best soltion m-th objective

			// Add cuboid distance for all other solutions
			for( int i = 1; i < front[ 0 ]-1; i++ ){ // idx: i*2, value: i*2+1		

				if( (*pop)[ idx_val_tuples[ i*2 ] ]->crowding_distance() != INF ){

					// (*pop)[ idx_val_tuples[ i*2 ] ]->crowding_distance() += ( idx_val_tuples[ (i+1)*2+1 ] - idx_val_tuples[ (i-1)*2+1 ] ); // Without Normalization
					(*pop)[ idx_val_tuples[ i*2 ] ]->crowding_distance() += ( idx_val_tuples[ (i+1)*2+1 ] - idx_val_tuples[ (i-1)*2+1 ] ) / ( idx_val_tuples[ 2*front[ 0 ]-1 ] - idx_val_tuples[ 1 ] ); // With Normalization

				}

			}

		}

	}

	// NOTE: Original NSGA-II code implements an additional step where the obtained crowding distance
	// values are divided by the number of objectives. However, this does not change rank ordering of 
	// solutions, and therefore is not here implemented for the sake of efficiency.

}	
////////////////////////////////////////////////////////////////////////////////
// NSGA-II replacement strategy based on nondominated sorting and crowding distance
void Nsga2::nsga2_replecement( PopulationPtr source, PopulationPtr selected, PopulationPtr nonselected ){

	int f, i, ctr;

	// Rank combined population by means of Nondominated Sorting
	nondominated_sorting( source );		

	// Fill new population based on the computed ranks, use crowding distance as secondary criterion
	// Select _population_size individuals from the 2*_population_size individuals of the merged population
	// Nonselected individuals need to be copied to children population	(allocated memory will be reused)
	for( f = 0; selected->size() < _population_size; f++ ){

		// Compute crowding distance of individuals in the current front
		// Even if this is not always necessary to discriminate in survival selection,
		// crowding is used in mating selection as a secondary criterion, thus
		// needs to be computed for all selected individuals
		crowding_distance( source, _nds_front[ f ] );	

		// Space available for all individuals in this front?
		if( selected->size() + _nds_front[ f ][ 0 ] <= _population_size ){

			// Yes: add all individuals to the population
			for( i = 1; i <= _nds_front[ f ][ 0 ]; i++ ){

				selected->add( (*source)[ _nds_front[ f ][ i ] ] ); 

			}

		}else{

			// No: use crowding as a secondary criterion

			// Get (idx, val) tuples
			for( i = 1, ctr = 0; i <= _nds_front[ f ][ 0 ]; i++ ){

				// Insert idx
            	idx_val_tuples[ ctr++ ] = double( _nds_front[ f ][ i ] );

            	// Insert cwd value
            	idx_val_tuples[ ctr++ ] = (*source)[ _nds_front[ f ][ i ] ]->crowding_distance();

			}

			// Sort tuples in descending order of cwd (cwd is to be maximised)
        	qsort( (void *)idx_val_tuples, _nds_front[ f ][ 0 ], (2*sizeof(double)), compare_descending_tuple ); 

        	// Fill population with individuals in the less	crowded regions		
        	for( i = 0; selected->size() < _population_size; i++ ){ 

        		selected->add( (*source)[ idx_val_tuples[ i*2 ] ] ); 

        	}

			// The rest of the individuals are copied to children
			for( ; i < _nds_front[ f ][ 0 ]; i++ ){

				nonselected->add( (*source)[ idx_val_tuples[ i*2 ] ] ); 

			}

		}

	}

	// Remaining nonselected individuals need to be copied to children population
	// for( ; nonselected->size() < _population_size; f++ ){
	for( ; (selected->size() + nonselected->size()) < source->size(); f++ ){

		for( i = 1; i <= _nds_front[ f ][ 0 ]; i++ ) 
			nonselected->add( (*source)[ _nds_front[ f ][ i ] ] );

	}	

}
////////////////////////////////////////////////////////////////////////////////
// NSGA-II survival selection: +selection, nds, crowding distance
void Nsga2::replacement(){

	// Merge parent and offspring populations
	_auxiliary->clear();
	for( int i=0; i < _population_size; i++ ){

		_auxiliary->add( (*_population)[ i ] );
		_auxiliary->add( (*_offspring)[ i ] );

	}

	// Empty populations
	_population->clear();
	_offspring->clear();

	// Apply NSGA-II replacement strategy based on nondominated sorting and crowding distance
	nsga2_replecement( _auxiliary, _population, _offspring );

	// Clear auxiliary population
	_auxiliary->clear();	

}
////////////////////////////////////////////////////////////////////////////////
// Reports results - generates output files
// Only considers nondominated solutions, except for g=1 (initial population)
void Nsga2::generate_output( int g ){	

	// Rank population by means of Nondominated Sorting
	nondominated_sorting( _population );

	ofstream file, file2, file3, file4;

	#ifdef DISPLAY_PROGRESS_MESSAGES
		cout << "Generating output files" << endl;
	#endif

	if( _out_filename.empty() ) _out_filename = _algorithm_name + "_output";

	// ****************************
	// MEASURES file
	// ****************************	

	string measures_filename;
	measures_filename = _out_filename + "_final_rank1_measures.txt";

	#ifdef DISPLAY_PROGRESS_MESSAGES
		if( g != -1 ) cout << "\t\t";	
		cout << "\tWriting file: " + measures_filename << endl;
	#endif

	file.open( measures_filename );

	EvaluatorFullPtr evaluator_full = EvaluatorFullPtr( new EvaluatorFull() );
	
	for( int i = 0, ctr = 0; i < _population_size; i++ ){

		if( (*_population)[ i ]->rank() == 1 ){

			ctr++;

			// Decode given solution
    		ClusteringPtr clustering = (*_population)[ i ]->decode_clustering();
			
			// If real clusters labels were provided, we can compute Adjusted Rand Index
    		if( PROBLEM->labels_provided() ){

    			// Variance, connectivity, k, ARI
				file << evaluator_full->variance( clustering ) << "\t" << evaluator_full->connectivity( clustering ) << "\t" << clustering->total_clusters() << "\t" << evaluator_full->adjusted_rand_index( clustering ) << endl;

    		}else{

    			// Variance, connectivity, k
				file << evaluator_full->variance( clustering ) << "\t" << evaluator_full->connectivity( clustering ) << "\t" << clustering->total_clusters() << endl;

    		}		

    		// CLUSTERING
			string clustering_filename;
			clustering_filename = _out_filename + "_solution_" + to_string( ctr ) + ".txt";
			file2.open( clustering_filename );
			for( int j = 0; j < PROBLEM->ndata(); j++ ){

				file2 << clustering->assignment( j ) << endl;

			}
			file2.close();	

			// Free memory
    		delete clustering;

		}

	}

	delete evaluator_full;

	file.close();	

}
////////////////////////////////////////////////////////////////////////////////

