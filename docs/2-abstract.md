# Abstract

This page gives an overview of the features available in the library. 

Please refer to the [Tutorials](tutorials) section for more detals on how to use them. If proper
documentation does not exist yet, or lacks of clarity, please [open an issue](https://github.com/Quetzal-framework/quetzal-CoaTL/issues).

## Input/Output

Quetzal-CoaTL provides a set of functions and classes that facilitate the reading of data structures (using parsers) and the writing of data structures (using generators) that are crucial for addressing coalescence problems.

- Newick tree format
- Extended Newick network format
- Raster datasets
- Spatial points
- Genetic datasets

## Algorithms

The Quetzal-CoaTL algorithms consist of a core set of algorithm patterns (implemented as generic algorithms) and graph algorithms:
  - Mergers:
    - binary
    - multiple
  - Kingman's n-coalescent
    - binary merger
    - continuous time
  - Wright-Fisher coalescent
    - exact
    - multiple merger
    - discrete time
  - Lambda coalescent
    - multiple merger
    - continuous time
  - Structured coalescent
  - Spatial coalescent (Wright-Fisher with migration)
  - Coalescent with recombination

By themselves, these algorithm patterns do not compute or update any meaningful
quantities over lineages; they are merely building blocks for instantiating
coalescence algorithms.

## Graphs

Since algorithms are decoupled from data-structures, these need to

## Compile-time units system

A part of the Quetzal-CoaTL uses [mp-units](https://mpusz.github.io/units/) to enable
compile-time dimensional analysis and unit/quantity manipulation. The rational
of compile-time support for Physical Units in C++ libraries can  be found
[here](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p1935r2.html).

In fewer words, instead of having `double` and `int` all across the code base
with comments similar to `// !!! THIS IS IN km !!!`, Quetzal actually implements
them as distinct types so that the compiler can check **before runtime** that you are
not trying to add for example a branch length in unit of coalescence with a branch length in
units of generations.

The objective here is to secure your code by *actually* triggering
compilation errors if you are not doing things right.

If you don't feel like using it, it's fine, you don't have to. 
If you want, then see [Defining and using systems of units](system-of-units.md)
for explanations about how you can use and extend Quetzal's unit system:

- Landscape quantities
  - Suitability \f$s\f$
  - Growth rate \f$r\f$
  - Carrying capacity \f$K\f$
- Demographic quantities
  - Population size \f$N\f$
  - Population density \f$d\f$
  - Migration rates \f$m\f$
- Coalescence quantities
  - Probability of coalescence
  - Probability of recombination
  - Probability of backward migration
- Mutation process
  - Probability of mutation
- Branch length
  - coalescence unit
  - generation unit



## Data Structures


[1] M. H. Austern.
Generic Programming and the STL.
Professional computing series. Addison-Wesley, 1999.
