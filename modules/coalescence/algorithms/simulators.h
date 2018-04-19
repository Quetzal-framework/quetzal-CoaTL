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

#include "../../demography/History.h"
#include "../containers/Forest.h"
#include "../../../random.h"

namespace quetzal {
namespace coalescence {

template<typename... Args>
class Simulator;

template<typename Space, typename Time, typename Value>
class Simulator<quetzal::demography::History<Space, Time, Value>> {

public:
  using merger_type = quetzal::coalescence::SimultaneousMultipleMerger<quetzal::coalescence::occupancy_spectrum::on_the_fly>;
  using history_type = quetzal::demography::History<Space, Time, Value>;
  using coord_type = Space;
  using time_type = Time;

  template<typename Tree>
  using forest_type = quetzal::coalescence::Forest<coord_type, Tree>;

private:

  using discrete_distribution_type = quetzal::random::DiscreteDistribution<coord_type>;
  using backward_kernel_type = quetzal::random::TransitionKernel<time_type, discrete_distribution_type>;

  history_type const& m_history;
  mutable backward_kernel_type m_kernel;

  auto make_backward_distribution(coord_type const& x, time_type const& t) const {
    std::vector<double> w;
    std::vector<coord_type> X;
    assert(m_history.flows().flux_to_is_defined(x,t));
    auto const& flux_to = m_history.flows().flux_to(x,t);
    w.reserve(flux_to.size());
    X.reserve(flux_to.size());
    for(auto const& it : flux_to){
      X.push_back(it.first);
      w.push_back(static_cast<double>(it.second));
    }
    return discrete_distribution_type(std::move(X),std::move(w));
  }

  template<typename Generator, typename Forest>
  void sim_backward_migration(Forest & forest, time_type const& t, Generator& gen) const {
    Forest new_forest;
    for(auto const it : forest){
      coord_type x = it.first;
      if( ! m_kernel.has_distribution(x, t)){
        m_kernel.set(x, t, make_backward_distribution(x, t));
      }
      new_forest.insert(m_kernel(gen, x, t), it.second);
    }
    assert(forest.nb_trees() == new_forest.nb_trees());
    forest = new_forest;
  }

  template<typename Generator, typename Tree>
  void coalesce(forest_type<Tree>& forest, time_type const& t, Generator& gen) const {
    auto const& N = m_history.N();
    for(auto const & x : forest.positions()){
      auto range = forest.trees_at_same_position(x);
      std::vector<Tree> v;
      for(auto it = range.first; it != range.second; ++it){
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

public:

  Simulator(history_type const& history) : m_history(history){}

  template<typename Generator, typename Tree>
  forest_type<Tree> operator()(Generator& gen, forest_type<Tree> forest) const {
    auto t = m_history.last_time();
    while( (forest.nb_trees() > 1) && (t > m_history.first_time()) ){
      sim_backward_migration(forest, t, gen);
      coalesce(forest, t, gen);
      --t;
    }
    return forest;
  }

};

} // namespace coalescence
} // namespace quetzal

#endif
