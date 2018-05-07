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

  mutable history_type m_history;

  template<typename Generator, typename Forest>
  void simulate_backward_migration(Forest & forest, time_type const& t, Generator& gen) const
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

  template<typename Generator, typename Growth, typename Dispersal>
  void simulate_demography(Growth growth, Dispersal kernel, time_type t_sampling, Generator& gen )
  {
    time_type nb_generations = t_sampling - m_history.first_time() ;
    m_history.expand(nb_generations, growth, kernel, gen);
  }

  template<typename Tree>
  bool is_history_consistent_with_sampling(history_type const& history, forest_type<Tree> const& forest) const
  {
    time_type t = history.last_time();
    for(auto const& it : forest.positions())
    {
      //std::cout << it << " " << t << std::endl;
      if( !history.N().is_defined(it, t) || history.N()(it, t) < forest.nb_trees(it))
      {
        return false;
      }
    }
    return true;
  }

  template<typename Generator, typename Tree>
  forest_type<Tree> coalescence_process(forest_type<Tree> forest, history_type const& history, Generator& gen)
  {
    auto t = history.last_time();
    while( (forest.nb_trees() > 1) && (t > history.first_time()) )
    {
      coalesce(forest, t, gen);
      simulate_backward_migration(forest, t, gen);
      --t;
    }
    coalesce(forest, t, gen);
    return forest;
  }

  template<typename Generator, typename F, typename Tree>
  forest_type<Tree> coalescence_process(forest_type<Tree> forest, history_type const& history, F binary_op, Generator& gen)
  {
    auto t = history.last_time();

    while( (forest.nb_trees() > 1) && (t > history.first_time()) )
    {
      //std::cout << t << std::endl;
      coalesce(forest, t, binary_op, gen);
      simulate_backward_migration(forest, t, gen);
      --t;
    }
    coalesce(forest, t, binary_op, gen);
    return forest;
  }

public:

  IDDC_model_1(coord_type x_0, time_type t_0, N_type N_0) : m_history(x_0, t_0, N_0){}

  auto const& size_history() const
  {
    return m_history.N();
  }

  template<typename Generator, typename Growth, typename Dispersal, typename Tree>
  forest_type<Tree> simulate(
    forest_type<Tree> const& forest,
    Growth growth,
    Dispersal kernel,
    time_type t_sampling,
    Generator& gen )
  {

    simulate_demography(growth, kernel, t_sampling, gen);

    if( ! is_history_consistent_with_sampling(m_history, forest))
    {
        throw std::domain_error("Simulated population size inferior to sampling size");
    }

    return coalescence_process(forest, m_history, gen);
  }


  template<typename Generator, typename Growth, typename Dispersal, typename F, typename Tree>
  forest_type<Tree> simulate(
    forest_type<Tree> forest,
    Growth growth,
    Dispersal kernel,
    time_type t_sampling,
    F binary_op,
    Generator& gen )
  {

    simulate_demography(growth, kernel, t_sampling, gen);

    if( ! is_history_consistent_with_sampling(m_history, forest))
    {
        throw std::domain_error("Simulated population size inferior to sampling size");
    }

    return coalescence_process(forest, m_history, binary_op, gen);
  }

};

} // namespace coalescence
} // namespace quetzal

#endif
