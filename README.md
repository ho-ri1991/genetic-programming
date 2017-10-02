# genetic-programming
This repository is a framework of genetic programming in C++.
This framework mainly provides the expression of individual (i.e. tree structure) and evaluation of them (i.e. execution of tree).
The following properties are supported in the expression of individual:
- type-strict nodes
  - some default nodes are provided (ex. add, if, substitution, or, greater etc...)
- user defined nodes
  - derive NodeBase class and define evaluation.
- local variables
- substitution to variables (arguments and local variables)
- subroutines
  - recursions
  - reference passing
 
This framework also provides some default genetic operations (mutation and crossover and selection etc...) and IO of tree and training data. 

## requirements
- C++ compiler >= C++17
- boost >= 1.65

## getting started
