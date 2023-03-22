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

- Geospatial Formats
  - https://gdal.org/drivers/raster/index.html
  - @subpage geospatial_parser
  - @subpage geospatial_generator

- Genetic Format
  -
## Demographic Histories

### Growth Expressions

### Dispersal Kernels

### Memory Management


[//]: # (----------------------------------------------------------------------)
@page newick_parser Parsing a Newick tree string

@tableofcontents

## Introduction

### Background

Newick tree format is a way of representing graph-theoretical trees with edge
lengths using parentheses and commas.

Yet not very efficient, Newick is a simple and popular format for representing trees, and provides
a rather useful abstraction in coalescence theory for representing the shared history of populations
(species trees, population trees) or gene genealogies that coalesce into these
trees.

**Quetzal** provides some utilities to parse, generate and manipulate such format.

@note
To provide users with sensical defaults, **Quetzal** made to choice to generate
and manipulate graphs objects from the [Boost Graph Library](https://www.boost.org/doc/libs/1_81_0/libs/graph/doc/index.html) when not told otherwise by the user.
This choice is mostly practical and not hard coded, as **Quetzal** interfaces
still remain open-ended and offer the possibility to generate and manipulate
your own custom graph classes if required to.

### Grammar

Whitespace here refer to any of the following: spaces, tabs, carriage returns, and linefeeds.

- Whitespace  within number is prohibited.
- Whitespace outside of node names is ignored.
- Grammar characters (semicolon, parentheses, comma, and colon) are prohibited
- Comments are enclosed in square brackets.

### Objectives

In this tutorial section you will learn how to:
- Parse a Newick string to a **Quetzal** k-ary tree class,
- Customize the default **Quetzal** tree properties to describe your data and problem correctly,
- Parse a Newick string to your own custom tree class to limit ripple effects in your code base.

## To a Quetzal k-ary tree

### With default properties

In this example we will use the default (and simplistic) properties of the graph synthetized by the parsing.
By *properties* we mean the type of the data structures that describe vertices
and edges in a graph.

For general graphs it could be anything, but since we are parsing Newick formats,
we can reasonably expect *ad minima*:
- a `std::string` to describe a vertex (the *name* of the node)
- a `double` to describe an edge (the *distance to a parent node*).

**Quetzal** defaults to these minima when it tries to build a tree from a Newick string.

@remark
In the next sections we will see that you can actually parse into more complex data
structures - as long as vertices (*resp.* edges) remain constructible from a simple `std::string` (resp. `double`)!

**Input**

@include{lineno} newick_parser_1.cpp

**Output**

@include{lineno} newick_parser_1.txt

### With custom properties

In the previous example we used default types to represent the properties of a vertex and an edge.
These defaults are useful, but not always sufficient. For example, you may want to visit
your tree graph and trigger specific events when specific nodes or edges are visited.

In this case, you would need to define your own small structures to represent the graph properties you want:
- the vertex should be constructible from a `std::string`
- the edge should be constructible from a `double`
- the event would be to print to `std::cout` with a pretty display

@remarks
You can think of pretty much any type of events here:
- convert the branch length to coalescence units, generations, etc
- update a mutational state data member,
- construct a slightly modified copy of the tree while visiting its clone
- etc

This is surprisingly easy to inject your own classes into a graph!

**Input**

@include{lineno} newick_parser_2.cpp

**Output**

@include{lineno} newick_parser_2.txt

## To a custom k-ary tree

If you have been coding for a while, you may already depend heavily on your own
class to describe a tree graph, and not feel very excited about refactoring your whole
project to switch to **Quetzal** tree classes. Fair enough. This section shows how to use **Quetzal** parser to populate
your own custom class and then forget about **Quetzal**.

1. You will need to parse Newick strings into an AST (Abstract Syntax Tree): this
is a temporary data structure that represents the syntactic structure of the tree.
2. This AST can then be iterated over to convert it into the data structure of your choice.

@note
1. **Quetzal** does not know anything about your own classes, so you will need a bit of extra effort to write a recursion on the AST data
2. But at least you don't have to worry about the parsing logic anymore!
3. Adapting your own classes can be tedious, don't hesitate to ask for help
on the github issue pages!
4. On most compilers but Apple Clang, you could simplify the example code by writing: `subtree.left_child = std::make_unique<MyNode>(ast.name, ast.distance_to_parent);` in the recursion function.

**Input**

@include{lineno} newick_parser_3.cpp

**Output**

@include{lineno} newick_parser_3.txt


[//]: # (----------------------------------------------------------------------)
@page newick_generator Generating a Newick tree string

@tableofcontents

## Introduction

### Design choices

As we saw with Newick parser logic, it can be interesting to isolate independent behaviors,
as it makes Newick tree grammar reusable across different code bases even if
these codes made different choices concerning tree-like graphs implementation.

In the same logic, when generating a Newick string, **Quetzal** considers independently
1. the node data access logic, or *how to access the data stored in a node class*
2. the recursion logic, or *how to traverse a tree from the root to the tips*
3. the formatting logic, or *the grammar - that is the correspondence between the tree topology and the Newick string characters*

### Justification

Although you may find it requires a -small- effort to use this generator,
this decoupling allows to generalize the **Quetzal** generator over any class that
a user may have used to represent a tree in their own code bases - even classes
that **Quetzal** does'nt know anything about. Doing so, it hopefully saves the
community the time and effort to develop their own grammars and generators independently.

### Objectives

In this tutorial section you will learn how to:
- Generate a Newick string from a **Quetzal** k-ary tree class,
- Customize the behavior of the generator based on the **Quetzal** tree properties you used to describe your data and problem correctly,
- Generate a Newick string from your own custom tree class.

## From a Quetzal k-ary tree

**Input**

@include{lineno} newick_generator_1.cpp

**Output**

@include{lineno} newick_generator_1.txt

### With default properties

### With custom properties

## From a custom k-ary tree
