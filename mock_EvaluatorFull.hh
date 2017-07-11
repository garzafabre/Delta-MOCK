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

#ifndef __MOCK_EVALUATORFULL_HH__
#define __MOCK_EVALUATORFULL_HH__

/******************
Dependencies
******************/
#include "mock_EvaluatorFull.fwd.hh"
#include "mock_Evaluator.hh"
#include "mock_Clustering.hh"

/******************
Class definition
******************/
class EvaluatorFull : public Evaluator {
    
	/******************
	Attributes
	******************/

	private:

		VectorDoublePtr _connectivity_penalty;			// Pre-computed penalties for the connectivity measure

		const int _knn;										// Number of nearest neighbours to use (connectivity)

	/******************
	Methods
	******************/

	public:

		// Constructor / destructor
		EvaluatorFull();
		~EvaluatorFull();

		// Accesor to evaluations counter
		unsigned long int total_evaluations() const { return _total_evaluations; }

		// Evaluation of solution(s)
		void evaluate( SolutionPtr solution );
		void evaluate( PopulationPtr population );

		// Performance measures and objective functions

		// Internal criteria (dont use reference cluster labels)
		double overall_deviation( ClusteringPtr clustering );
		double connectivity( ClusteringPtr clustering );		
		double variance( ClusteringPtr clustering );	

		// External criteria (use reference cluster labels)
		double adjusted_rand_index( ClusteringPtr clustering );
		double adjusted_rand_index( SolutionPtr solution );
		
};

#endif

