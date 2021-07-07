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

#include "BaseHistory.h"
#include "dispersal_policy.h"

namespace quetzal
{
  namespace demography
  {

    /*!
    * @brief Unspecialized class (CRTP design pattern)
    *
    * @tparam Space    Demes identifiers.
    * @tparam Time     EqualityComparable, CopyConstructible.
    * @tparam dispersal_policy    policy used for simulating populations dynamics
    *
    * @ingroup demography
    *
    */
    template<typename Space, typename Time, typename DispersalPolicy>
    class History : public BaseHistory<Space, Time, DispersalPolicy>
    {};


    /** @brief Partial specialization where each individual is dispersed individually.
    *
    * @tparam Space    Demes identifiers.
    * @tparam Time     EqualityComparable, CopyConstructible.
    *
    * @ingroup demography
    *
    * @section Example
    * @snippet demography/test/History/Individual_based_history_test.cpp Example
    *
    * @section Output
    * @include demography/test/History/Individual_based_history_test.output
    *
    */
    template<typename Space, typename Time>
    class History<Space, Time, dispersal_policy::individual_based> : public BaseHistory<Space, Time, dispersal_policy::individual_based>
    {
      using BaseHistory<Space, Time, dispersal_policy::individual_based>::BaseHistory;
      using coord_type = Space;
    public:

      /**
      * @brief Expands the demographic database.
      *
      * @tparam T a functor for representing the growth process.
      * @tparam U a functor for representing the dispersal process.
      * @tparam V a random number generator.
      *
      * @param nb_generations the number of generations to simulate
      * @param sim_growth a functor simulating \f$\tilde{N}_{x}^{t}\f$. The functor
      *                   can possibly internally use a reference on the population
      *                   sizes to represent the time dependency. The signature
      *                   of the function should be equivalent to the following:
      *                   `unsigned int sim_growth(V &gen, const coord_type &x, const time_type &t);`
      * @param kernel a functor representing the dispersal location kernel that
      *               simulates the coordinate of the next location conditionally
      *               to the current location \f$x\f$. The signature should be equivalent
      *               to `coord_type kernel(V &gen, const coord_type &x, const time_type &t);`
      *
      * @exception std::domain_error if the population goes extincted before the simulation is completed.
      *
      * \section Example
      * \snippet demography/test/History/Individual_based_history_test.cpp Example
      *
      * \section Output
      * \include demography/test/History/Individual_based_history_test.output
      */
      template<typename T, typename U, typename V>
      void expand(unsigned int nb_generations, T sim_growth, U kernel, V& gen)
      {
        for(unsigned int g = 0; g < nb_generations; ++g)
        {
          auto t = this->last_time();
          auto t_next = t; ++ t_next;

          this->m_times.push_back(t_next);

          unsigned int landscape_individuals_count = 0;

          // TODO : optimize the definition_space function (for loop)
          for(auto x : this->m_sizes->definition_space(t) )
          {
            auto N_tilde = sim_growth(gen, x, t);
            landscape_individuals_count += N_tilde;
            if(N_tilde >= 1)
            {
              for(unsigned int ind = 1; ind <= N_tilde; ++ind)
              {
                coord_type y = kernel(gen, x);
                this->m_flows->add_to_flux_from_to(x, y, t, 1);
                this->m_sizes->operator()(y, t_next) += 1;
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
    * @tparam Time     EqualityComparable, CopyConstructible.
    *
    * @ingroup demography
    *
    * @section Example
    * @snippet demography/test/History/Mass_based_history_test.cpp Example
    *
    * @section Output
    * @include demography/test/History/Mass_based_history_test.output
    */
    template<typename Space, typename Time>
    class History<Space, Time, dispersal_policy::mass_based> : public BaseHistory<Space, Time, dispersal_policy::mass_based>{

      using BaseHistory<Space, Time, dispersal_policy::mass_based>::BaseHistory;

    public:

      /** @brief Expands the demographic history through space and time.
      *
      * @tparam T a functor for representing the growth process.
      * @tparam U a functor for representing the dispersal process.
      * @tparam V a random number generator.
      *
      * @param nb_generations the number of generations to simulate.
      * @param sim_growth a functor simulating \f$\tilde{N}_{x}^{t}\f$. The functor
      *        can possibly internally use a reference on the population sizes database
      *        to represent the time dependency. The signature of the function should
      *        be equivalent to the following:
      *        `unsigned int sim_growth(V &gen, const coord_type &x, const time_type &t);`.
      * @param kernel a functor representing the dispersal transition matrix.
      *               The signature of the function should be equivalent to
      *               `double kernel( const coord_type & x, const coord_type &y);`
      *               and the function should return the probability for an individual
      *               to disperse from \f$x\f$ to \f$y\f$ at time \f$t\f$.
      *               The expression `kernel.arrival_state()` must be valid
      *               and returns an iterable container of geographic coordinates
      *               indicating the transition kernel state space.
      *
      * @section Example
      * @snippet demography/test/History/Mass_based_history_test.cpp Example
      *
      * @section Output
      * @include demography/test/History/Mass_based_history_test.output
      */
      template<typename T, typename U, typename V>
      void expand(unsigned int nb_generations, T sim_growth, U kernel, V& gen)
      {
        for(unsigned int g = 0; g < nb_generations; ++g)
        {
          auto t = this->last_time();
          auto t_next = t; ++ t_next;

          this->m_times.push_back(t_next);

          unsigned int landscape_individuals_count = 0;

          for(auto x : this->m_sizes->definition_space(t) )
          {
            auto N_tilde = sim_growth(gen, x, t);

            for(auto const& y : kernel.arrival_space(x) )
            {
              auto m = kernel(x, y);
              assert(m >= 0.0 && m <= 1.0);

              // TODO : I changed floor to ceil for oversea migration
              double nb_migrants = std::ceil(m * static_cast<double>(N_tilde));
              if(nb_migrants >= 0){
                landscape_individuals_count += nb_migrants;
                this->m_flows->set_flux_from_to(x, y, t, nb_migrants);
                this->m_sizes->operator()(y, t_next) += nb_migrants;
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
