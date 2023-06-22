
# Introduction

@tableofcontents

## Why Quetzal?

Inferring the evolutionary history of populations based on genetic datasets can pose a challenging undertaking. In certain scenarios, the task becomes mathematically intractable, necessitating the utilization of simulation-intensive methods.

Many softwares solving for this exist out-there:
- [SPLATCHE](http://splatche.com/),
- [simcoal2](http://cmpg.unibe.ch/software/simcoal2/),
- [egglib](http://mycor.nancy.inra.fr/egglib/index.html),
- [msprime](http://msprime.readthedocs.io/en/stable/index.html),
- [SLiM](https://messerlab.org/slim/) 

Whenever possible, it is highly recommended to utilize these resources as they are incredibly valuable and user-friendly.

These resources have a top-down focus, aiming to empower researchers in simulating complex evolutionary models and swiftly constructing pipelines for data analysis. They excel at solving high-level challenges and providing a comprehensive overview.

However, the trade-off is that these resources do not address the smaller-scale engineering problems, such as offering reusable classes and algorithms in an efficient language, which are essential for the research activity.

Despite the existence of numerous applications, to our knowledge, there is currently no solution available to assist Computer Science students, specifically those familiar with C++, who simply need to parse and manipulate programmatic objects with the semantic of a phylogenetic network.

While top-down approaches are valuable, there is also a need for a toolbox of extremely modular components.

Quetzal fills this gap by providing these components (structures, algorithms, and concepts) that can be reused to construct programs.

---

## Why -CoaTL ?

For **Coa**-lescence **T**-emplate **L**-ibrary.

Quetzalcoatl, the Feathered Serpent, was a powerful divinity from Mesoamerica who created the world (it still took him 600 years to reach a satisfying version, and we strongly identify to this part of the tale).

The god’s name is a combination of two Nahuatl words:

- `quetzal` refers to the Resplendant Quetzal (an emerald bird symbol of freedom)
- `coatl` basically means snake (remember, all that glitters is not gold…) and conveniently aligns with Coalescence Template Library.

But let's be honest, the full name is quite a mouthful, so let's just focus on the dazzling aspects and call it Quetzal for short (because who doesn't love a little sparkle in their code?).

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

The interfaces and components of Quetzal-CoaTL are designed to be generic, similar to the concept of the Standard Template Library (STL) [2].

Genericity is what contributes to Python's user-friendly nature, where library users don't need to overly concern themselves with specific types being manipulated.

In C++, templates offer library users a similar experience (though, for the actual developers of the library, it may be a different story altogether).

--- 

## Why decoupling algorithms from data structures?

**Quetzal** algorithms are written to be **data-structure agnostic**.

In contrast to other programs, the **Quetzal** algorithms have a more abstract nature. This distinction enables a single function to handle a diverse range of data structures. Essentially, you can integrate a  **Quetzal** algorithm into your code without being inundated with a vast array of unfamiliar data structures.

@note
This decoupling also has a significant impact on the size of the code. It reduces the complexity from \f$O(M*N)\f$ to \f$O(M+N)\f$, where \f$M\f$ represents the number of algorithms and \f$N\f$ represents the number of data structures.

This feature holds particular importance for computational biology researchers. When dealing with 20 algorithms applied to 5 data structures, it makes the difference between having to create, debug, document, and maintain 100 functions versus just 25. And as the number of algorithms and data structures grows, this difference becomes even more pronounced, growing at an exponential rate.

---

[1] M. H. Austern.
Generic Programming and the STL.
Professional computing series. Addison-Wesley, 1999.
