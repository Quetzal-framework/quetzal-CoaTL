// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __HISTORY_H_INCLUDED__
#define __HISTORY_H_INCLUDED__

#include "PopulationFlux.h"
#include "PopulationSize.h"
#include "../../random.h"

#include <vector>

namespace quetzal {
namespace demography {

  template<typename Space, typename Time, typename Value>
  class History;

  template<typename Space, typename Time>
  class History<Space, Time, unsigned int>{

  public:

    using flow_type = PopulationFlux<Space, Time, unsigned int>;
    using N_type = PopulationSize<Space, Time, unsigned int>;
    using coord_type = Space;
    using time_type = Time;

    std::vector<Time> m_times;

  private:
    N_type m_sizes;
    flow_type m_flows;

    using discrete_distribution_type = quetzal::random::DiscreteDistribution<coord_type>;
    using backward_kernel_type = quetzal::random::TransitionKernel<time_type, discrete_distribution_type>;
    mutable backward_kernel_type m_kernel;

    auto make_backward_distribution(coord_type const& x, time_type const& t) const
    {

      std::vector<double> weights;
      std::vector<coord_type> support;

      weights.reserve(m_flows.flux_to(x,t).size());
      support.reserve(m_flows.flux_to(x,t).size());

      for(auto const& it : m_flows.flux_to(x,t) )
      {
        support.push_back(it.first);
        weights.push_back(static_cast<double>(it.second));
      }

      return discrete_distribution_type(std::move(support),std::move(weights));
    }

  public:

    History(coord_type const& x, time_type const& t, unsigned int N){
      m_sizes(x,t) = N;
      m_times.push_back(t);
    }

    flow_type const& flows() const {return m_flows;}
    flow_type & flows() {return m_flows;}

    N_type const& N() const {return m_sizes;}
    N_type & N() {return m_sizes;}

    time_type const& first_time() const {return m_times.front(); }
    time_type const& last_time() const {return m_times.back(); }

    template<typename Growth, typename Dispersal, typename Generator>
    void expand(unsigned int nb_generations, Growth sim_growth, Dispersal kernel, Generator& gen)
    {
      for(unsigned int g = 0; g < nb_generations; ++g)
      {
        auto t = last_time();
        auto t_next = t; ++ t_next;
        //std::cout << t << " -> " << t_next << std::endl;

        m_times.push_back(t_next);

        unsigned int landscape_individuals_count = 0;

        // TODO : optimize the definition_space function (for loop)
        for(auto x : m_sizes.definition_space(t) )
        {
          auto N_tilde = sim_growth(gen, x, t);
          landscape_individuals_count += N_tilde;
          if(N_tilde >= 1)
          {
            for(unsigned int ind = 1; ind <= N_tilde; ++ind)
            {
              coord_type y = kernel(gen, x, t);
              m_flows(x, y, t) += 1;
              m_sizes(y, t_next) += 1;
            }
          }
        }

        if(landscape_individuals_count == 0)
        {
          throw std::domain_error("Landscape populations went extinct before sampling");
        }

      }
    }

    // when i'm in x at time t, where could i have been at t-1 ?
    template<typename Generator>
    auto backward_kernel(coord_type const& x, time_type t, Generator& gen) const
    {
      --t;
      //std::cout << "Phi( " << x << ", " << t << ") not defined" << std::endl;
      assert(m_flows.flux_to_is_defined(x,t));

      if( ! m_kernel.has_distribution(x, t))
      {
        m_kernel.set(x, t, make_backward_distribution(x, t));
      }
      return m_kernel(gen, x, t);
    }

  };

} // namespace demography
} // namespace quetzal


#endif
