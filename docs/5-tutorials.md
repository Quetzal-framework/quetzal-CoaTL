@page tutorials Tutorials

## Input/Output

- Tree Format
  - @subpage newick_parser
  - @subpage newick_generator

- Network Format
  - @subpage extended_newick_parser
  - @subpage extended_newick_generator

- Geospatial Formats
  - @subpage geotiff_parser
  - @subpage geotiff_generator
  - @subpage shapefile_generator

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
@page newick_parser Parsing a Newick string

@tableofcontents

## Introduction

--- 

### Background

Newick tree format is a way of representing graph-theoretical trees with edge
lengths using parentheses and commas.

Yet not very efficient, Newick is a simple and popular format for representing trees, and provides
a rather useful abstraction in coalescence theory for representing the shared history of populations
(species trees, population trees) or gene genealogies that coalesce into these
trees.

**Quetzal** provides some utilities to parse, generate and manipulate such format.

--- 

### Grammar

Whitespace here refer to any of the following: spaces, tabs, carriage returns, and linefeeds.

- Whitespace  within number is prohibited.
- Whitespace outside of node names is ignored.
- Grammar characters (semicolon, parentheses, comma, and colon) are prohibited
- Comments are enclosed in square brackets.

--- 

### Objectives

In this tutorial section you will learn how to:
- Parse a Newick string to a **Quetzal** k-ary tree class,
- Customize the default **Quetzal** tree properties to describe your data and problem correctly,
- Parse a Newick string to your own legacy tree class

----------------------------------------------------

## To a Quetzal k-ary tree

--- 

### Default properties

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

--- 

### Custom properties

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

----------------------------------------------------

## Interfacing legacy code

If you have been coding for a while, you may already depend heavily on your own
class to describe a tree graph, and not feel very excited about refactoring your whole
project to switch to **Quetzal** trees. Fair enough. 

This section shows how to use **Quetzal** parser to populate
your own legacy class and then forget about **Quetzal**.

1. You will need to parse Newick strings into an AST (Abstract Syntax Tree): this
is a temporary data structure that represents the syntactic structure of the tree.
2. This AST can then be iterated over to convert it into the data structure of your choice.

@note
1. **Quetzal** does not know anything about your own classes, so you will need a bit of extra effort to write a recursion on the AST data
2. But at least you don't have to worry about the parsing logic anymore!
3. If the task becomes tedious, don't hesitate to [ask for help](https://github.com/Quetzal-framework/quetzal-CoaTL/issues) !
4. On most compilers but Apple Clang, you could simplify the example code by writing: `subtree.left_child = std::make_unique<MyNode>(ast.name, ast.distance_to_parent);` in the recursion function.

**Input**

@include{lineno} newick_parser_3.cpp

**Output**

@include{lineno} newick_parser_3.txt


