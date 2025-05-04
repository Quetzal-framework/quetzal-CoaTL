// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/*********************************************************************** * This program is free software; you can
 *redistribute it and/or modify * it under the terms of the GNU General Public License as published by * the Free
 *Software Foundation; either version 2 of the License, or    * (at your option) any later version. *
 *                                                                      *
 ***************************************************************************/

#ifndef __DISCRETE_WRIGHT_FISHER_H_INCLUDED__
#define __DISCRETE_WRIGHT_FISHER_H_INCLUDED__

#include <quetzal.hpp>

using namespace quetzal::units;

namespace quetzal
{
int sampling_size = 10;
using haplotype = int;
constexpr std::vector<haplotype> sample(0, sampling_size);
constexpr auto L = 100 q_bp;
constexpr auto mu = mutation_rate(10⁻9 q_mutations / q_generation / q_bp);

// define a model of demography and reproduction, equivalently assumptions = wright_fisher
assumptions =
    {
        panmixia, diploidy, discrete_time, constant_population_size, neutral, non_overlapping_generation_times,
}

// define a data generation process
generative_model = {draw, number_of_remaining_lineages, infinite_site_model}

summary = {TajimasD}

// assumptions induce a law on gene genealogies
auto genealogical_law = gene_genealogy::induce_law_from(assumptions);
static_assert(genealogical_law == wright_fisher);

/// there are model properties & expectations here

distribution_of_number_of_offspring Binomial(2N, 1/2N)
prob. of coalescence in previous generation 1/(2N)
average coalescence time for 2 individuals: T = 2N
time since last common ancestor: T generations
mean fraction of sites differing between 2 individuals: π = 2µT.
average diversity: π = 2Tµ = 2.2N.µ = 4Nµ = θ
θ: scaled mutation rate (N and µ are confounded)
effective_population_size
census_size
prob. of coalescence in t generations (1 − 1/(2N)) ^ t−1 (1/(2N))
t has a geometric distribution

} // namespace quetzal

#endif
