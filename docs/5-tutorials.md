@page tutorials Tutorials

## Input/Output

- Tree Format
  - @subpage newick_parser
  - @subpage newick_generator

- Network Format
  - @subpage extended_newick_parser
  - @subpage extended_newick_generator

- Demes Format
  - @subpage demes_parser
  - @subpage demes_generator

- Raster Format
  - https://gdal.org/drivers/raster/index.html

- Genetic Format
  -
## Demographic Histories

### Growth Expressions

### Dispersal Kernels

### Memory Management


[//]: # (----------------------------------------------------------------------)
@page newick_parser Newick tree parser

@tableofcontents

## Start

Newick tree format is a way of representing graph-theoretical trees with edge
lengths using parentheses and commas.

Yet not very efficient, Newick is a simple and popular format for representing trees, and provides
a rather useful abstraction in coalescence theory for representing the shared history of populations
(species trees, population trees) or gene genealogies that coalesce into these
trees.

Quetzal-CoaTL provides some utilities to parse and generate such format.

In this page we show how to parse Newick strings into an AST (Abstract Syntax Tree),
a temporary simplistic data structure that represents the syntactic
structure of the tree. This AST can then be converted into the data structure of
your choice for further processing.

## Grammar

Whitespace here refer to any of the following: spaces, tabs, carriage returns, and linefeeds.

- Whitespace  within number is prohibited.
- Whitespace outside of node names is ignored.
- Grammar characters (semicolon, parentheses, comma, and colon) are prohibited
- Comments are enclosed in square brackets.

## To a Quetzal k-ary tree

### Default properties

In this example we will use simplistic properties of the graph:
- a vertex (the *name* of the node) is represented by a simple `std::string`
- an edge (the *distance to a parent node*) is a simple `double`.

@remark
In the next sections we will see that you can actually parse into more complex data
structures - as long as they are constructible from a simple `std::string` and `double`!

@note
You can easily visualize the graphviz format printed in the output using the online editor https://edotor.net/

**Input**

@include{lineno} newick_parser.cpp

**Output**

@include{lineno} newick_parser.txt

### Custom properties




[//]: # (----------------------------------------------------------------------)
@page newick_generator Newick tree generator

To make Newick tree generator reusable, **quetzal** proposes to consider independently
1. the node data access logic, or *how to access the data stored in a node class*
2. the recursion logic, or *how to traverse a tree from the root to the tips*
3. the formatting logic, or *the grammar - that is the correspondence between the tree topology and the Newick string characters*

@note
Although it may require a -small- effort from the user to use this generator,
this decoupling allows to generalize the quetzal generator over any class that
a user may use to represent a tree in their program. Doing so, it saves them the
time and effort to develop their own generator.

## Using a Quetzal Tree class

## Using a user-defined Binary Tree

## Using a user-defined N-ary Tree
