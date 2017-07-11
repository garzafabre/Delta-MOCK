# Delta-MOCK

**Delta-MOCK** is a new version of the *multiobjective clustering with automatic k-determination 
(MOCK)* algorithm. MOCK is an evolutionary approach to multiobjective data clustering, originally 
proposed by Julia Handl and Joshua Knowles [1]. Our new algorithm Delta-MOCK presents extensive 
changes and improves upon the effectiveness and computational efficiency of MOCK. This translates 
into a better scalability which is essential given the unprecedented volumes of data that require 
to be processed in current clustering applications.

Delta-MOCK is described in detail in our paper:

	Mario Garza-Fabre, Julia Handl and Joshua Knowles. 
	An Improved and More Scalable Evolutionary Approach to Multiobjective Clustering.
	IEEE Transactions on Evolutionary Computation (accepted for publication).
	
**Contact:**

	Mario Garza-Fabre - garzafabre@gmail.com
	Julia Handl - julia.handl@manchester.ac.uk
	Joshua Knowles - j.knowles@cs.bham.ac.uk

**References:**

	1.	Julia Handl and Joshua Knowles. An Evolutionary Approach to Multiobjective Clustering, 
		IEEE Transactions on Evolutionary Computation, vol. 11, no. 1, pp. 56–76, 2007.

---

**Compilation:**

- Please use the Makefile provided

---

**Execution:**

./delta_mock --file data_example/spiral_labels_headers.data --normalise false --algorithm nsga2 --population 100 --generations 100 --crossover 1.0 --mutation 1.0 --representation split --delta 80 --kmax 50 --output myresults/testrun --seed 1

---

**Input parameters:**


--file: full path to the dataset file (please refer to the description of the format of this file, provided below)

--normalise: this is to normalise the data (true or false)

--algorithm: nsga2 (this is the only algorithm currently implemented)

--population: size of the population

--generations: number of generations/iterations (stopping criterion)

--crossover: crossover probability

--mutation: mutation probability. The number given here is divided by the length of the genotype (n); thus, "--mutation 1.0" will set the mutation probability to 1/n.

--representation: three different problem representations available:
	
	locus: original full-length representation of MOCK
	
	short: reduced-length Delta-Locus representation (needs to set parameter --delta)
	
	split: reduced-length Delta-Binary representation (needs to set parameter –delta)

--delta: parameter required by the Delta-Locus and Delta-Binary representations

--kmax: maximum number of clusters to use during initialisation

--output: here we can define a path and/or a prefix for the name of the output files. In this example, "--output myresults/testrun" will save all output files in directory "myresults" and the name of all files will start with "testrun". NOTE: the program does not create directories, it assumes the provided path already exists

--seed: specific seed for the random numbers generator (optional)

---

**Input file:**

The input dataset file has to be formatted as follow:

	Line 1: Size of data set (N)
	Line 2: Dimensionality (d)
	Line 3: 1 or 0, depending on whether or not the real cluster labels (ground truth) are provided
	Line 4: Number of real clusters (only relevant if previous line is 1)
	Line 5: X1,1 X1,2 … X1,d LabelX1 
	...
	Line N+4: XN,1 XN,2 ... XN,d LabelXN 

where Xi,j refers to variable/dimension j of data instance i, and LabelXi refers to the corresponding reference label (this last column is to be only included if Line 3 is set to 1).

Please find example input files in directory "data_example".

---

**Output files:**

Even when the algorithm works with a population of P solutions, the algorithm reports at the end only the M<=P solutions which are the Pareto front approximation (the non dominated solutions). The algorithm produces the following files: 

- [PREFIX]measures.txt: a file that has M lines (one for each solution), and 4 columns providing performance measures evaluating the solution: (1) first objective: variance; (2) second objective: connectivity; (3) number of clusters; (4) adjusted rand index. The last column, adjusted rand index, is only included if real labels (ground truth) is provided.

- [PREFIX]solution_i.txt: a total of M files which contain the assignment of data points to clusters (predicted labels).



