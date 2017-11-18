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

An example of representation of a tree is the following XML form:
```
<?xml version="1.0" encoding="utf-8"?>
<tree>
    <name>Fact[int]</name>
    <return_type>int</return_type>
    <arguments>
        <type>int</type>
    </arguments>
    <local_variables>
    </local_variables>
    <tree_entity>
        +--If[int]
            |
            +--Greater[int]
            |   |
            |   +--Argument[int,0]
            |   |
            |   +--Const[int,1]
            |
            +--Mult[int]
            |   |
            |   +--Argument[int,0]
            |   |
            |   +--Fact[int]
            |       |
            |       +--Sub[int]
            |           |
            |           +--Argument[int,0]
            |           |
            |           +--Const[int,1]
            |
            +--Const[int,1]


    </tree_entity>
</tree>

```

Of course, you need not write the tree entity field like the upper example.
The format is more loose.
- the node name start with the header `+--` and not contain white space
- there must be zero or one node name with header
- if the node header `+--` is not found, the line is ignored   

The following format is also fine (the same tree as the upper example):
```
<?xml version="1.0" encoding="utf-8"?>
<tree>
    <name>Fact[int]</name>
    <return_type>int</return_type>
    <arguments>
        <type>int</type>
    </arguments>
    <local_variables>
    </local_variables>
    <tree_entity>
+--If[int]
+--Greater[int]
+--Argument[int,0]
+--Const[int,1]
+--Mult[int]
+--Argument[int,0]
+--Fact[int]
+--Sub[int]
+--Const[int,1]
+--Const[int,1]

    </tree_entity>
</tree>

```

You can load and execute this tree by using example/tree_executor.cpp.

## requirements
- C++ compiler and standard library >= C++17
- boost >= 1.62

We have compiled by g++7.2.0 with boost 1.62.0

## getting started

This framework is a header only template library.
Therefor, you can use this just including header files.

### build examples and tests
We use cmake to build, execute following commands to build.
If you want to run tests execute `ctest` after the build.

```
$ git clone https://github.com/ho-ri1991/genetic-programming.git
$ cd  genetic-programming
$ mkdir build
$ cmake ..
$ make
```

Then, you will find executable in the `example1` directory.
You can run the genetic_programming by executing `./eexample1/example1 ${YOUR_PROBLEM_FILE1} ${YOUR_PROBLEM_FILE2} ...`.
The `example1` load each problem file and solve it sequentially.
After solving problems, `example1` write 10 trees in order of the fitness. 
See `genetic_programing/example1/problem_.*.xml` for example problem format.
`example1/tree_executor` load tree and execute it.
