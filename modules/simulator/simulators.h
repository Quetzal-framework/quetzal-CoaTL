// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __SIMULATORS_H_INCLUDED__
#define __SIMULATORS_H_INCLUDED__

#include "../demography/History.h"
#include "../coalescence/containers/Forest.h"
#include "../coalescence/policies/merger.h"

#include <map>

namespace quetzal {
namespace simulators {

template<typename Strategy>
class Policy
{};

template<>
class Policy<quetzal::demography::strategy::individual_based>
{
public:
  using merger_type = coalescence::SimultaneousMultipleMerger<coalescence::occupancy_spectrum::on_the_fly>;

  template<typename T, typename U>
  void check_consistency(T const& history, U const& forest) const
  {
    auto t = history.last_time();
    for(auto const& it : forest.positions())
    {
      if( !history.pop_sizes().is_defined(it, t) || history.pop_sizes()(it, t) < forest.nb_trees(it))
      {
        throw std::domain_error("Simulated population size inferior to sampling size");
      }
    }
  }
};

template<>
class Policy<quetzal::demography::strategy::mass_based>
{
public:
  using merger_type = coalescence::BinaryMerger;

  template<typename T, typename U>
  void check_consistency(T const& history, U const& forest) const
  {
    auto t = history.last_time();
    for(auto const& it : forest.positions())
    {
      if( !history.pop_sizes().is_defined(it, t) || history.pop_sizes()(it, t) < forest.nb_trees(it))
      {
        throw std::domain_error("Simulated population size inferior to sampling size");
      }
    }
  }
};

/**
* @brief Coalescence simulator in a spatially explicit landscape.
*
* @tparam Space deme identifiers (like the populations geographic coordinates)
* @tparam Time time identifier (like an integer representing the year)
* @tparam Strategy a politic for the demographic expansion (e.g. individual_based)
*
* @ingroup simulator
*
* @section Example
* @include simulator/test/spatially_explicit_coalescence_simulator/mass_based_simulator_test.cpp
* @section Output
* @include simulator/test/spatially_explicit_coalescence_simulator/mass_based_simulator_test.output
*/
template<typename Space, typename Time, typename Strategy>
class SpatiallyExplicitCoalescenceSimulator : public Policy<Strategy>
{

public:

  using coord_type = Space;
  using time_type = Time;
  using strategy_type = Strategy;
  using N_value_type = typename strategy_type::value_type;

  template<typename Tree>
  using forest_type = quetzal::coalescence::Forest<coord_type, Tree>;

private:

  using history_type = demography::History<coord_type, time_type, strategy_type>;
  history_type m_history;

public:


  /**
  * @brief Constructor
  *
  * @param x_0 Initialization coordinate.
  * @param t_0 Initialization time.
  * @param N_0 Population size at intialization
  */
  SpatiallyExplicitCoalescenceSimulator(coord_type x_0, time_type t_0, N_value_type N_0) : m_history(x_0, t_0, N_0){}

  /**
  * @brief Read-only access to the population size history.
  *
  * @return a functor with signature 'N_value_type fun(coord_type const& x,
  * time_type const& t)' giving the population size in deme \f$x\f$ at time \f$t\f$.
  *
  */
  auto pop_size_history() const noexcept
  {
    return std::cref(m_history.pop_sizes());
  }

  /**
  * @brief Simulate the forward-in-time demographic expansion.
  *
  */
  template<typename Generator, typename Growth, typename Dispersal>
  void expand_demography(time_type t_sampling, Growth growth, Dispersal kernel, Generator& gen )
  {
    time_type nb_generations = t_sampling - m_history.first_time() ;
    m_history.expand(nb_generations, growth, kernel, gen);
  }

