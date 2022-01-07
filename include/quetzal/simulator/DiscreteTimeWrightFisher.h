// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __DISCRETE_WRIGHT_FISHER_H_INCLUDED__
#define __DISCRETE_WRIGHT_FISHER_H_INCLUDED__

#include "../coalescence/container/Forest.h"
#include "../coalescence/merger_policy.h"
#include "../coalescence/occupancy_spectrum/sampling_policy.h"

#include <boost/math/special_functions/binomial.hpp>

namespace quetzal {

/**
* @brief Discrete time simulation in a Wright-Fisher Population.
* @remark All methods are static. Use on-the-fly policy.
* @details This simulator consists in a backward coalescent simulation
*          that traces back the ancestry of a sample of gene copies sampled in a
*          populations. Either the user wants to perform generation-by-generation
*          coalescence during a given number of generations (leading to a Forest of
*          coalescent trees rather a single genealogy), or the user would be interested
*          in coalescing lineages until MRCA has been found.
* @ingroup simulator
*/
class DiscreteTimeWrightFisher
{

public:

  //! \typedef forest type
  template<typename Space, typename Tree>
  using forest_type = quetzal::coalescence::container::Forest<Space, Tree>;

  /* @brief Coalesce a spatial forest of trees in a Wrigh-Fisher population until MRCA has been found.
   *
   * @tparam Space the coordinate type
   * @tparam Tree the tree type
   * @tparam Generator a random number Generator
   * @tparam Binop a BranchingOperation functor for coalescence
   * @tparam TimeFun an intialization functor for coalescence
   *
   * @param forest the spatial distribution of lineages to coalesce
   * @param N number of gene copies in the Wright-Fisher population
   * @param gen a random number Generator
   * @param branch a branching binary operation (functor usually given by the selected coalescence policy).
   * @param make_tree a initialization functor (functor usually given by the selected coalescence policy).
   *
   * @detail Coalesce lineages by sampling the waiting times directly in their distribution.
   *
   * @return One single tree.
  */
  template<typename Space, typename Tree, typename Generator, typename Binop, typename TimeFun>
  static Tree coalesce(forest_type<Space, Tree> const& forest, unsigned int N, Generator& gen, Binop branch, TimeFun make_tree)
  {

    if(forest.nb_trees() == 0){

      throw std::logic_error("The forest to coalesce is empty.");

    }else if(forest.nb_trees()==1){

      auto range = forest.trees_at_same_position(*(forest.positions().begin()));
      assert(std::distance(range.first, range.second) == 1);
      return range.first->second;

    }
    assert(forest.nb_trees() >= 2 && "Trying to coalesce less than 2 nodes.");
    auto trees = forest.get_all_trees();
    return coalesce(trees, N, gen, branch, make_tree);
  }

