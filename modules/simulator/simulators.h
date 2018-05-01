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
#include "../../random.h"

namespace quetzal {
namespace simulators {

template<typename Space, typename Time, typename Value>
class IDDC_model_1 {

public:

  using merger_type = quetzal::coalescence::SimultaneousMultipleMerger<quetzal::coalescence::occupancy_spectrum::on_the_fly>;
  using history_type = quetzal::demography::History<Space, Time, Value>;
  using coord_type = Space;
  using time_type = Time;
  using N_type = Value;

  template<typename Tree>
  using forest_type = quetzal::coalescence::Forest<coord_type, Tree>;

private:

  using discrete_distribution_type = quetzal::random::DiscreteDistribution<coord_type>;
  using backward_kernel_type = quetzal::random::TransitionKernel<time_type, discrete_distribution_type>;

  mutable backward_kernel_type m_kernel;
  history_type m_history;

  template<typename Growth, typename Dispersal, typename Generator>
  void
  expand(
    history_type & history,
    unsigned int nb_generations,
    Growth sim_growth,
    Dispersal kernel,
    Generator& gen)
  {

    for(unsigned int g = 1; g != nb_generations; g++)
    {
      auto t = history.last_time();
      auto t_next = t; ++ t_next;
      unsigned int landscape_individuals_count = 0;

      for(auto x : history.N().definition_space(t) )
      {
        auto N_tilde = sim_growth(gen, x, t);
        landscape_individuals_count += N_tilde;
        if(N_tilde >= 1)
        {
          for(unsigned int ind = 1; ind <= N_tilde; ++ind)
          {
            coord_type y = kernel(gen, x, t);
            history.flows().operator()(x, y, t) += 1;
            history.N().operator()(y, t_next) += 1;
          }
        }
      }
      if(landscape_individuals_count == 0)
      {
        throw std::domain_error("Landscape populations went extinct before sampling");
      }
    }
  }

  auto make_backward_distribution(coord_type const& x, time_type const& t) const
  {
    std::vector<double> weights;
    std::vector<coord_type> support;
    assert(m_history.flows().flux_to_is_defined(x,t));
    auto const& flux_to = m_history.flows().flux_to(x,t);
    weights.reserve(flux_to.size());
    support.reserve(flux_to.size());

    for(auto const& it : flux_to)
    {
      support.push_back(it.first);
      weights.push_back(static_cast<double>(it.second));
    }

    return discrete_distribution_type(std::move(support),std::move(weights));
  }

  template<typename Generator, typename Forest>
  void simulate_backward_migration(Forest & forest, time_type const& t, Generator& gen) const
  {
    Forest new_forest;
    for(auto const it : forest)
    {
      coord_type x = it.first;

      if( ! m_kernel.has_distribution(x, t))
      {
        m_kernel.set(x, t, make_backward_distribution(x, t));
      }

      new_forest.insert(m_kernel(gen, x, t), it.second);
    }
    assert(forest.nb_trees() == new_forest.nb_trees());
    forest = new_forest;
  }

  template<typename Generator, typename Tree>
  void coalesce(forest_type<Tree>& forest, time_type const& t, Generator& gen) const
  {
    auto const& N = m_history.N();

    for(auto const & x : forest.positions())
    {
      auto range = forest.trees_at_same_position(x);
      std::vector<Tree> v;

      for(auto it = range.first; it != range.second; ++it)
      {
        v.push_back(it->second);
      }

      if(v.size() >= 2){
        auto last = merger_type::merge(v.begin(), v.end(), N(x, t), gen );
        forest.erase(x);
        for(auto it = v.begin(); it != last; ++it){
          forest.insert(x, *it);
        }
      }

    }
  }

  template<typename Generator, typename Tree, typename F>
  void coalesce(forest_type<Tree>& forest, time_type const& t, F binop, Generator& gen) const
  {
    auto const& N = m_history.N();

    for(auto const & x : forest.positions())
    {
      auto range = forest.trees_at_same_position(x);
      std::vector<Tree> v;

      for(auto it = range.first; it != range.second; ++it)
      {
        v.push_back(it->second);
      }

      if(v.size() >= 2){
        auto last = merger_type::merge(v.begin(), v.end(), N(x, t), Tree(), binop, gen );
        forest.erase(x);
        for(auto it = v.begin(); it != last; ++it){
          forest.insert(x, *it);
        }
      }

    }
  }

public:

  IDDC_model_1(coord_type x_0, time_type t_0, N_type N_0) : m_history(x_0, t_0, N_0){}

  auto const& size_history() const
  {
    return m_history.N();
  }

  template<typename Generator, typename Growth, typename Dispersal, typename Tree>
  forest_type<Tree> coalesce(
    forest_type<Tree> forest,
    Growth growth,
    Dispersal kernel,
    time_type t_sampling,
    Generator& gen )
  {
    assert(forest.positions().size() == 1);
    auto x_sampling = *(forest.positions().cbegin());
    time_type nb_generations = t_sampling - m_history.first_time() ;

    expand(m_history, nb_generations, growth, kernel, gen);

    if( m_history.N()(x_sampling, m_history.last_time()) < forest.nb_trees(x_sampling)){
      throw std::domain_error("Simulated population size inferior to sampling size");
    }

    //std::cout << history.last_time() << std::endl;
    //std::cout << history.flows() << std::endl;

    auto t = m_history.last_time();

    while( (forest.nb_trees() > 1) && (t > m_history.first_time()) )
    {
      simulate_backward_migration(forest, t, gen);
      coalesce(forest, t, gen);
      --t;
    }
    return forest;
  }

  template<typename Generator, typename Growth, typename Dispersal, typename F, typename Tree>
  forest_type<Tree> coalesce(
    forest_type<Tree> forest,
    Growth growth,
    Dispersal kernel,
    time_type t_sampling,
    F binary_op,
    Generator& gen )
  {
    assert(forest.positions().size() == 1);
    auto x_sampling = *(forest.positions().cbegin());
    time_type nb_generations = t_sampling - m_history.first_time() ;

    expand(m_history, nb_generations, growth, kernel, gen);

    if( m_history.N()(x_sampling, m_history.last_time()) < forest.nb_trees(x_sampling)){
      throw std::domain_error("Simulated population size inferior to sampling size");
    }

    //std::cout << history.last_time() << std::endl;
    //std::cout << history.flows() << std::endl;

    auto t = m_history.last_time();

    while( (forest.nb_trees() > 1) && (t > m_history.first_time()) )
    {
      simulate_backward_migration(forest, t, gen);
      coalesce(forest, t, binary_op, gen);
      --t;
    }
    return forest;
  }

};

} // namespace coalescence
} // namespace quetzal

#endif
