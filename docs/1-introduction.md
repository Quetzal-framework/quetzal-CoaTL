
# Introduction

---

## Why Quetzal?

Infering the evolutionary history of populations from genetic datasets
an be a complex task. In some settings it is a mathematically intractable
problem, so simulations methods are needed.

Many softwares exist out-there:
- [SPLATCHE](http://splatche.com/),
- [simcoal2](http://cmpg.unibe.ch/software/simcoal2/),
- [egglib](http://mycor.nancy.inra.fr/egglib/index.html),
- [msprime](http://msprime.readthedocs.io/en/stable/index.html),
- [SLiM](https://messerlab.org/slim/) 

All are very useful and user-friendly resources that should be used whenever possible.

These resources are very *top-down*: they focus on enabling researchers 
to easily simulate complex evolutionary models and assemble pipelines to analyze data.
Consequently, none of these resources solve a basic problem: providing the 
research community with reusable classes and algorithms in an efficient language.

Despite numerous applications out-there, there is to our knowledge nothing outside of Quetzal
that can help a Computer Science student who simply needs a class with the semantic of a phylogenetic network
where they could store some data.

Top-down approaches are great *and* we also need something that looks like a box of small components.

Quetzal comes here to provide small components (structures, algorithms, concepts) that can be reused to build programs.

Despite C++ bad reputation, C++ changed tremendously in the last two decades. It is now completely
feasible to use generic components with a near-pythonic syntax. There is nothing inaccessible in writing:

```cpp
  auto [tree, root] = get_random_binary_tree<>(number_of_leaves, rng);
  using Flavor = quetzal::format::newick::TreeAlign;
  auto s = quetzal::format::newick::generate_from(tree, root, Flavor());
  std::cout << s << std::endl;
```

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