  /* @brief Simulates the number of generations to wait before the next coalescence event
  * @tparam Generator a random number Generator
  * @param k number of lineages
  * @param N number of gene copies in the Wright-Fisher population
  * @param gen a random number Generator
  * @detail Samples the number of generations to wait before the next coalescence
  * event in a geometric distribution of parameter {k \choose 2} / N
  * (see Molecular evolution, a statistical approach, Z. Yang, 9.2 p 313).
  * @return the number of generations to wait before the next coalescence event.
  */
  template<typename Generator>
  static unsigned int sample_waiting_time(unsigned int k, unsigned int N, Generator& gen)
  {
    assert(N > k);
    double mean = static_cast<double>(N) / boost::math::binomial_coefficient<double>(k, 2);
    double p = 1 / (1 + mean);
    assert( 0 < p && p <= 1);
    std::geometric_distribution dist(p);
    return dist(gen);
  }
  /*
   * @brief Coalesce a non-spatial forest of trees in a Wrigh-Fisher population until MRCA has been found.
   *
   * @tparam Tree the tree type
   * @tparam Generator a random number Generator
   * @tparam Binop a BranchingOperation functor for coalescence
   * @tparam TimeFun an intialization functor for coalescence
   *
   * @param trees the set of lineages to coalesce
   * @param N number of gene copies in the Wright-Fisher population
   * @param gen a random number Generator
   * @param branch a branching binary operation (functor usually given by the selected coalescence policy).
   * @param make_tree a initialization functor (functor usually given by the selected coalescence policy).
   *
   * @detail Coalesce lineages by sampling the waiting times directly in their
   * distribution. The algorithm will perform one generation of simultaneous
   * multiple merges if the number of lineages to coalesce k is superior to the
   * population size N.
   *
   * @return One single tree.
   */
  template<typename Tree, typename Generator, typename Binop, typename TimeFun>
  static Tree coalesce(std::vector<Tree> & trees, unsigned int N, Generator& gen, Binop branch, TimeFun make_tree)
  {
    assert(trees.size() >= 2 && "Trying to coalesce less than 2 nodes.");
    if (trees.size() > N)
    {
      // Wright Fisher assumptions are clearly violated
      // Let's do one generation of simultaneous multiple merges to fix it.
      unsigned int g = 1;
      using smm_type = coalescence::merger_policy::SimultaneousMultipleMerger<coalescence::occupancy_spectrum::sampling_policy::on_the_fly>;
      auto last = smm_type::merge(trees.begin(), trees.end(), N, make_tree(g), branch, gen);
      trees.erase(last, trees.end());
    }
    unsigned int k = trees.size();
    assert( k <= N && "number of lineages greater than ancestral Wright-Fisher population size.");
    auto last = trees.end();
    while( k > 1 ){
      unsigned int g = sample_waiting_time(k, N, gen);
      last = quetzal::coalescence::algorithm::binary_merge(trees.begin(), last, make_tree(g), branch, gen);
      --k;
    }
    return *(trees.begin());
  }
  /*
   * @brief Coalesce a non-spatial forest of trees in a single Wrigh-Fisher population
   * during a finite number of generations.
   *
   * @tparam Tree the tree type
   * @tparam Generator a random number Generator
   * @tparam Binop a BranchingOperation functor for coalescence
   * @tparam TimeFun an intialization functor for coalescence
   *
   * @param trees the set of lineages to coalesce
   * @param N number of gene copies in the Wright-Fisher population
   * @param gen a random number Generator
   * g the number of generations that lasts the coalescence process
   * @param branch a branching binary operation (functor usually given by the selected coalescence policy).
   * @param make_tree a initialization functor (functor usually given by the selected coalescence policy).
   *
   * @detail Coalesce lineages by sampling the waiting times directly in their
   * distribution. The algorithm will perform one generation of simultaneous
   * multiple merges if the number of lineages to coalesce k is superior to the
   * population size N.
   *
   * @return A set of trees, possibly of length 1 if MRCA was found during the g generations.
   */
  template<
  typename MergerType = coalescence::merger_policy::SimultaneousMultipleMerger<coalescence::occupancy_spectrum::sampling_policy::on_the_fly>,
  typename Tree, typename Generator, typename Binop, typename TimeFun>
  static std::vector<Tree> coalesce(std::vector<Tree> & trees, unsigned int N, unsigned int g, Generator& gen, Binop branch, TimeFun make_tree)
  {
    assert(trees.size() >= 2 && "Trying to coalesce less than 2 nodes.");
    assert(g > 0 && "Number of generations for the coalescence process should be at least 1");
    assert(N >= 1 && "Population size should be positive for evaluating coalescence probability");
    using merger_type = MergerType;
    unsigned int t = 0;
    auto last = trees.end();
    while( t < g && std::distance(trees.begin(), last) > 1)
    {
      last = merger_type::merge(trees.begin(), last, N, make_tree(t), branch, gen);
      ++t;
    }
    trees.erase(last, trees.end());
    return trees;
  }
};  // End DiscreteTimeWrightFisher

} // namespace quetzal

  #endif
