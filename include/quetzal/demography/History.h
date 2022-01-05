// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __HISTORY_H_INCLUDED__
#define __HISTORY_H_INCLUDED__

#include "HistoryBase.h"
#include "demographic_policy.h"

namespace quetzal
{
  namespace demography
  {
    /*!
    * @brief Unspecialized class (CRTP design pattern)
    *
    * @tparam Space    Demes identifiers.
    * @tparam demographic_policy    policy used for simulating populations dynamics
    *
    * @ingroup demography
    *
    */
    template<typename Space, typename DispersalPolicy, typename MemoryPolicy>
    class History : public HistoryBase<Space, DispersalPolicy, MemoryPolicy>
    {};


    /** @brief Partial specialization where each individual is dispersed individually.
    *
    * @tparam Space    Demes identifiers.
    *
    * @ingroup demography
    *
    */
    template<typename Space, typename Memory>
    class History<Space, demographic_policy::individual_based, Memory> : public HistoryBase<Space, demographic_policy::individual_based, Memory>
    {
      // Using the HistoryBase class constructor
      using HistoryBase<Space, demographic_policy::individual_based, Memory>::HistoryBase;
    public:
      /**
      * @brief Expands the demographic database.
      *
      * @tparam T a functor for representing the growth process.
      * @tparam U a functor for representing the dispersal process.
      * @tparam V a random number generator.
      *
      * @param sim_growth a functor simulating \f$\tilde{N}_{x}^{t}\f$. The functor
      *                   can possibly internally use a reference on the population
      *                   sizes to represent the time dependency. The signature
      *                   of the function should be equivalent to the following:
      *                   `unsigned int sim_growth(V &gen, const coord_type &x, unsigned int t);`
      * @param kernel a functor representing the dispersal location kernel that
      *               simulates the coordinate of the next location conditionally
      *               to the current location \f$x\f$. The signature should be equivalent
      *               to `coord_type kernel(V &gen, const coord_type &x, unsigned int t);`
      *
      * @exception std::domain_error if the population goes extincted before the simulation is completed.
      */
      template<typename T, typename U, typename V>
      void simulate_forward(T sim_growth, U kernel, V& gen)
      {
        for(unsigned int t = 0; t < this->nb_generations(); ++t)
        {
          unsigned int landscape_individuals_count = 0;
          for(auto x : this->distribution_area(t) )
          {
            auto N_tilde = sim_growth(gen, x, t);
            landscape_individuals_count += N_tilde;
            if(N_tilde >= 1)
            {
              for(unsigned int ind = 1; ind <= N_tilde; ++ind)
              {
                auto y = kernel(gen, x);
                this->m_flows->add_to_flow_from_to(x, y, t, 1);
                this->pop_size(y, t+1) += 1;
              }
            }
          }
          if(landscape_individuals_count == 0)
          {
            throw std::domain_error("Landscape populations went extinct before sampling");
          }
        }
      }
    };
    /** @brief Partial specialization where populations levels are assumed high enough to be considered as divisible masses.
    *
    * @tparam Space    Demes identifiers.
    *
    * @ingroup demography
    */
    template<typename Space, typename Memory>
    class History<Space, demographic_policy::mass_based, Memory> : public HistoryBase<Space, demographic_policy::mass_based, Memory>
    {
      // Using the HistoryBase constructor
      using HistoryBase<Space, demographic_policy::mass_based, Memory>::HistoryBase;
    public:
      /** @brief Simulate forward the demographic history.
      *
      * @tparam T a functor for representing the growth process.
      * @tparam U a functor for representing the dispersal process.
      * @tparam V a random number generator.
      *
      * @param sim_growth a functor simulating \f$\tilde{N}_{x}^{t}\f$. The functor
      *        can possibly internally use a reference on the population sizes database
      *        to represent the time dependency. The signature of the function should
      *        be equivalent to the following:
      *        `unsigned int sim_growth(V &gen, const coord_type &x, unsigned int t);`.
      * @param kernel a functor representing the dispersal transition matrix.
      *               The signature of the function should be equivalent to
      *               `double kernel( const coord_type & x, const coord_type &y);`
      *               and the function should return the probability for an individual
      *               to disperse from \f$x\f$ to \f$y\f$ at generation \f$t\f$.
      *               The expression `kernel.arrival_state()` must be valid
      *               and returns an iterable container of geographic coordinates
      *               indicating the transition kernel state space.
      */
      template<typename T, typename U, typename V>
      void simulate_forward(T sim_growth, U kernel, V& gen)
      {
        for(unsigned int t = 0; t < this->nb_generations(); ++t)
        {
          unsigned int landscape_individuals_count = 0;
          for(auto x : this->distribution_area(t) )
          {
            auto N_tilde = sim_growth(gen, x, t);
            for(auto const& y : kernel.arrival_space(x) )
            {
              auto m = kernel(x, y);
              assert(m >= 0.0 && m <= 1.0);
              // Use of std::ceil to allow oversea migration
              double nb_migrants = std::ceil(m * static_cast<double>(N_tilde));
              if(nb_migrants >= 0){
                landscape_individuals_count += nb_migrants;
                this->m_flows->set_flow_from_to(x, y, t, nb_migrants);
                this->pop_size(y, t+1) += nb_migrants;
              }
            }
          }
          if(landscape_individuals_count == 0)
          {
            throw std::domain_error("Landscape populations went extinct before sampling");
          }
        }
      }
    };
  } // namespace demography
} // namespace quetzal


#endif
