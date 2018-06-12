// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr> Florence Jornod <florence@jornod.com>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
************************************************************************/

#ifndef __DISTANCE_BASED_TRANSITION_KERNEL_H_INCLUDED__
#define __DISTANCE_BASED_TRANSITION_KERNEL_H_INCLUDED__

#include "SymmetricDistanceMatrix.h"
#include <unordered_map>

namespace quetzal{
namespace demography {
namespace dispersal {

// contructed at once
template<typename Space>
class DiscreteLocationTransitionKernel
{
  using coord_type = Space;
  std::vector<coord_type> m_points;

  using ID_type = unsigned int;
  std::unordered_map<coord_type, ID_type> m_dico;

  using distribution_type = std::discrete_distribution<ID_type>;
  mutable std::vector<distribution_type> m_kernels;

  std::unordered_map<coord_type, unsigned int> make_dico(std::vector<coord_type> const& points) const
  {
    unsigned int position = 0;
    std::unordered_map<coord_type, unsigned int> m;
    for(auto const& it : points)
    {
      m.emplace(it, position);
      ++position;
    }
    return m;
  }

public:

  DiscreteLocationTransitionKernel(std::vector<coord_type> const& points, std::vector<distribution_type> && kernels):
  m_points(points),
  m_dico(make_dico(points)),
  m_kernels(kernels)
  {}

  template<typename Generator>
  coord_type operator()(coord_type const& x, Generator & gen) const
  {
    auto id = m_dico.at(x);
    auto & dist = m_kernels.at(id);
    return m_points.at(dist(gen));
  }

};

struct DiscreteLocationKernelFactory {

  template<typename Space, typename Value, typename Kernel>
  static auto make(SymmetricDistanceMatrix<Space, Value> const& d, typename Kernel::param_type const& p)
  {
    Kernel k;
    auto f = [k,p](Value r){return k.pdf(r, p);};
    auto weights = d.apply(f);
    std::vector<std::discrete_distribution<unsigned int>> discrete_kernels;
    discrete_kernels.reserve(weights.points().size());
    for(auto const& x : weights.points())
    {
      auto x_weights = weights.at(x);
      discrete_kernels.push_back(std::discrete_distribution<unsigned int>(x_weights.begin(), x_weights.end()));
    }
    return DiscreteLocationTransitionKernel<Space>(weights.points(), std::move(discrete_kernels));
  }

};



} // namespace dispersal
} // namespace demography
} // namespace quetzal

#endif
