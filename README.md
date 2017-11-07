# genetic-programming
This repository is a framework of genetic programming in C++.
This framework mainly provides the expression of individual (i.e. expression tree) and evaluation of them (i.e. execution of expression trees).
The following properties are supported in the expression tree:
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
- C++ compiler and standard library >= C++17
- boost >= 1.62

I have compiled by g++7.2.0 with boost 1.62.0

## getting started
