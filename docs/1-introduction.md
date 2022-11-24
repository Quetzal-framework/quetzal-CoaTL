
# Introduction

Coalescence is a mathematical abstraction that is useful for solving many types
of problems in computational population genetics. Consequently, the abstractions of
coalescence theory must also be represented in computer programs. A standardized
generic interface for manipulating concepts like demographic history, reproduction,
migration, lineages, mergers, mutation, recombination is of utmost importance to
encourage reuse of algorithms and data structures. This library intends to do this
for the C++ language.

Part of the Quetzal Coalescence Template Library is a generic interface that allows access
and manipulation of demographic and genealogical graph structures and of data
generation models that operate on these structures, but hides the details of the implementation.

This is an *“open”* interface in the sense that any code that implements this
interface will be interoperable with the Quetzal generic algorithms.

Quetzal-CoaTL provides some general purpose classes that conform to these interfaces,
but they are not meant to be the “only” compatible classes; users will certainly
come with different class implementations that are better for certain situations.

We believe that the main contribution of
the Quetzal library is the formulation of these interfaces.

The Quetzal-CoaTL interfaces and components are generic, in the same sense as the
Standard Template Library (STL) [2].

## Genericity in Quetzal-CoaTL

The Quetzal-CoaTL library is generic in 3 ways:

### 1 - Algorithm and Data-Structure Interoperability

Each algorithm is written to be **data-structure agnostic**.

This allows a single function to operate on many different classes of data structures.
This decoupling has a huge impact on the code size, falling from \f$O(M*N)\f$ to \f$O(M+N)\f$, \f$M\f$ being the number of algorithms and \f$N\f$ the number of data structures.

This is important for researchers in computational biology:
for 20 algorithms and 5 data-structures, it is the difference between having to write, debug, document and maintain 100 functions versus 25. And this difference grows faster and faster with the number of algorithms and data-structures.

### 2 - Extensibility through Function Objects

The second reason why the library is generic is because its algorithms and containers are **extensible**. Users can adapt and customize Quetzal-CoaTL by using *function objects*. This flexibility is what makes it such a great tool for solving general coalescence problems.

### 3 - Element Type Parameterization

The third way that this library is generic is that its containers are parameterized on the element type.
Meaning that data structures like population size history may be represented as `list<T>`,
where T is any type you want (integers, double, floats, high-precision types...).

[1] M. H. Austern.
Generic Programming and the STL.
Professional computing series. Addison-Wesley, 1999.
