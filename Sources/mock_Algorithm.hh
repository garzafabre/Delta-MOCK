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

#ifndef __MOCK_ALGORITHM_HH__
#define __MOCK_ALGORITHM_HH__

/******************
Dependencies
******************/
#include "mock_Algorithm.fwd.hh"
#include "mock_Global.hh"
#include "mock_Solution.hh"

/******************
Class definition
******************/
class Algorithm{

	/******************
	Attributes
	******************/

	protected:

		const string _algorithm_name;		

	public:

	/******************
	Methods
	******************/

	protected:		

		virtual void initialise() = 0; 						// Must be defined in derived class
		virtual void configure() = 0;						// Must be defined in derived class

	public:

		Algorithm( string n ) : _algorithm_name( n ){}
		virtual ~Algorithm(){}

		string name(){ return _algorithm_name; }
				
		virtual void run() = 0;								// Must be defined in derived class
		virtual void generate_output( int g = -1 ) = 0;		// Must be defined in derived class
};

#endif
