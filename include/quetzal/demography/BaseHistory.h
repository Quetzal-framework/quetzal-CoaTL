// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __BASE_HISTORY_H_INCLUDED__
#define __BASE_HISTORY_H_INCLUDED__

#include "../random.h" // BackwardKernel

#include <vector>
#include <memory> // unique_ptr


namespace quetzal
{
  namespace demography
  {
    /*!
    * @brief Base class for simulating and recording spatial population history.
    *        It encpasulates backward migration logic. It makes use of the
    *        CRTP design pattern, meaning that this class has to be specialized
    *        to define History classes offering
    *        customized behaviors, like details of the demograhic algorithms
    *        (mass_based or individual_based) or storage aspects.
    *
    * @tparam Space    Demes identifiers.
    * @tparam DispersalPolicy    Policy use for populations dynamics simulation algorithms, see dispersal_policy
    *
    * @ingroup demography
    *
    */
    template<typename Space, typename DispersalPolicy>
    class BaseHistory
    {
    public:
      //! \typedef space type
      using coord_type = Space;
      //! \typedef time type
      using time_type = unsigned int;
      //! \typedef strategy used for simulating populations dynamics
      using dispersal_policy = DispersalPolicy;
      //! \typedef strategy used for simulating populations dynamics
      //! \typedef type of the population flows database
      using flow_type = Flow<coord_type, time_type, typename dispersal_policy::value_type>;
      //! \typedef type of the population size database
      using pop_sizes_type = PopulationSize<coord_type, time_type, typename dispersal_policy::value_type>;
      //                     PopulationSizeOptimized<Space>;
      //                     typename storage_policy::population_size_t<Space, Time, dispersal::policy::value_type>
      //! \typedef type of the discrete distribution used inside the backward dispersal kernel
      using discrete_distribution_type = quetzal::random::DiscreteDistribution<coord_type>;
      //! \typedef Backward dispersal kernel type
      using backward_kernel_type = quetzal::random::TransitionKernel<time_type, discrete_distribution_type>;
      /**
      * @brief Constructor initializing the demographic database.
      *
      * @param x the coordinate of introduction
      * @param N the population size at coordinate x at time 0
      * @param nb_generations the number of generations history is supposed to last
      */
      BaseHistory(coord_type const& x, typename dispersal_policy::value_type N, unsigned int nb_generations):
      m_nb_generations(nb_generations),
      m_sizes(std::make_unique<pop_sizes_type>()),
      m_flows(std::make_unique<flow_type>()),
      m_kernel(std::make_unique<backward_kernel_type>())
      {
        m_sizes->operator()(x,0) = N;
      }
      /**
      * @brief Used in derived class History to increment migrants
      */
      auto& pop_size(coord_type const& x, time_type const& t)
      {
        return m_sizes->operator()(x,t);
      }
      /**
      * @brief Used in ForwardBackwardSpatiallyExplicit::pop_size_history to create a cref and functor composition
      */
      const pop_sizes_type & expose_pop_size() const
      {
        return *m_sizes;
      }
      /**
      * @brief Last time recorded in the foward-in-time database history.
      */
      unsigned int const& nb_generations() const
      {
        return m_nb_generations;
      }
      /**
      * @brief Samples a coordinate from the backward-in-time transition matrix
      *
      * \details The transition matrix is computed from the demographic flows
      * database. The returned coordinate object will basically answer the question:
      * when an individual is found in \f$x\f$ at time \f$t\f$, where could it
      * have been at time \f$t-1\f$ ?
      */
      template<typename Generator>
      coord_type backward_kernel(coord_type const& x, time_type t, Generator& gen) const
      {
        --t;
        assert(m_flows->flux_to_is_defined(x,t));
        if( ! m_kernel->has_distribution(x, t))
        {
          m_kernel->set(x, t, make_backward_distribution(x, t));
        }
        return m_kernel->operator()(gen, x, t);
      }
    protected:
      // Need to be accessed by the expand method
      std::unique_ptr<flow_type> m_flows;
      // mutable because sampling backward kernel can update the state
      mutable std::unique_ptr<backward_kernel_type> m_kernel;
    private:
      unsigned int m_nb_generations;
      std::unique_ptr<pop_sizes_type> m_sizes;
      auto make_backward_distribution(coord_type const& x, time_type const& t) const
      {
        std::vector<double> weights;
        std::vector<coord_type> support;
        weights.reserve(m_flows->flux_to(x,t).size());
        support.reserve(m_flows->flux_to(x,t).size());
        for(auto const& it : m_flows->flux_to(x,t) )
        {
          support.push_back(it.first);
          weights.push_back(static_cast<double>(it.second));
        }
        return discrete_distribution_type(std::move(support),std::move(weights));
      }
    }; // end BaseHistory
  } // namespace demography
} // namespace quetzal


#endif
