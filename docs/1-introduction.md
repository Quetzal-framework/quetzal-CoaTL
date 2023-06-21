
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

These resources are *top-down*-focused: they seek to enable researchers 
to simulate complex evolutionary models and quickly assemble pipelines to analyze data.
They are very good at solving the big picture. 

The price to pay is that none of these resources 
look at the small picture to solve a basic engineering problem: providing the 
research community with reusable classes and algorithms in a reasonably efficient language.

Despite numerous applications out-there, there is to our knowledge nothing out-there
that can help a Computer Science, C++-taught student who simply needs to parse and manipulate programmatic objects
having the semantic of a phylogenetic network.

Top-down approaches are great **and** also we need something that looks like a toolbox of (very) small components.

**Quetzal** provides these components (structures, algorithms, concepts) that can be reused to build programs.

---

## Why C++?

When the limitations of a project revolve around the number of simulations that can be executed within a limited timeframe and with a restricted budget, it becomes natural to choose a language renowned for its performance. While Rust could have been an option, C++ is commonly taught in Computer Science courses and extensively utilized in Ecology and Evolution software, such as msprime, SLiM, and Splatche, making it a logical choice.

Despite C++ being known for its complexity, the language has undergone significant changes over the past two decades. Now, even beginners can comfortably utilize generic components with a syntax that closely resembles Python. There are no barriers to writing code like the following:

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

Genericity is what makes Python a friendly language: as a user of a library you don't 
have to worry too much about the exact types that are manipulated.

Templates in C++ give to the user of a library a similar experience 
(although that may be another story for the actual developers of the library).

--- 

## Why decoupling algorithms from data structures?

**Quetzal** algorithms are written to be **data-structure agnostic**.

In contrast to other programs, the **Quetzal** algorithms have a more abstract nature. This distinction enables a single function to handle a diverse range of data structures. Essentially, you can integrate a  **Quetzal** algorithm into your code without being inundated with a vast array of unfamiliar data structures.

This decoupling also has a significant impact on the size of the code. It reduces the complexity from O(M*N) to O(M+N), where M represents the number of algorithms and N represents the number of data structures.

This feature holds particular importance for computational biology researchers. When dealing with 20 algorithms applied to 5 data structures, it makes the difference between having to create, debug, document, and maintain 100 functions versus just 25. And as the number of algorithms and data structures grows, this difference becomes even more pronounced at an exponential rate.

---

[1] M. H. Austern.
Generic Programming and the STL.
Professional computing series. Addison-Wesley, 1999.
