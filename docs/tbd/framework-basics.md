# Frameworks basics

## Basic Concepts

Quetzal abstracts a number of concepts to make room for implementations that are
external to the library: see the use of [external adaptation].

### Genetics

Providing high-level abstractions for genetic polymorphism has multiple interests
like opening the door for custom implementation: for sake of efficiency,
or when the *natural* concept of genetic marker does not apply:

- Synthetic DNA and RNA ([Hachimoji](https://www.science.org/doi/10.1126/science.aat0971))
  with more than 4 four informational element has recently been produced,
  with consequences for exobiology and production/analysis of xenobots. These new
  genetic systems will need access to polymorphism analysis for which other implementations
  are not supporting but that Quetzal's abstractions enable.
- Video-games are leaning towards including models of genetic evolution in their
  game-play, and while being not bound to the constraints of real-life molecular biology
  still need to compute diversity statistics, with the potential to highlight universal principles
  of biological evolution (see for example the 2010 paper [Foldit](https://en.wikipedia.org/wiki/Foldit)).

#### Sequences Concepts

- sequence
- site
- allele
- individual
- sample
- aligned
