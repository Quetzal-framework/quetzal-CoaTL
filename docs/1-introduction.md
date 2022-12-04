
# Introduction

## Motivations

:seedling: Infering the evolutionary history of populations from genetic datasets
an be a complex task. In some settings it is
actually a mathematically intractable problem, so simulations methods are needed.

Existing softwares like
[SPLATCHE](http://splatche.com/),
[simcoal2](http://cmpg.unibe.ch/software/simcoal2/),
[egglib](http://mycor.nancy.inra.fr/egglib/index.html), or
[msprime](http://msprime.readthedocs.io/en/stable/index.html) are very useful and
user-friendly resources that should be used whenever possible to achieve this task.

However if you are:
- a phD student working on developing some border case simulation model, or
- a post-doctoral researcher uncomfortable with some assumptions/constraints of the existing
softwares, or
- a video-game company looking for implementing realistic
models of evolution :eyes:

you will surely hope to develop your own program without having to start from scratch. And that's why we need something that looks like a standard pile of small components.

:v: Quetzal can help you doing so by offering atomic components (structures,
algorithms, concepts) that can be reused to build a new program.
The template mechanism allow to adapt them efficiently to each particular situation
you may encounter.

The Quetzal-CoaTL interfaces and components are generic, in the same sense as the
Standard Template Library (STL) [2].

## Genericity in Quetzal-CoaTL

The Quetzal-CoaTL library is generic in 3 ways:

### 1 - Algorithm and Data-Structure Interoperability

Each algorithm is written to be **data-structure agnostic**.

This allows a single function to operate on many different classes of data structures.
In other words you can import a **quetzal** algorithm in your code without being invaded by new data-structures.

This decoupling has a huge impact on the code size, falling from \f$O(M*N)\f$ to \f$O(M+N)\f$, \f$M\f$ being the number of algorithms and \f$N\f$ the number of data structures.

@note
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
