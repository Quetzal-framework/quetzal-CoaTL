// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/*********************************************************************** * This program is free software; you can
 *redistribute it and/or modify * it under the terms of the GNU General Public License as published by * the Free
 *Software Foundation; either version 2 of the License, or    * (at your option) any later version. *
 *                                                                      *
 ***************************************************************************/

#ifndef __BASE_HISTORY_H_INCLUDED__
#define __BASE_HISTORY_H_INCLUDED__

#include "../utils/random/DiscreteDistribution.hpp" // BackwardKernel

#include <functional> // std::cref
#include <memory>     // unique_ptr
#include <vector>

namespace quetzal
{
namespace demography
{
/*!
 * @brief Base class for simulating and recording spatial population history,
 *        encpasulating backward migration logic.
 *
 * @details  It makes use of the
 *        CRTP design pattern, meaning that this class has to be specialized
 *        to define History classes offering
 *        customized behaviors, like details of the demograhic algorithms
 *        (mass_based or individual_based) or storage aspects.
 *
 * @tparam Space    Demes identifiers.
 * @tparam DemographicPolicy    Policy Used for populations dynamics simulation algorithms, see demographic_policy
 * @tparam MemoryPolicy    Policy used for storage and access to demographic quantities, see memory_policy
 *
 * @ingroup demography
 *
 */
template <typename Space, typename DemographicPolicy, typename MemoryPolicy> class HistoryBase
{
  public:
    //! \typedef space type
    using coord_type = Space;
    //! \typedef time type
    using time_type = unsigned int;
    //! \typedef policy for storage and access to demographic data
    using memory_policy = MemoryPolicy;
    //! \typedef strategy used for simulating populations dynamics
    using demographic_policy = DemographicPolicy;
    //! \typedef value stored, usually double
    using value_type = typename demographic_policy::value_type;
    //! \typedef type of the population flows database
    using flow_type = typename memory_policy::template flow_type<coord_type, time_type, value_type>;
    //! \typedef type of the population size database
    using pop_sizes_type = typename memory_policy::template pop_sizes_type<coord_type, time_type, value_type>;
    //! \typedef type of the discrete distribution used inside the backward dispersal kernel
    using discrete_distribution_type = quetzal::utils::random::DiscreteDistribution<coord_type>;
    /**
     * @brief Constructor initializing the demographic database.
     *
     * @param x the coordinate of introduction
     * @param N the population size at coordinate x at time 0
     * @param nb_generations the number of generations history is supposed to last
     */
    HistoryBase(coord_type const &x, value_type N, unsigned int nb_generations)
        : m_nb_generations(nb_generations), m_sizes(std::make_unique<pop_sizes_type>()),
          m_flows(std::make_unique<flow_type>())
    {
        m_sizes->set(x, 0, N);
    }
    /*!
    \brief Retrieve demes where N > 0 at time t.
    */
    auto distribution_area(time_type t) const
    {
        return m_sizes->definition_space(t);
    }
    /**
     * \brief Population size at deme x at time t.
     * \return a reference to the value, initialized with value_type default constructor
     * \remark operator allows for more expressive mathematical style in client code
     */
    auto &pop_size(coord_type const &x, time_type const &t)
    {
        return m_sizes->operator()(x, t);
    }
    /**
     * @brief Read-only functor
     */
    auto get_functor_N() const noexcept
    {
        auto N_ref = std::cref(this->m_sizes);
        return [N_ref](coord_type const &x, time_type t) { return N_ref.get()->get(x, t); };
    }
    /**
     * @brief Read-only
     */
    auto get_pop_size(coord_type const &x, time_type const &t)
    {
        return m_sizes->get(x, t);
    }
    /**
     * @brief Used in derived class History to increment migrants
     */
    void set_pop_size(coord_type const &x, time_type const &t, value_type N)
    {
        m_sizes->set(x, t, N);
    }
    /**
     * @brief Last time recorded in the foward-in-time database history.
     */
    unsigned int const &nb_generations() const
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
    template <typename Generator> coord_type backward_kernel(coord_type const &x, time_type t, Generator &gen) const
    {
        --t;
        assert(m_flows->flow_to_is_defined(x, t));
        auto k = make_backward_distribution(x, t);
        return k(gen);
    }

  protected:
    // Need to be accessed by the expand method
    std::unique_ptr<flow_type> m_flows;

  private:
    unsigned int m_nb_generations;
    std::unique_ptr<pop_sizes_type> m_sizes;
    /*!
    \brief Return a backward migration discrete distribution
    */
    auto make_backward_distribution(coord_type const &x, time_type const &t) const
    {
        std::vector<double> weights;
        std::vector<coord_type> support;
        weights.reserve(m_flows->flow_to(x, t).size());
        support.reserve(m_flows->flow_to(x, t).size());
        for (auto const &it : m_flows->flow_to(x, t))
        {
            support.push_back(it.first);
            weights.push_back(static_cast<double>(it.second));
        }
        return discrete_distribution_type(std::move(support), std::move(weights));
    }
}; // end HistoryBase
} // namespace demography
} // namespace quetzal

#endif
