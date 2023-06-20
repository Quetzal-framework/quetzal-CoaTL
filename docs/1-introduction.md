
# Introduction

@tableofcontents

## Why Quetzal?

Infering the evolutionary history of populations from genetic datasets
an be a complex task. In some settings it is a mathematically intractable
problem, so simulation-intensive methods are needed.

Many softwares exist out-there:
- [SPLATCHE](http://splatche.com/),
- [simcoal2](http://cmpg.unibe.ch/software/simcoal2/),
- [egglib](http://mycor.nancy.inra.fr/egglib/index.html),
- [msprime](http://msprime.readthedocs.io/en/stable/index.html),
- [SLiM](https://messerlab.org/slim/) 

All are very useful and user-friendly resources that should be used whenever possible.

These resources are very *top-down*: they focus on enabling researchers 
to simulate complex evolutionary models and quickly assemble pipelines to analyze data.
They are very good at solving the big picture. 

Sadly, the price to pay is that none of these resources 
look at the small picture to solve a basic engineering problem: providing the 
research community with reusable classes and algorithms in a reasonably efficient language.

Despite numerous applications out-there, there is to our knowledge nothing out-there
that can help a Computer Science student who simply needs to parse and manipulate a programmatic object 
with the semantic of a phylogenetic network.

Top-down approaches are great **and** also we need something that looks like a toolbox of very small components.

**Quetzal** provides these components (structures, algorithms, concepts) that can be reused to build programs.

---

## Why C++?

When the limits of a project is how many millions of simulations you can run in a limited time
with a constrained budget, it becomes natural to opt for a language that is know for its performance.
It could have been Rust, but C++ is popular in Computer Science classes, and widely used, so we chose C++.

Despite C++ difficult reputation, the language changed tremendously in the last two decades. It is now completely
feasible for a beginner to use generic components with a near-pythonic syntax. There is nothing inaccessible in writing:

```cpp
  auto [tree, root] = get_random_binary_tree<>(number_of_leaves, rng);
  using Flavor = quetzal::format::newick::TreeAlign;
  auto s = quetzal::format::newick::generate_from(tree, root, Flavor());
  std::cout << s << std::endl;
```

--- 

## Why templates?

The Quetzal-CoaTL interfaces and components are generic, in the same sense as the
Standard Template Library (STL) [2]. 

Genericity is what make Python a friendly
language: you don't have to worry too much about the exact types that are manipulated.

Templates in C++ give to the user of a library a similar experience 
(although that may be another story for the actual developers of the library).

--- 

## Why decoupling algorithms from data structures?

**Quetzal** algorithms are written to be **data-structure agnostic**.

This allows a single function to operate on many different classes of data structures.
In other words you can import a **Quetzal** algorithm in your code without being invaded 
by an avalanche of new data-structures.

This decoupling has a huge impact on the code size, falling from \f$O(M*N)\f$ to \f$O(M+N)\f$, \f$M\f$ being the number of algorithms and \f$N\f$ the number of data structures.

@note
This is important for researchers in computational biology:
for 20 algorithms applied to 5 data-structures, it is the difference between having to write, debug, document and maintain 100 functions versus 25. And this difference grows faster and faster with the number of algorithms and data-structures.

---

[1] M. H. Austern.
Generic Programming and the STL.
Professional computing series. Addison-Wesley, 1999.