  /**
  * @brief Coalesce a forest (nodes) conditionally to the simulated demography.
  *
  */
  template<typename Generator, typename F, typename Tree, typename U>
  forest_type<Tree> coalesce_along_history(forest_type<Tree> forest, F binary_op, Generator& gen, U make_tree) const
  {

    this->check_consistency(m_history, forest);

    auto t = m_history.last_time();

    while( (forest.nb_trees() > 1) && (t > m_history.first_time()) )
    {
      may_coalesce_colocated(forest, t, gen, binary_op, make_tree);
      migrate_backward(forest, t, gen);
      --t;
    }
    may_coalesce_colocated(forest, t, gen, binary_op, make_tree);
    return forest;
  }

private:

  template<typename Generator, typename Forest>
  void migrate_backward(Forest & forest, time_type t, Generator& gen) const noexcept
  {
    Forest new_forest;
    for(auto const it : forest)
    {
      coord_type x = it.first;
      new_forest.insert(m_history.backward_kernel(x, t, gen), it.second);
    }
    assert(forest.nb_trees() == new_forest.nb_trees());
    forest = new_forest;
  }


  template<typename Generator, typename Tree, typename F, typename U>
  void may_coalesce_colocated(forest_type<Tree>& forest, time_type const& t, Generator& gen, F binop, U make_tree) const noexcept
  {
    auto N = pop_size_history();

    for(auto const & x : forest.positions())
    {
      auto range = forest.trees_at_same_position(x);
      std::vector<Tree> v;

      for(auto it = range.first; it != range.second; ++it)
      {
        v.push_back(it->second);
      }

      if(v.size() >= 2){
        auto last = Policy<strategy_type>::merger_type::merge(v.begin(), v.end(), N(x, t), make_tree(x,t), binop, gen );
        forest.erase(x);
        for(auto it = v.begin(); it != last; ++it){
          forest.insert(x, *it);
        }
      }
    }
  }

};

class DiscreteTimeWrightFisher
{

public:

  template<typename Space, typename Tree>
  using forest_type = quetzal::coalescence::Forest<Space, Tree>;

  // @brief Simulates the number of generations to wait before the next coalescence event
  //
  // @tparam Generator a random number Generator
  // @param k number of lineages
  // @param N number of gene copies in the Wright-Fisher population
  // @gen a random number Generator
  //
  // Samples the number of generations to wait before the next coalescence event in
  // a geometric distribution of parameter {k \choose 2} / N
  //
  //
  // @return the number of generations to wait before the next coalescence event.
  //
  template<typename Generator>
  static unsigned int sample_waiting_time(unsigned int k, unsigned int N, Generator& gen)
  {
    assert(N > k);
    // Molecular evolution, a statistical approach, Z. Yang, 9.2 p 313
    double mean = static_cast<double>(N) / boost::math::binomial_coefficient<double>(k, 2);
    double p = 1 / (1 + mean);
    assert( 0 < p && p <= 1);
    std::geometric_distribution dist(p);
    return dist(gen);
  }


  template<typename Tree, typename Generator, typename Binop, typename TimeFun>
  static Tree coalesce(std::vector<Tree> & trees, unsigned int N, Generator& gen, Binop branch, TimeFun make_tree)
  {
    auto last = trees.end();
    unsigned int k = std::distance(trees.begin(), last);
    assert(k > 1);
    while( k > 1 ){
      unsigned int g = sample_waiting_time(k, N, gen);
      last = quetzal::coalescence::binary_merge(trees.begin(), last, make_tree(g), branch, gen);
      --k;
    }

    return *(trees.begin());
  }


  template<typename Space, typename Tree, typename Generator, typename Binop, typename TimeFun>
  static Tree coalesce(forest_type<Space, Tree> const& forest, unsigned int N, Generator& gen, Binop branch, TimeFun make_tree)
  {

    if(forest.nb_trees()==1){
      auto range = forest.trees_at_same_position(*(forest.positions().begin()));
      assert(std::distance(range.first, range.second) == 1);
      return range.first->second;
    }

    std::vector<Tree> trees;
    trees.reserve(forest.nb_trees());
    for(auto & it : forest){
      trees.push_back(it.second);
    }

    return coalesce(trees, N, gen, branch, make_tree);

  }

};


    } // namespace simulators
  } // namespace quetzal

  #endif
