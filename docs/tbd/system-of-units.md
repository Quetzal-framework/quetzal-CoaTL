# Defining and using systems of units in Quetzal

A recurrent problem in software development is a repeated discomfort and distrust
when one is
using a `double` variable and has to believe that the documentation is correct
when it states `// !!! THIS IS IN km !!!`. Another problem is to actually
perform dimensional analysis, deducing the dimension of a computed expression
based on the units of its elements.

An answer to this comes from the [mp-units](https://mpusz.github.io/units/) library,
that enables compile-time dimensional analysis and unit/quantity manipulation.
This library is trying to make it to the next standard C++23/26, and we want Quetzal
to be ready when this new feature will come.

To this end, Quetzal implements kilometers, mutation rates, probabilities etc as **distinct types** so the compiler can check **before the program even runs** that the user is not trying to, for example, add a branch length in unit of coalescence with a branch length in units of generations.

- Lanscape quantities
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

The purpose of this feature is to secure your code by *actually* triggering
compilation errors if you are not doing things right. If you don't feel like using
it, it's fine, you don't have to.
