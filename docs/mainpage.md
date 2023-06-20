
# The Quetzal Coalescence Library

**Arnaud Becheler**
Copyright © 2016-2023 Arnaud Becheler

@note
This documentation is meant for C++ developers who want to include the Quetzal
Coalescence Template Library (Quetzal-CoalTL) in their own code project.

Coalescence is a mathematical abstraction of the genealogical relationships between 
DNA sequences that is useful for solving types
of problems in computational population genetics. Consequently, the abstractions of
coalescence theory must be represented in computer programs.

A standardized generic interface for manipulating concepts like genes genealogies, 
lineages colliders, mutation models, recombination, landscapes,
demographic histories, reproduction, migration is of utmost importance to
encourage reuse of algorithms and data structures. 

This library intends to do this for the C++ language.