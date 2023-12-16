
# The Quetzal Coalescence Library

**Arnaud Becheler**
Copyright © 2016-2023 Arnaud Becheler

@note
This documentation targets C++ developers seeking to integrate the Quetzal Coalescence Template Library (Quetzal-CoalTL) into their own code projects.

Coalescence theory, also known as the coalescent theory, is a fundamental concept in population genetics that describes the genealogical relationships and ancestry of individuals within a population. It provides a theoretical framework for understanding how genetic variation arises and is inherited over temporal and spatial scales.

In coalescence theory, the focus is on tracing back the genetic history of individuals to identify their common ancestors. It assumes that all individuals within a population share a common ancestor at some point in the past. As we move back in time, the number of ancestors of a particular gene or allele decreases, and eventually, all lineages **coalesce** into a single ancestral lineage.

Coalescence theory has been widely used in population genetics research to investigate topics such as molecular evolution, species divergence, demographic history, and the inference of population parameters from genetic data. It has provided valuable insights into the evolutionary processes that shape genetic diversity and the relationships among individuals within and between populations.

The theory considers various factors such as genetic mutations, genetic drift, spatial heterogeneity and population size changes. It provides simulation-based approaches to study the patterns of genetic diversity, gene flow, and the effects of environmental changes and evolutionary forces on genetic variation within populations.

**Consequently, the abstractions of coalescence theory must be represented in computer programs.**

A standardized generic interface for manipulating concepts like genes genealogies, lineages colliders, mutation models, recombination, landscapes, demographic histories, reproduction, migration is of utmost importance to
encourage reuse of algorithms and data structures. 

This library intends to do this for the C++ language.

\image html pictures/quetzal_scheme.png