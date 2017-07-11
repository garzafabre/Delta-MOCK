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

#ifndef __MOCK_NSGA2_HH__
#define __MOCK_NSGA2_HH__

/******************
Dependencies
******************/
#include "mock_Global.hh"
#include "mock_Algorithm.hh"
#include "mock_SolutionLocus.hh"
#include "mock_SolutionShort.hh"
#include "mock_SolutionSplit.hh"
#include "mock_Population.hh"
#include "mock_BinaryOperator.hh"
#include "mock_UnaryOperator.hh"
#include "mock_EvaluatorFull.hh"
#include "mock_EvaluatorDelta.hh"

/******************
Class definition
******************/
class Nsga2 : public Algorithm {

	/******************
	Attributes
	******************/

	protected:

		PopulationPtr _population;		// Main algorithm's (parent) population 

		PopulationPtr _offspring;		// Offspring population 

		PopulationPtr _auxiliary;		// Auxiliary population (for +selection-based replacement strategy)

		EvaluatorPtr _evaluator;		// Collection of performance measures and evaluation functions	

		int _population_size;			// Number of individuals in the GA's population

		int _max_generations;			// Maximum number of GA's generations

		unsigned long _max_evaluations;	// Maximum number of objective function evaluations
		
		double _crossover_prob;			// Probability of crossover
		
		double _mutation_prob;			// Probability of mutation

		int _generation;				// Current generation

		string _out_filename;			// Name of output file(s)

		int _frequency;					// How often to report results (generations)

		string _representation;			// Solutions' encoding to be used

		int _max_solutions;				// Maximum number of solutions to be processed

		// ---------------------------------			
		// NOTE: Attributes below are auxiliary structures that could have been defined
		// locally in the respective methods where they are used. However, these are defined
		// here in order to improve efficiency by avoiding allocating and deallocating
		// memory (in some cases also initialising) multiple times.
		// ---------------------------------

		MatrixIntPtr _nds_front;		// Fronts obtained from nondominated sorting procedure
		
		MatrixIntPtr _nds_s;			// Auxiliary structure for nds procedure
		
		VectorIntPtr _nds_n;			// Auxiliary structure for nds procedure

		VectorDoublePtr idx_val_tuples;	// Auxiliary structure for crowding distance

		VectorIntPtr parents;			// Permutations of parents used in mating selection

	/******************
	Methods
	******************/

	protected:

		// Method configuration
		virtual void initialise();
		virtual void configure();	

		// Initial solution generators
		void (Nsga2::*initialisation)( bool instantiate_offspring );
		
		void hybrid_initialisation( bool instantiate_offspring = true );
		void hybrid_initialisation_short( bool instantiate_offspring = true );
		void hybrid_initialisation_split( bool instantiate_offspring = true );
		SolutionPtr create_kmeans_solution( int n );
		SolutionPtr create_kmeans_solution_short( int n );
		SolutionPtr create_kmeans_solution_split( int n );
		SolutionPtr create_kmedoids_solution( int n );
		SolutionPtr create_kmedoids_solution_short( int n );
		SolutionPtr create_kmedoids_solution_split( int n );
		SolutionPtr create_priority_solution( int n );
		SolutionPtr create_priority_solution_short( int n );
		SolutionPtr create_priority_solution_split( int n );

		// Construction/processing of the initial parent population
		void generate_evaluate_initial_population();

		// NDS and crowding
		void nondominated_sorting( PopulationPtr pop );
		void crowding_distance( PopulationPtr pop, VectorIntPtr front );
		void crowding_distance_fronts( PopulationPtr pop );

		// Selection (mating and survival selection strategies)
		void selection_variation();		
		SolutionPtr binary_tournament( SolutionPtr p1, SolutionPtr p2 );
		void replacement();
		void nsga2_replecement( PopulationPtr source, PopulationPtr selected, PopulationPtr nonselected );

		// Variation operators
		void (*mutation_operator)( SolutionPtr const sol, double prob );
		void (*crossover_operator)( SolutionPtr const parent1, SolutionPtr const parent2, SolutionPtr child1, SolutionPtr child2, double const prob );

	public:

		// Constructor / destructor
		Nsga2();
		virtual ~Nsga2();

		// Main execution routine
		virtual void run();

		// Output generation / configuration details
		virtual void generate_output( int g = -1 );
    
};

#endif

