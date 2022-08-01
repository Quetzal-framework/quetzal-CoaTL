
# The Quetzal Coalescence Library

This documentation is meant for C++ developers who want to include the Quetzal Coalescence Template Library in their own code project.

Coalescence is a mathematical abstraction that is useful for solving many types of problems in computational population genetics. Consequently, the abstractions of
coalescence theory must also be represented in computer programs. A standardized generic interface for manipulating concepts like demographic history, reproduction, migration, lineages, mergers, mutation, recombination is of utmost importance to encourage reuse of algorithms and data structures.

Part of the Quetzal Coalescence Library is a generic interface that allows access
and manipulation of demographic and genealogical graph structures and of data
generation models that operate on these structures, but hides the details of the implementation.

This is an *“open”* interface in the sense that any library that implements this
interface will be interoperable with the Quetzal generic algorithms and with other
algorithms that also use this interface.

The Quetzal CTL provides some general purpose classes that conform to these interfaces,
but they are not meant to be the “only” classes; there certainly will be other classes
that are better for certain situations. We believe that the main contribution of
the Quetzal CTL is the formulation of these interfaces.

The Quetzal-CTL interfaces and components are generic, in the same sense as the
Standard Template Library (STL) [2]. In the following sections, we review the role
that generic programming plays in the STL and compare that to how we applied generic
programming in the context of coalescence.

Of course, if you are already familiar with generic programming, please dive right in!

## Genericity in STL

The STL is generic in 3 ways:

### 1 - Algorithm and Data-Structure Interoperability

Each algorithm is written to be **data-structure agnostic**. This allows a single templated function to operate on many different classes of containers. This decoupling (mainly through the use of iterators) has a huge impact on the STL's code size, that falls from \f$O(M*N)\f$ to \f$O(M+N)\f$, \f$M\f$ being the number of algorithms and \f$N\f$ the number of containers.

For example, for 20 algorithms and 5 data-structures, it is the difference between having to write, debug and maintain 100 functions versus 25. And this difference grows faster and faster with the number of algorithms and data-structures.

### 2 - Extensibility through Function Objects

The second reason why the STL is generic is because its algorithms and containers are **extensible**. Users can adapt and customize the STL by using *function objects*. This flexibility is what makes STL such a great tool for solving real-world problems. Each programming problem brings its own set of entities and interactions that must be modeled. Function objects provide a mechanism for extending the STL to handle the specifics of each problem domain.

### 3 - Element Type Parameterization

The third way that STL is generic is that its containers are parameterized on the element type. Though hugely important, this is perhaps the least “interesting” way in which STL is generic. Generic programming is often summarized by a brief description of parameterized lists such as `std::list<T>`. This hardly scratches the surface!

## Genericity in the Quetzal CTL

Like the STL, there are three ways in which the CTL is generic.

### Algorithm/Data-Structure Interoperability

First, the lineages and population algorithms of the CTL are written to an interface that abstracts away the details of the particular graph data-structure. Like the STL, the CTL uses iterators to define the interface for data-structure traversal. There are three distinct graph traversal patterns:
- traversal of all vertices in the graph,
- through all of the edges,
- and along the adjacency structure of the graph (from a vertex to each of its neighbors).

There are separate iterators for each pattern of traversal.

This generic interface allows template functions such as `format_newick()` to work on a large variety of coalescence data-structures, from lineages implemented with pointer-linked nodes for infinite site mutation models to lineages encoded in arrays for spatially explicit simulation of recombining sequences.

This flexibility is especially important in the domain of coalescence. Coalescence data-structures are often custom-made for a particular application (simulator). Traditionally, if programmers want to reuse an algorithm implementation they must convert/copy their graph data (let's say an Extended Newick Format string for HybridLambda) into the graph library's prescribed graph structure (let's say ms format). This is the case with programs/libraries such as HybridLambda, KernelPop IBDsim, Splatche, ms, msprim, CDPop, EcoGenetics, tskit. This severely limits the reuse of their graph algorithms.

In contrast, custom-made (or even legacy) graph structures can be used as-is with the generic linege algorithms of the CTL, using **external adaptation** (see Section How to Convert Existing Graphs to the CTL). External adaptation wraps a new interface around a data-structure without copying and without placing the data inside adaptor objects. The CTL interface was carefully designed to make this adaptation easy. To demonstrate this, we have built interfacing code for using a variety of graph structures for coalescence and demography (HybridLambda) in CTL graph algorithms.

## Compile-time units system

A part of the CTL uses [mp-units](https://mpusz.github.io/units/) to enable
compile-time dimensional analysis and unit/quantity manipulation. The rational
of compile-time support for Physical Units in C++ libraries can  be found
[here](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p1935r2.html).

In fewer words, instead of having `double` and `int` all across the code base
with comments similar to `// !!! THIS IS IN km !!!`, Quetzal actually implements
them as distinct types so that the compiler can check **before runtime** that you are
not trying to add for example a branch length in unit of coalescence with a branch length in
units of generations.

The objective here is to secure your code by *actually* triggering
compilation errors if you are not doing things right. If you don't feel like using
it, it's fine, you don't have to. If you want, then see [Defining and using systems of units](system-of-units.md)
for explanations about how you can use and extend Quetzal's unit system in your code.

- Landscape quantities
  - Suitability \f$s\f$
  - Growth rate \f$r\f$
  - Carrying capacity \f$K\f$
- Demographic quantities:
  - Population size \f$N\f$
  - Population density \f$d\f$
  - Migration rates \f$m\f$
- Coalescence quantities:
  - Probability of coalescence
  - Probability of recombination
  - Probability of backward migration
- Mutation process:
  - Probability of mutation
- Branch length:
  - coalescence unit
  - generation unit


## Algorithms

The CTL algorithms consist of a core set of algorithm patterns (implemented as generic algorithms) and a larger set of graph algorithms.
By themselves, the algorithm patterns do not compute and update any meaningful quantities over lineages; they are merely building blocks for constructing coalescence algorithms.

The core algorithm patterns are:

- Coalescence engines:
  - mergers:
    - binary
    - multiple
  -
  - Kingman coalescent (binary merger, continuous time)
  - Wright-Fisher (exact) coalescent (multiple merger, discrete time)
  - Lambda coalescent (multiple merger, continuous time)
  - Coalescent with recombination


## Data Structures


[1] M. H. Austern.
Generic Programming and the STL.
Professional computing series. Addison-Wesley, 1999.
