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

## Linking landscapes to local demographic parameters

- @subpage niche_in_quetzal
- Examples 
  - @subpage expressive_suitability_raster
  - @subpage expressive_suitability_DEM_landscape
  - @subpage expressive_friction

## Demographic Histories

- @subpage demographic_histories_in_quetzal
- Examples
  - @subpage Growth Expressions
  - @subpage Dispersal Kernels
  - @subpage Memory Management

## Graphs

- @subpage graphs_in_quetzal
- Coalescence 
  - @subpage coalescence_binary_tree
  - @subpage coalescence_k_ary_tree
  - network






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

**Input**

@include{lineno} newick_generator_6.cpp

**Output**

@include{lineno} newick_generator_6.txt

[//]: # (----------------------------------------------------------------------)
@page extended_newick_parser Parsing an Extended Newick string

Extended Newick format expands beyond the traditional Newick notation by accommodating explicit phylogenetic networks. This extension enables the encoding of complex relationships that go beyond the simple hierarchical structure of phylogenetic trees. 

In phylogenetic networks, nodes can signify either divergence events (cladogenesis) or reticulation events, such as hybridization, introgression, horizontal gene transfer, or recombination. 

To represent reticulation events, the nodes are duplicated and annotated by appending the `#` symbol within the Newick format. These duplicated nodes are sequentially numbered, starting from 1, to maintain clarity and organization within the representation.

**Input**

@include{lineno} newick_extended_parser_1.cpp

**Output**

@include{lineno} newick_extended_parser_1.txt

[//]: # (----------------------------------------------------------------------)
@page extended_newick_generator Generating an Extended Newick string

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

Landscapes, along with their spatial and temporal heterogeneity, play a significant role in shaping the dynamics of populations and lineages. In computational models, these landscapes are often represented as geospatial grids or rasters.

In most analyses, it is common for users to establish connections between local growth processes and a habitability raster obtained from Ecological Niche Models. Additionally, they may associate dispersal or connectivity with a Digital Elevation Model.

The Quetzal library offers a quetzal::geography::raster class with a streamlined interface to incorporate these spatial grids into georeferenced coalescence simulations.

By utilizing quetzal::geography::raster, you can define a single variable that encompasses:

- Spatial heterogeneity: Rasters divide a geographical space into cells identified by their row and column coordinates.
- Temporal heterogeneity: Rasters have a depth, meaning multiple bands (layers) that are utilized to model the temporal dimension.

When reading a variable using the `from_file`() function, there are a few considerations to keep in mind:

1. You need to decide on the raster template argument `time_type`. In most cases, it would be an integer representing a band mapped to a year (e.g., 2023 for the latest band). However, you can opt for a more complex type such as a time period (e.g., 7000–3000 BCE).
2. Keep in mind that duplicating identical bands is unnecessary and can make the raster bulky. Instead, you can virtualize repeated bands using the VRT (Virtual Dataset) format. See the Python **Quetzal-CRUMBS** helper library to do so.
3. There are multiple ways to identify locations within a spatial grid, and the choice depends on the specific usage context. The `quetzal::geography::raster` interface allows for switching between them:
  - `lonlat` or `latlon`: Decimal longitude and latitude values, which may or may not fall within the spatial extent of the grid. Genetic samples and other user inputs will generally use these real-world coordinate formats and may require projection into a `location_descriptor`.
  - `colrow` or `rowcol:` The most intuitive way to index the cells of a grid.
  - `location_descriptor`: A one-dimensional index representing the grid cells. It can be thought of as mapping 0 to the top-left cell (grid origin) and width × height - 1 to the bottom-right cell. Since it's a simple integer, this 1D system is computationally efficient and designed for intensive simulations. Converting a `location_descriptor` to `latlon` or `lonlat` will provide the coordinates of the cell centroid. The `to_centroid` function is also available for this purpose.
4. When reading the value of a variable at a specific location, it may or may not exist. In the latter case, it is considered a NA (Not Available) value, and an empty optional is returned. The actual representation of NA in the dataset can be obtained using the `NA()` function.

**Input**

@include{lineno} geography_raster_1.cpp

**Output**

@include{lineno} geography_raster_1.txt

---

## Reading multiple variables

Aligning rasters refers to a spatial data representation technique used in Geographic Information Systems (GIS). In this
context, raster data layers are aligned and registered to a common coordinate system and grid structure. This ensures that the cells or pixels in the raster layers are spatially aligned and correspond to the same geographic locations.

Alignment is achieved in `quetzal::geography::raster` objects by defining a consistent grid structure, such as a uniform cell size and orientation, for all raster layers.

In situations where multiple GIS variables that change over time are involved (such as a suitability and an elevation across geological times), an additional alignment mechanism is required. The `quetzal::geography::landscape` class addresses this need by combining multiple raster datasets, each potentially having multiple layers, into a single cohesive object. It ensures that all spatial grids are properly aligned and maintains the temporal dimension represented by multiple layers.  This allows for accurate overlay, simulation, and composition of multiple datasets within the GIS environment.

**Input**

@include{lineno} geography_landscape_1.cpp

**Output**

@include{lineno} geography_landscape_1.txt

---

## Next steps

Geospatial datasets play a crucial role in coalescence by providing valuable information about the demographic process. Once a raster is successfully parsed, users typically have three primary objectives they may want to pursue:

---

### Demes on a regular spatial grid

The rasters grid structure can be employed to construct a spatial graph of demes that is fully connected and utilized for simulating the demographic process. This is pertinent when looking at very larged continuous land masses. In this context:

- Vertices correspond to demes (populations) and are represented by the centroids of the raster cells.
- Edges represent the distances between demes, and their connectivity can be modified using dispersal kernels.

---

### Demes on a variable spatial graph

In archipelagos or clusters of loosely connected islands, the fluctuations in sea levels have a notable impact on the dynamics of species. These variations cause the intermittent connection and separation of land masses and their corresponding populations. Similarly, climatic shifts in sky-island complexes can have similar effects. As a result, the assumption of a completely interconnected graph representing population units (demes) is no longer applicable in these situations.

To address this, Digital Elevation Models (DEMs) can be utilized to establish a relationship between the dynamics of sea levels and species. In these cases, the connectivity of the graph at any given time is determined by the current elevation, and changes in elevation directly impact the level of connectivity between different areas.

---

### Inform local processes

Ecological Niche Models (ENMs) play a crucial role in predicting how populations might respond to changes in climate, both in the past and the future.

Typically, these models produce raster outputs that represent estimates of suitability. These suitability values are then utilized to determine growth rates and carrying capacity.

By utilizing the values within the raster, it becomes possible to derive demographic quantities through the construction of mathematical expressions that incorporate both time and space. This approach is exemplified in the `quetzal::expressive` framework.

When encountering NA values, such as cells representing oceanic areas or those lying outside the spatial extent of the raster, the `std::optional` mechanism is employed. This means that these values may or may not have a defined value, leaving it to the user to decide how to handle such cases.


[//]: # (----------------------------------------------------------------------)
@page geotiff_generator Writing spatial variables to rasters

Once users have simulated various continuous geospatial values such as friction coefficients or population sizes across a landscape, they will often desire a visual confirmation of these values. One approach is to export these quantities as a multiband raster file and utilize different software, such as Quetzal-CRUMBS or the `raster` package in R, for visualization.

To export a specific quantity, it is necessary to create a callable function that can be invoked at each location within the raster model during an arbitrary time interval. Each point of the time interval parameter will become a new band in the new raster file.

**Input**

@include{lineno} geography_write_quantity_1.cpp

**Output**

@include{lineno} geography_write_quantity_1.txt

[//]: # (----------------------------------------------------------------------)
@page shapefile_generator Writing spatial samples to shapefile

Users sometimes simulate quantities that do not always correspond to continuous spatial values. Instead, these quantities can be discrete and result from a sampling process across the landscape. In such cases, users often want to visually verify the sampling scheme.

One way to achieve this is by exporting the samples as a shapefile and employing various software tools like Quetzal-CRUMBS or the raster package in R to visualize them.

**Input**

@include{lineno} geography_write_sample_1.cpp

**Output**

@include{lineno} geography_write_sample_1.txt


[//]: # (----------------------------------------------------------------------)
@page niche_in_quetzal Niche models in Quetzal
@tableofcontents

# Niche models as mathematical expressions

In the field of ecology, the **niche** refers to the compatibility between a species and a specific environmental condition. It explains how an organism or a population adapts to the availability of resources and competition. 

For instance, when resources are plentiful and the presence of predators, parasites, and pathogens is low, the organism or population tends to grow. In locations and times of resources scarcity and high predation, the populations tend to shrink.

The composition and significance of environmental variables that constitute the dimensions of a niche can vary among species, and the importance of specific environmental factors for a species may change depending on geographical and ecological contexts.

@note
In the broader context of this library, the niche model lacks a specific definition as it relies on the preferences and requirements of individual users. What we can understand is that it involves some combination of spatial and temporal factors that are closely intertwined with the demographic process.
This justifies the need for a highly adaptable approach for representing the concept of niche, as it is bound to vary depending on the specific species, population, or geographic area under study. 

Given this understanding, the primary goal of **Quetzal** is to provide users with `quetzal::expressive`, a user-friendly means to effectively combine geospatial and temporal elements. This enables users to establish meaningful mathematical connections between a diverse and heterogeneous landscape and the underlying demographic processes, based on their own perspectives and preferences. Generally speaking, users will want to use `quetzal::expressive` to mathematically compose spatio-temporal expressions with a signature `double (location_type, time_type)` until they reach a satisfying definition of the local demographic parameter of interest.

# Mathematical composition in C++ 

Mathematical composition in C++ is not trivial as unlike languages, C++ does not understand mathematical expressions and their composition. In this strongly-typed language, you can not simply multiply the integer `42` and the function \f$ f(x,y) = x^y \f$ and expect it to work. Indeed, if you try to rescale a lambda function, e.g.:
```cpp
int a = 42;
auto f = [](auto x, auto y){return std::pow(x,y); };
my_expression = a * f; // error
``` 
it will not compile. As the compiler does not know natively what the rules are to multiply a number with whatever type `f` is, it will (verbosely) insult you saying the [`operator*()`](https://en.cppreference.com/w/cpp/language/operators) is not defined for the type `int` and an anonymous type (the lambda function). 

# Mathematical composition with quetzal::expressive

You need a library to tell the compiler what to do. This is where `quetzal::expressive` comes handy: it allows you to add, substract, multiply, or compose mathematical expressions by giving them a uniform interface.

In the context of demographic models, `my_expression` in the example below could represent for example `r(x,t)` the growth rate at location `x` at time `t`. Being able to compose several smaller expressions into bigger expressions gives much freedom to users who can for example define `r` in one part on the program and compose it at a later time with a different expression *e.g.* `k(x,t)` the carrying capacity at location `x` at time `t`. By treating these local demographic parameters as independent expressions that can be mixed and matched, `quetzal::expressive` allows great freedom in the implementation details of a spatio-temporal process.

**Input**

@include{lineno} expressive_1.cpp

**Output**

@include{lineno} expressive_1.txt

---

[//]: # (----------------------------------------------------------------------)
@page expressive_suitability_raster Preparing a suitability landscape for the simulation

@note 
The objective of this section is to load a suitability map with `quetzal::geography::raster` and prepare its integration into the simulation framework with `quetzal::expressive`.

In the context of ecological niche modeling (ENM), suitability refers to the degree of appropriateness or favorability of a particular environmental condition for a species to thrive and persist. ENM is a technique used in ecology to predict the distribution of species across landscapes based on their known occurrences and environmental variables.

Suitability can be thought of as a spectrum, ranging from conditions that are highly suitable for a species' survival and reproduction to conditions that are unsuitable or even detrimental. ENM attempts to map this spectrum across geographical space by analyzing the relationships between species occurrences and various environmental factors, such as temperature, precipitation, elevation, soil type, and vegetation cover.

It's important to note that suitability does not solely depend on a single environmental variable. Instead, it's a complex interplay of multiple factors that determine whether a species can survive, reproduce, and establish a viable population in a given area.
Suitability GIS maps derived from previous analysis steps (see e.g. the niche models in **Quetzal-crumbs** python package) can then be integrated into the coalescence simulation framework.

The following example code 
- loads a suitability map using `quetzal::geography::raster` 
- prepares its integration into the simulation framework using `quetzal::expressive` 
- it gives to the suitability the semantic of a function of space and time `f(x,t)`
- it gives it access to mathematical operators with `quetzal::expressive::use`
- then it builds upon it to define an (arbitrary) expression of e.g. the growth rate (but it could be friction, or carrying capacity...)

**Input**

@include{lineno} expressive_2.cpp

**Output**

@include{lineno} expressive_2.txt

---

[//]: # (----------------------------------------------------------------------)
@page expressive_suitability_DEM_landscape Adjusting a suitability map using a Digital Elevation Model

@note 
The objective of this section is to load both a suitability map and a Digital Elevation Model (DEM) with `quetzal::geography::landscape` and prepare their integration into the simulation framework with `quetzal::expressive`
by dynamically modulating the suitability value as a function of the local elevation.

## Why separating Elevation from Suitability ?

It is a common approach to pack a wealth of information into a sole raster file. For instance, this can involve using a single friction file and designating ocean cells with NA values, and obstacles for dispersal with values of 1. Another 
way is to try to inject elevational data into the suitability map during the Ecological Niche Modeling step.

While these approaches are feasible and occasionally advantageous, they do not consistently represent the optimal or most adaptable solution.

In a broader sense, it's advisable to consider each Geographic Information System (GIS) input file as a means of representing a distinct attribute of the landscape such as suitability, friction, or elevation, utilizing `quetzal::geography::landscape` to read and align these input files then utilizing `quetzal::expressive` to adjust or combine these quantities according to the user's modeling intentions.

As an illustration we will show in this example how to adjust the stochasticity of a suitability-driven process as a function of the 
elevation model. This case and its variations can be applicable in various contexts:

- Conveniently enforcing a threshold value beyond which the suitability undergoes a significant change (*e.g.*, becoming 0). This approach is valuable for examining sky-island systems or for easily estimating an isoline by dynamically adjusting the cutoff parameter value at runtime, as opposed to altering input files for every simulation run.
- Depicting the likelihood of a propagule reaching a nunatak, that is a small-scale suitable shelter protruding from the snow sheet (or ice cap). Typically nunataks are the only places where plants can survive in these environments.

**Input**

@include{lineno} expressive_3.cpp

**Output**

@include{lineno} expressive_3.txt

---

[//]: # (----------------------------------------------------------------------)
@page expressive_friction Defining a friction model for trans-oceanic dispersal events

@note 
The objective of this section is to load both a suitability map and a Digital Elevation Model (DEM) with `quetzal::geography::landscape` and prepare their integration into the simulation framework with `quetzal::expressive`
and account for trans-oceanic dispersal events by dynamically defining the friction and the carrying capacity of a cell as functions of the suitability and elevation value.

Drafting events, in the context of biological dispersal, refer to a phenomenon where organisms are carried across large distances by clinging to or being transported by other organisms, objects, or air currents. This can be thought of as a form of passive dispersal where an organism takes advantage of external forces to move beyond its usual range.

The term "drafting" is borrowed from concepts in physics. In biology, this concept is applied to scenarios where organisms, often small and lightweight, catch a ride on other organisms (like birds, insects, or larger animals), objects (like debris), or wind currents. Drafting events can play a significant role in the dispersal of organisms, especially those with limited mobility. 

These drafting events provide an opportunity for organisms to reach new areas that they might not be able to access through their own locomotion. It's an interesting ecological phenomenon that highlights the intricate ways in which different species interact and influence each other's distribution and survival.

The general idea is to define lambda expressions that embed stochastic aspects of the model, while preserving the callable interface `(space, time) -> double`.

**Input**

@include{lineno} expressive_4.cpp

**Output**

@include{lineno} expressive_4.txt

---

[//]: # (----------------------------------------------------------------------)
@page demographic_histories_in_quetzal Demographic Histories in Quetzal

## Background 

In population genetics, demographic history refers to the historical changes in population size, structure, and distribution of individuals within a species over time. It encompasses various events and processes that have shaped the genetic diversity and characteristics of a population. Some key aspects of demographic history include:

- **Population Size Changes:** Demographic history includes fluctuations in population size, such as population expansions (increases) and bottlenecks (sharp decreases). These events can leave distinct signatures in the genetic makeup of a population.

- **Migration and Gene Flow:** Movement of individuals between different populations can lead to gene flow, which can impact the genetic diversity and structure of populations. Understanding migration patterns is crucial for studying demographic history.

- **Founder Effects:** When a small group of individuals establishes a new population, their genetic makeup might not accurately represent the genetic diversity of the original population. This phenomenon is known as a founder effect.

- **Admixture:** Admixture occurs when genetically distinct populations interbreed, leading to the mixing of their genetic material. Admixture can result from migration or colonization events and has a significant influence on genetic diversity.

- **Population Isolation:** Isolated populations may experience unique evolutionary trajectories due to reduced gene flow and increased genetic drift. This can lead to the development of distinct genetic traits and adaptations.

## Graphs structure

There a different modeling approaches to look at these demographic histories, primarily based on the level of intricacy one intends to incorporate into the framework:

- **Phylogenetic Trees:** typically represent a bifurcating pattern of evolution, where each node (or branching point) represents a speciation event. This gives population histories the semantic of a tree graph (called a population tree or species tree), where ancestral populations split into sub-populations, sometimes loosely connected by migration. Gene trees coalesce into these species tree. Genetic inference under this kind of model aims at estimating the tree topology and/or the timing and parameters of events, such as ancestral population size changes and migration rates. It is important to note that in this framework geography is implicit and events such as population size changes and admixture events are explicitely modeled and estimated.

- **Phylogenetic Networks:** while a phylogenetic tree typically represents a bifurcating pattern of evolution, real-world evolutionary processes can be more intricate. Phylogenetic networks are employed when evolutionary events like horizontal gene transfer, hybridization, recombination, and other reticulate processes are involved. These events can lead to interconnected patterns that cannot be accurately illustrated by a simple tree. Networks with Hybridization are specifically designed to capture the evolutionary relationships of organisms that have undergone hybridization events, where two distinct species interbreed to produce hybrid offspring.

- **Spatial graph structure:** Another category of models puts an emphasis on the geographic structure of these populations and embed GIS information into the model. Here the demographic history receives the semantic of a dense spatial graph where geolocalized nodes represent demes and local processes whereas edges represent distances and dispersal processes. In this kind of models estimating the individual properties of so many demes is not judicious and genetic inference rather aims at estimating species-wide parameters that link local deme conditions (such as rainfall) to populational processes (like growth rate). It's crucial to understand that within this geospatial framework, events like population changes and admixture events are emergent properties of the model: they are generally not explicitly outlined in the model nor subjected to direct estimation.

---