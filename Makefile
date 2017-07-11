################################################################################
# Copyright (C) 2017 Mario Garza-Fabre, Julia Handl, Joshua Knowles
#
# This file is part of Delta-MOCK.
#
# Delta-MOCK is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Delta-MOCK is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Delta-MOCK. If not, see <http://www.gnu.org/licenses/>.
#
# ------------------------------------------------------------------------------
#
# Author: Mario Garza-Fabre (garzafabre@gmail.com)
# Last updated: 30 June 2017
#
################################################################################

CC = g++

# debugging/valgrind
# CFLAGS = -std=c++11 -O0 -g 

# release executable
CFLAGS = -std=c++11 -O3

TARGET = delta_mock
OBJ = 	mock.o mock_Util.o mock_ClusteringProblem.o mock_Clustering.o mock_SolutionLocus.o \
		mock_SolutionShort.o mock_SolutionSplit.o mock_Population.o mock_EvaluatorFull.o \
		mock_BinaryOperator.o mock_UnaryOperator.o mock_Nsga2.o mock_EvaluatorDelta.o

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.cc
	$(CC) $(CFLAGS) -c $^

clean: 
	rm -f $(TARGET)	
	rm -f *.o	


