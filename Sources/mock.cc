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

#include "mock.hh"

////////////////////////////////////////////////////////////////////
// Main execution routine
int main( int argc, char *argv[] ){

	#ifdef DISPLAY_PROGRESS_MESSAGES
		cout << "----------------\nMOCK - Start\n----------------" << endl;
	#endif	

	// Parse input parameters and initialises 
	// problem's and algorithm's parameters
	initialise( argc, argv );

	// Apply search algorithm 
	ALGO->run();

	// Report results
	ALGO->generate_output();

	// Free memory
	clean_memory();

	#ifdef DISPLAY_PROGRESS_MESSAGES
		cout << "----------------\nMOCK - End\n----------------" << endl;
	#endif	

	return 0;

}
////////////////////////////////////////////////////////////////////
// Parse input parameters and initialises 
// problem's and algorithm's parameters
void initialise( int argc, char *argv[] ){
	
	#ifdef DISPLAY_PROGRESS_MESSAGES
		cout << "Initialising:" << endl;
	#endif

	// Read input parameters
	parse_input_parameters( argc, argv );

	// Initialise random numbers generator
	initialise_random( seed );

	// Create and configure problem instance	
	PROBLEM = ClusteringProblemPtr( new ClusteringProblem() );

	// Instantiate and configure search algorithm
	if( algorithm_name == "nsga2" ){

		ALGO = AlgorithmPtr( new Nsga2() );

	}else{
	 	
 		show_usage( string( argv[0] ) );
		error_message_exit( "Unrecognised search algorithm (-a,--algorithm): " + algorithm_name );

	}

}
////////////////////////////////////////////////////////////////////
// Reads command-line input parameters
void parse_input_parameters( int argc, char *argv[] ){
	
	#ifdef DISPLAY_PROGRESS_MESSAGES
		cout << "\tParsing input parameters" << endl;
	#endif

	// Read command-line input parameters
	// Parameters are given in tuples (option, value)
	// except for "help" option for which no value is provided.
	for( int i = 1; i < argc; i++ ){

		// Read option name (convert to lowercase)
		string option = argv[ i ];
		std::transform( option.begin(), option.end(), option.begin(), ::tolower );

		// Verify valid options were provided
		if( !(
			(option == "--help") 			||
			(option == "--file") 			||
			(option == "--normalise") 		||
			(option == "--algorithm")		||
			(option == "--population")		||
			(option == "--crossover")		||
			(option == "--mutation")		||
			(option == "--generations")		||	
			(option == "--representation")	||
			(option == "--delta")			||	
			(option == "--kmax") 			||
			(option == "--seed")			||
			(option == "--output")			||
			(option == "--initialsize") 	||
			(option == "--lparameter") 		||
			(option == "--evaluations")			
		)){

			show_usage( string( argv[0] ) );
			error_message_exit( "Unrecognised command-line option: " + option );

		} 

		// Help option?
		if( (option == "--help") ){

			show_usage( string( argv[0] ) );
			exit( 0 );

		}else{

			// Verify option value has been provided
			if( i+1 >= argc ){

				error_message_exit( "No value was provided for command-line option: " + option );

			}       	

			// Read option value (convert to lowercase)
			string value = argv[ ++i ];

			if(  (option != "--output") && (option != "--file") ){ // Case of input and output filenames is not affected

				std::transform( value.begin(), value.end(), value.begin(), ::tolower );

			}

			// Option value should not start with "-"
			if( value[0] == '-' ){

				error_message_exit( "Provided value is not valid for command-line option '" + option + "': " + value );

			}       	

			// --seed option
			if( (option == "--seed") ){

				// Seed for random numbers
				seed = stoul( value );
				continue;

			}

			// --lparameter option
			if( (option == "--lparameter") ){

				// L parameter
				mock_L = stoi( value );
				continue;

			}

			// --kmax option
			if( (option == "--kmax") ){

				// Kuser parameter
				mock_Kmax = stoi( value );
				continue;

			}

			// --initialsize option
			if( (option == "--initialsize") ){

				// Number of solutions to be generated during initialisation
				TOTAL_INITIAL_SOLUTIONS = stoi( value );
				continue;

			}

			// --algorithm option 
			if( (option == "--algorithm") ){

				// Input data file
				algorithm_name = value;
				continue;

			}

			// Add tuple to global list of input parameters
			input_parameters.resize( input_parameters.size()+1 );	
			input_parameters.back().resize( 2 );	
			input_parameters.back()[ 0 ] = option;
			input_parameters.back()[ 1 ] = value;

		}

	}

	// Verify mandatory attributes
	if( algorithm_name.empty() ) error_message_exit( "-a,--algorithm option was not correctly set!" );

}
////////////////////////////////////////////////////////////////////
// Displays usage help information
void show_usage( string program_name ){

	   std::cout	
	    << "\n****************************************"
		<< "****************************************\n"
		<< "\nUSAGE:\n\n      " << program_name << "   OPTIONS\n\n"
		<< "      OPTIONS:\n\n"
		<< "      --help            Shows this help message\n\n"        	
		<< "      --file            Path and name of data file\n\n"        	
		<< "      --normalise       Data normalisation: { true, false}\n\n"        	
		<< "      --algorithm       Optimiser to use: { nsga2 }\n\n"        	
		<< "      --population      Size of the population\n\n"        	
		<< "      --crossover       Crossover probability\n\n"        	
		<< "      --mutation        Mutation probability."
		<< " This is divided by the length of the genotype\n\n"        	
		<< "      --generations     Number of generations (iterations) of the algorithm\n\n"        	
		<< "      --representation  Representation to use: { locus, short, split }\n\n"        	
		<< "      --delta           Parameter of the reduced-length (short, split) representations\n\n"        	
		<< "      --kmax            Parameter of the initialisation routine\n\n"        	
		<< "      --output          Path and/or a prefix for"
		<< " the name of the output files\n"        	
		<< "      --seed            Seed for the random numbers generator\n\n"        	
		<< "\n****************************************"
		<< "****************************************\n"
		<< std::endl;

}
////////////////////////////////////////////////////////////////////
// Frees memory
void clean_memory(){

	delete ALGO;
	delete PROBLEM;
	delete rnd;

}
////////////////////////////////////////////////////////////////////

