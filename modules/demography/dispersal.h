// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#include "../../random.h"
#include <cmath>
#include <vector>

namespace quetzal{
  namespace demography {
    namespace dispersal {

//nathan et al 2012, table 15.1
struct Logistic
{
  static double pdf(double r, double a, double b)
  {
    assert(a > 0 && b >2 && r >= 0);
    return (b/(2*M_PI*(a*a)*std::tgamma(2/b)*std::tgamma(1-2/b)))*(1/(1+(std::pow(r,b)/(std::pow(a,b)))));
  }
};

struct Gaussian
{
  static double pdf(double r, double a)
  {
    assert(a > 0 && r >= 0);
    return 1/(M_PI*a*a) * std::exp(-(r*r)/(a*a)) ;
  }
};

struct NegativeExponential
{
  static double pdf(double r, double a)
  {
    assert(a > 0 && r >= 0);
    return  (1/2*M_PI*a*a)*std::exp(-r/a);
  }
};

template<typename Kernel, typename Distance, typename Space, typename... Args>
quetzal::random::DiscreteDistribution<Space>
make_dispersal_location_random_distribution(Space const& x, std::vector<Space> const& demes, Distance d, Args&&... args)
{
  std::vector<double> weights;
  weights.reserve(demes.size());
  for(auto const& it : demes)
  {
    weights.push_back(Kernel::pdf(d(x, it), std::forward<Args>(args)...));
  }
  return quetzal::random::DiscreteDistribution<Space>(demes, weights);
}

template<typename Kernel, typename Distance, typename Space, typename... Args>
auto make_dispersal_kernel(std::vector<Space> const& demes, Distance d, Args&&... args)
{
  assert(!demes.empty());
  using law_type =  quetzal::random::DiscreteDistribution<Space>;
  quetzal::random::TransitionKernel<law_type> kernel;
  for(auto const& it : demes)
  {
    kernel.set(it, make_dispersal_location_random_distribution<Kernel>(it, demes, d, std::forward<Args>(args)...));
  }
  return kernel;
}

}}}
