// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __BASE_HISTORY_H_INCLUDED__
#define __BASE_HISTORY_H_INCLUDED__

#include "Flow.h"
#include "PopulationSize.h"

#include "../random.h"

#include <vector>
#include <memory> // unique_ptr


namespace quetzal {
namespace demography {

/*!
* @brief Base class for spatially explicit and forward-in time population history simulators.
* @details CRTP design pattern: this class has to be specialized (inherited from) to define History classes with different
*          behaviors (eg mass-based or individual-based).
*
*
* @tparam Space    Demes identifiers.
* @tparam Time     EqualityComparable, CopyConstructible.
* @tparam DispersalPolicy    Policy use for populations dynamics simulation algorithms, see dispersal_policy
*
* @ingroup demography
*
*/
template<typename Space, typename Time, typename DispersalPolicy, typename StoragePolicy>
class BaseHistory {

public:

  //! \typedef strategy used for simulating populations dynamics
  using dispersal_policy = DispersalPolicy;

  //! \typedef strategy used for simulating populations dynamics
  using storage_policy = StoragePolicy;

  //! \typedef type of the population flows database
  using flow_type = Flow<Space, Time, typename dispersal_policy::value_type>;

  //! \typedef type of the population size database
  using pop_sizes_type = storage_policy::pop_sizes_type<Space, Time, typename dispersal_policy::value_type>;

  //! \typedef space type
  using coord_type = Space;

  //! \typedef time type
  using time_type = Time;

  //! \typedef type of the discrete distribution used inside the backward dispersal kernel
  using discrete_distribution_type = quetzal::random::DiscreteDistribution<coord_type>;

  //! \typedef Backward dispersal kernel type
  using backward_kernel_type = quetzal::random::TransitionKernel<time_type, discrete_distribution_type>;

  /**
  * @brief Constructor initializing the demographic database.
  *
  * @param x the coordinate of introduction
  * @param t the introduction time
  * @param N the population size at coordinate x at time t
  *
  * \section Example
  * \snippet demography/test/History/History_test.cpp Example
  * \section Output
  * \include demography/test/History/History_test.output
  */
  BaseHistory(coord_type const& x, time_type const& t, typename dispersal_policy::value_type N):
  m_sizes(std::make_unique<pop_sizes_type>()),
  m_flows(std::make_unique<flow_type>()),
  m_kernel(std::make_unique<backward_kernel_type>())
  {
    m_sizes->operator()(x,t) = N;
    m_times.push_back(t);
  }

  /**
  * @brief Read-only access to the demographic flows database
  */
  flow_type const& flows() const
  {
    return *m_flows;
  }

  /**
  * @brief Read and write access to the demographic flows database
  */
  flow_type & flows()
  {
    return *m_flows;
  }

  /**
  * @brief Read-only access to the demographic sizes database.
  * \remark Can be used for composition into time dependent growth functions.
  */
  const pop_sizes_type & pop_sizes() const
  {
    return *m_sizes;
  }

  /**
  * @brief Read-and-write access to the demographic sizes database
  */
  pop_sizes_type & pop_sizes()
  {
    return *m_sizes;
  }

  /**
  * @brief First time recorded in the foward-in-time database history.
  */
  time_type const& first_time() const
  {
    return m_times.front();
  }

  /**
  * @brief Last time recorded in the foward-in-time database history.
  */
  time_type const& last_time() const
  {
    return m_times.back();
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
  std::unique_ptr<pop_sizes_type> m_sizes;
  std::unique_ptr<flow_type> m_flows;
  std::vector<Time> m_times;

  // mutable because sampling backward kernel can update the state
  mutable std::unique_ptr<backward_kernel_type> m_kernel;

private:

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