[//]: # (----------------------------------------------------------------------)
@page newick_generator Generating a Newick string

@tableofcontents

## Objectives

In this tutorial section you will learn how to:
- Use the `quetzal::format::newick::generate_from` function to generate a Newick string
    - from a **Quetzal** coalescence binary tree.
    - from a **Quetzal** coalescence k-ary tree.
- Generate a Newick string from your own (non-Quetzal) legacy tree class.
- Customize the behavior of the generator based on the type of information (properties) you wish to format.

----------------------------------------------------

## From a Quetzal binary tree

--- 

### No property

When a Newick string is generated from a tree that has no vertex nor edge information/properties
attached to it, it is then assumed the only interest is the tree topology: as there is no clear 
way to populate the labels or branch length data fields in the Newick string, those are left empty.

**Input**

@include{lineno} newick_generator_1.cpp

Since there are no comments stored in the labels of the generated random tree, 
the output of the different flavors are here quite similar:

**Output**

@include{lineno} newick_generator_1.txt

--- 

### Custom properties

When a Newick string is generated from a tree that has some information attached to its vertices and edges
through a property class, the formatter can acccess this information as long as the property class 
defined a `std::string label() const` method.

**Input**

@include{lineno} newick_generator_2.cpp

**Output**

@include{lineno} newick_generator_2.txt

----------------------------------------------------

## From a Quetzal k-ary tree

Extending the string generation to a k-ary tree is straightforward: 
instead of passing a `quetzal::coalescence::binary_tree` object, just pass a 
`quetzal::coalescence::k_ary_tree` to the `quetzal::format::newick::generate_from` function.

--- 

### No property

No vertex nor edge properties are embedded: the vertices labels and branch length data fields of the Newick string are left empty.

**Input**

@include{lineno} newick_generator_3.cpp

Since there are no comments stored in the labels of the generated random tree, 
the output of the different flavors are here quite similar:

**Output**

@include{lineno} newick_generator_3.txt

--- 

### Custom properties

The Newick string is generated from a tree that embeds some information attached to its vertices and edges
through a property class,
the formatter can acccess this information as long as the property class defined a `std::string label() const` method.

**Input**

@include{lineno} newick_generator_4.cpp

**Output**

@include{lineno} newick_generator_4.txt

----------------------------------------------------

## Interfacing legacy code

If your existing code heavily dependson your own class to describe a tree graph, then refactoring your whole
project to switch to **Quetzal** trees may be too costly. 

In this case, you can connect the `quetzal::format::newick::generator` class to your own legacy code.

@note
1. **Quetzal** does not know anything about your own classes, so you will need a bit of extra effort to make the  `quetzal::format::newick::generator` aware of your own design.
2. But at least you don't have to worry about the grammar logic anymore: if you can connect the dots the generator 
internals will handle the Newick string generation.
3. If the task becomes tedious, don't hesitate to [ask for help](https://github.com/Quetzal-framework/quetzal-CoaTL/issues)!

--- 

### No property

 **Quetzal** implements a `quetzal::format::newick::generator` that consumes a tree graph and produces a newick string.
 
This generator is not aware of your class. Instead, it requires you to defined 4 functors that embed all the information there is to know about a vertex \f$v\f$:
 - `has_parent(v)`: returns `false` if \f$v\f$ is the root, `true` otherwise.
 - `has_children(v)`: returns `false` if \f$v\f$ is a leaf, `true` otherwise.
 - `label(v)`: returns a (possibly empty) `std::string` used as label for vertex \f$v\f$
 - `branch_length_to_parent(v)`: returns a (possibly empty) `std::string` used as label for the edge between \f$v\f$ and its parent.

**Input**

@include{lineno} newick_generator_5.cpp

**Output**

@include{lineno} newick_generator_5.txt

--- 

### Custom properties

Once you understand the previous example with no property, then generating a Newick string from one of your legacy tree 
class object with added vertices and/or edge labels is pretty straightforward. 

You only need to modify three things:
- Your class somehow embeds a data field
- the `label(v)` lambda looks into the vertex to return a `std::string` used as its label
- the `branch_length_to_parent(v)`: can also look into the vertex to return a `std::string` to label the edge between \f$v\f$ and its parent.

@include{lineno} newick_generator_6.cpp

**Output**

@include{lineno} newick_generator_6.txt

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

## Depth First Search (DFS)  Algorithms

DFS is a form of graph traversal referering to the process of visiting (e.g. retrieving, updating, or deleting) each node in a tree data structure, exactly once.

## DFS on trees

When the graph has the structure of a tree, there are only three operations that are worth
worrying about:
- `pre-order`, 
- `in-order`, 
- `post-order`.  

\image html dfs_tree.png width=33%

When you perform a DFS on a tree with *Quetzal* you need to pass to the DFS method a starting vertex (probably the root of the tree) and a visitor object: you would write `tree.depth_first_search(start, visitor)`. This visitor must be a cheap-to-copy instance of a class that implements the details of the DFS operations as decided by the user, for example:

```cpp
template <typename Order, typename Vertex>
struct tree_visitor
{
  void operator()(Order stage, Vertex v)
  {
    switch(stage) {
      case boost::visit::pre :
        // this line can be whatever the user sees fit
        std::cout << "Pre " << v << std::endl;
        break;
      case boost::visit::in:
        std::cout << "In " << v << std::endl; // same
        break;
      case boost::visit::post:
        std::cout << "Post " << v << std::endl; // same
        break;
      default:
        throw std::invalid_argument( "received invalid DFS order" );
    }
  }
};
```
You could also capture a reference to the tree graph instance if you need it:

```cpp
template <typename Graph, typename Order>
struct tree_visitor
{
  Graph& _tree;

  void operator()(Order stage, Vertex v)
  {
    switch(stage) {
      case boost::visit::pre :
        // this line can be whatever the user sees fit
        std::cout << "Pre " << _tree[v] << std::endl;
        break;
      case boost::visit::in:
        std::cout << "In " <<_tree[v] << std::endl; // same
        break;
      case boost::visit::post:
        std::cout << "Post " << _tree[v] << std::endl; // same
        break;
      default:
        throw std::invalid_argument( "received invalid DFS order" );
    }
  }
};
```
     
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

[//]: # (----------------------------------------------------------------------)
@page geotiff_parser Reading spatial variables from rasters

@tableofcontents

## Reading a single variable

Landscapes and their spatial/temporal heterogeneity affect the dynamics of populations and lineages.
These landscapes are often incorporated into computational models as geospatialized grids, or rasters.

*Quetzal* provides a `quetzal::geography::raster` class with a small interface to integrate these spatial 
grids into a georeferenced coalescence simulation.

With `quetzal::geography::raster` you can define a single variable that exhibits:
- spatial heterogeneity: rasters delineate a geographic space into a number of cells identified by their row/columns.
- temporal heterogeneity: raster have a *depth*, *i.e.* multiple bands (layers) that are used to model the temporal scale.

When you read a variable with the `from_file()` function you will need to think about a few things:
- you need to decide what the raster template argument `time_type` is. In most cases that will be an integer mapping a band to
  a year (e.g 2023 for the last band), but you could chose to make it a slightly more complex type like a time period (e.g. 7000–3000 BCE).
- keep in mind that you don't need to duplicate identical bands (what could make the raster heavy): you can virtualize repeated bands with the VRT format.
- there are multiple ways to identify locations in a spatial grid: which way is best depends on the usage context. The `quetzal::geography::raster` interface allows to switch between them:
  - `lonlat` or `latlon`: decimal longitude and latitude values, that may or may not be contained in the spatial extent of the grid. 
    Users inputs such as genetic samples will often been given as such, and will need to be reprojected into a `location_descriptor`
  - `colrow` or `rowcol`: the most intuitive way to index the cells of a grid.
  - `location_descriptor` is a 1-dimensional index of the grid cells. You can think of it has the way to map  \f$0\f$ to the grid origin (top left cell) \f$ width \times height - 1\f$ to the bottom right cell. Since it's just an integer, this 1D system is cheap to copy and is meant to used in intensive simulations. Converting a `location_descritptor` to a `latlon` or `lonlat` will return the coordinates of the cell centroid. See also `to_centroid`.
- When the value of the variable at a given location is read, it may exist or not (in which case it is a NA). In that last case, an empty optional is returned. The actual value of a NA as represented in the dataset is given by the `NA()` function.

**Input**

@include{lineno} geography_raster_1.cpp

**Output**

@include{lineno} geography_raster_1.txt

## Reading multiple variables

If you are working with multiple variables (*e.g.* a suitability raster and an elevation raster), you could run into 
simulation problems if the two rasters have even slightly different grid properties, like different resolutions, origins or extent. 
To prevent these problems from happening, you can use `quetzal::geography::landscape` that wraps multiple rasters into
one single coherent object and checks that all rasters are aligned.

**Input**

@include{lineno} environmental_variable_1.cpp

**Output**

@include{lineno} environmental_variable_1.txt

## What to do after

Geospatial datasets are useful in coalescence to inform the demographic process. There are mainly
3 things you may want to do once you successfully parsed a raster:

----------------------------------------------------

### Use the grid structure to define a spatial graph

Rasters are used to build a fully-connected spatial graph that will be used to simulate the demographic process.
- Vertices are the demes (populations) represented by the centroid of the raster cells.
- Edges are the distance between demes. Their connectivity can be altered by dispersal kernels.

----------------------------------------------------

### Use Digital Elevation Models to define a spatial graph

Digital Elevation Models can also be used to link the dynamic of sea levels to the dynamic of species.
In that case the connectivity of the graph will be a function of the elevation.

----------------------------------------------------

### Use the environmental information to inform processes

Ecological niche models (ENMs) are an important tool to model likely responses of populations to past and future climatic variations.
In most case, the output of these analysis is a raster referencing suitability estimates. In turn the suitability
coefficient is used to inform growth rates and carrying capacity.

The values of the raster can be used to inform the demographic quantities by composing mathematical expressions
of time and space (see `quetzal::expressive`).

NA values (e.g. oceanic cells or beyond the raster spatial extent) are manipulated through `std::optional`: they may 
or not have a value, and it's up to the user to decide what to do when they don't.

**Input**

@include{lineno} environmental_variable_1.cpp

**Output**

@include{lineno} environmental_variable_1.txt


[//]: # (----------------------------------------------------------------------)
@page geotiff_generator Writing spatial variables to rasters

[//]: # (----------------------------------------------------------------------)
@page shapefile_generator Writing spatial samples to shapefile