// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __SIMULATORS_UTILS_H_INCLUDED__
#define __SIMULATORS_UTILS_H_INCLUDED__

#include "../demography/History.hpp"
#include "../coalescence/container/Forest.hpp"
#include "../coalescence/merger_policy.hpp"

#include <map>

namespace quetzal {

  template<typename Env, typename F>
  struct neighbor_migration
  {

  	using coord_type = typename Env::coord_type;
    using env_type = Env;
    using F_type = F;

    env_type const& _landscape;
    F_type _f;

    neighbor_migration(env_type const& env, F f):
    _landscape(env),
    _f(f)
    {}

    template<typename T>
  	std::vector<coord_type> arrival_space(coord_type const& x, const T&) const
  	{
      auto v = _landscape.four_nearest_defined_cells(x);
      v.push_back(x);
      return v;
  	}

    template<typename T>
  	double operator()(coord_type const& x, coord_type const& y, T const& t) const
  	{
  		auto v = arrival_space(x,t);
      auto binop = [this, t](auto const& a, auto const& b){return a + _f(b,t);};
      double sum = std::accumulate(v.begin(), v.end(), 0.0, binop);
      return _f(y,t) / sum;
  	}

  };

template<typename Env, typename F>
auto make_neighbor_migration(Env const& env, F f )
{
  return neighbor_migration<Env, F>(env, f);
}


#endif
