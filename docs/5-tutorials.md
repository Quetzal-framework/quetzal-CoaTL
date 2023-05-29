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

## Graphs

  - @subpage graphs_in_quetzal
  - Coalescence 
    - @subpage coalescence_binary_tree
    - @subpage coalescence_k_ary_tree
    - network

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
By *properties* we mean the type the data structures used to store arbitrary information along the vertices
and edges of a graph.

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
- etc ...

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

### Objectives

In this tutorial section you will learn how to:
- Generate a Newick string from a **Quetzal** coalescence binary tree.
- Generate a Newick string from a **Quetzal** coalescence k-ary tree.
- Customize the behavior of the generator based on the type of information you wish to format.

## From a Quetzal binary tree

### With no property

When a Newick string is generated from a tree that has no vertex nor edge information/properties
attached to it, it is then assumed the only interest is the tree topology: as there is no clear way to populate the labels or branch length data fields in the Newick string, those are left empty.

**Input**

@include{lineno} newick_generator_1.cpp

Since there are no comments stored in the labels of the generated random tree, 
the output of the different flavors are here quite similar:

**Output**

@include{lineno} newick_generator_1.txt

### With custom properties

When a Newick string is generated from a tree that has some information attached to its vertices and edges
through a property class,
the formatter can acccess this information as long as the property class defined a `std::string label() const` method.

**Input**

@include{lineno} newick_generator_2.cpp

**Output**

@include{lineno} newick_generator_2.txt

## From a custom k-ary tree

[//]: # (----------------------------------------------------------------------)
@page graphs_in_quetzal Graphs in Quetzal

@tableofcontents

Although graphs are ubiquitous in phylogeography and coalescence frameworks,
they can represent quite different objects depending on the exact context.
Consequently Quetzal had to make a number of design choices in their implementation.

## Invariants Guarantee

As geneticists, we expect from a simple Kingman coalescent tree to exhibit certain characteristics
(for example the guarantee of having either 0 or 2 children but never only 1)
that we don't expect from a phylogenetic network (that would in contrast exhibit cycles 
and hybrid nodes). 

In programming jargon, these expectations are called *class invariants*. 

Rather than exposing a 1-fit-all type of graph and be prone to programming mistakes, 
Quetzal comes with more specialized classes that maintain their invariants.

## Edge and Vertices descriptors

A point common to all class of graphs is that vertices and edges have unique
identifiers. 

This identifier is implementation-dependent and not always of type `int`. 
It is instead referred by a type nested in each graph class: `vertex_descriptor` and `edge_descriptor`.

Methods of a class that receive or return specific edges/vertices will manipulate objects of this type.

## Edge and Vertices Information (Property Classes)

The quality of information attached to the vertices and edges of a graph also varies depending on the user problem.

For example, a spatial coalescent tree may need a way to store the geographic 
location of the visited locations, whereas a Kingman coalescent tree does not need to allocated storage 
for this. Similarly, Phylogenetic Networks will surely embed completely different types of information, 
like \f$\gamma\f$, the inheritance probability.

To reflect this variability in the type of information, graphs can embed arbitrary information along edges and vertices 
through small user-defined classes called *property classes*.

The type of information a graph class embeds is referred by a nested type: `vertex_property` and `edge_property`.

[//]: # (----------------------------------------------------------------------)
@page coalescence_binary_tree Binary Trees
@tableofcontents

# Introduction

Coalescence Binary Trees classes provide a way to guarantee that each vertex \f$v\f$ has exactly 
  - 0 successor (the vertex is a leaf)
  - 2 successors (the vertex is an internal node)

The graph structure is bidirectional and allows to model a forest of disconnected trees.
That is, if a vertex has no predecessor, then it's either a root (if it has 2 successors) 
or an isolated vertex (if it has no successor).

Users will encounter this type of tree when returning from coalescence binary mergers, 
or if they simply want to build and manipulate graphs while enforcing a binary tree 
structure.

# Implementations 

Coalescence Binary Trees can store an additional arbitrary type of information 
(called a *Property Class*) along their vertices or edges: if the user decides to do so, 
the tree class interface adapts to offer ways
to manipulate this information.

There are logically 4 different classes (equivalently interfaces) resulting from this: 
- A class to represent a simple topology with no additional information (no property): <br>
  \ref CoalescenceBinaryTreeNoPropertyNoProperty "quetzal::coalescence::binary_tree< no_property, no_property >"
- A class to embed a user-defined class `VertexProperty` on each vertex: <br>
  \ref CoalescenceBinaryTreeVertexPropertyNoProperty "quetzal::coalescence::binary_tree< VertexProperty, no_property >"
- A class to embed  a user-defined class `EdgeProperty` on each edge: <br>
  \ref CoalescenceBinaryTreeVertexPropertyNoProperty "quetzal::coalescence::binary_tree< boost::no_property, EdgeProperty >"
- A class to embed  both `VertexProperty` and `EdgeProperty` information :<br>
  \ref CoalescenceBinaryTreeVertexPropertyEdgeProperty "quetzal::coalescence::binary_tree< VertexProperty, EdgeProperty >"

Usage examples are described in their respective class documentation.

[//]: # (----------------------------------------------------------------------)
@page coalescence_k_ary_tree K-ary Trees
@tableofcontents


# Introduction

Coalescence k-ary Trees classes are similar to Binary Trees, but provide an interface guaranteeing that each vertex \f$v\f$ has exactly 
  - \f$0\f$ successor (the vertex is a leaf)
  - \f$k \geq 2\f$ successors (the vertex is an internal node)

The graph structure is bidirectional and allows to model a forest of disconnected trees.
That is, if a vertex has no predecessor, then it's either a root (if it has 2 or more successors) 
or an isolated vertex (if it has no successor).

Users will encounter this type of tree when returning from coalescence multiple mergers, 
or if they simply want to build and manipulate graphs that do not make strong assumptions on
the tree structure.

# Implementations 

Coalescence K-ary Trees can store an additional arbitrary type of information 
(called a *Property Class*) along their vertices or edges: if the user decides to do so, 
the tree class interface adapts to offer ways
to manipulate this information.

There are logically 4 different classes (equivalently interfaces) resulting from this: 
- A class to represent a simple topology with no additional information (no property): <br>
  \ref CoalescenceKaryTreeNoPropertyNoProperty "quetzal::coalescence::k_ary_tree< no_property, no_property >"
- A class to embed a user-defined class `VertexProperty` on each vertex: <br>
  \ref CoalescenceKaryTreeVertexPropertyNoProperty "quetzal::coalescence::k_ary_tree< VertexProperty, no_property >"
- A class to embed  a user-defined class `EdgeProperty` on each edge: <br>
  \ref CoalescenceKaryTreeNoPropertyEdgeProperty "quetzal::coalescence::k_ary_tree< boost::no_property, EdgeProperty >"
- A class to embed  both `VertexProperty` and `EdgeProperty` information :<br>
  \ref CoalescenceKaryTreeVertexPropertyEdgeProperty "quetzal::coalescence::k_ary_tree< VertexProperty, EdgeProperty >"

Usage examples are described in their respective class documentation.