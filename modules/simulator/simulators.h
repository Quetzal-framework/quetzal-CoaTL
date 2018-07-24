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

#include "../demography/History.h"
#include "../coalescence/containers/Forest.h"
#include "../coalescence/policies/merger.h"

#include <map>

namespace quetzal {

  namespace simulators {

    template<typename Strategy>
    struct Policy
    {
      using merger_type = coalescence::BinaryMerger;

      template<typename ... Args>
      constexpr void check_consistency(Args&&...) noexcept
      {}

      };

      template<>
      struct Policy<quetzal::demography::strategy::individual_based>
      {
        using merger_type = coalescence::SimultaneousMultipleMerger<coalescence::occupancy_spectrum::on_the_fly>;

        template<typename T, typename U>
        void check_consistency(T const& history, U const& forest) const
        {
          auto t = history.last_time();
          for(auto const& it : forest.positions())
          {
            if( !history.N().is_defined(it, t) || history.pop_sizes(it, t) < forest.nb_trees(it))
            {
              throw std::domain_error("Simulated population size inferior to sampling size");
            }
          }
        }

      };


      template<typename Space, typename Time, typename Strategy>
      class Spatially_explicit_coalescence_simulator : public Policy<Strategy>
      {

      public:

        using coord_type = Space;
        using time_type = Time;
        using strategy_type = Strategy;
        using N_value_type = typename strategy_type::value_type;

        template<typename Tree>
        using forest_type = quetzal::coalescence::Forest<coord_type, Tree>;

      private:

        using history_type = demography::History<coord_type, time_type, N_value_type, strategy_type>;
        history_type m_history;

      public:


        /**
        * \brief Constructor
        *
        * \param x_0 Initialization coordinate.
        * \param t_0 Initialization time.
        * \param N_0 Population size at intialization
        */
        IDDC_model_1(coord_type x_0, time_type t_0, N_value_type N_0) : m_history(x_0, t_0, N_0){}

        /**
        * \brief Read-only access to the population size history.
        *
        * \details The primary purpose of this access is to allow the implemention of
        *          time-dependent population growth models>. The returned functor
        *          can be captured in a lambda expression and/or composed into more
        *          complex expressions using the expressive module.
        *
        * \return a functor with signature 'N_value_type fun(coord_type const& x,
        * time_type const& t)' giving the population size in deme \f$x\f$ at time \f$t\f$.
        *
        */
        auto size_history() const noexcept
        {
          return std::cref(m_history.pop_sizes());
        }


        template<typename Generator, typename Growth, typename Dispersal>
        void expand_demography(Growth growth, Dispersal kernel, time_type t_sampling, Generator& gen ) noexcept
        {
          time_type nb_generations = t_sampling - m_history.first_time() ;
          m_history.expand(nb_generations, growth, kernel, gen);
        }


        template<typename Generator, typename F, typename Tree>
        forest_type<Tree> coalesce_along_history(forest_type<Tree> forest, F binary_op, Generator& gen) const
        {

          TestPolicy<strategy_type>::check_consistency(m_history, forest);

          auto t = history.last_time();

          while( (forest.nb_trees() > 1) && (t > history.first_time()) )
          {
            may_coalesce_colocated(forest, t, binary_op, gen);
            auto new_forest = migrate_backward(forest, t, gen);
            --t;
          }
          may_coalesce_colocated(new_forest, t, binary_op, gen);
          return new_forest;
        }

      private:

        template<typename Generator, typename Forest>
        Forest migrate_backward(Forest & forest, time_type t, Generator& gen) const noexcept
        {
          Forest new_forest;
          for(auto const it : forest)
          {
            coord_type x = it.first;
            new_forest.insert(m_history.backward_kernel(x, t, gen), it.second);
          }
          assert(forest.nb_trees() == new_forest.nb_trees());
          return new_forest;
        }


        template<typename Generator, typename Tree, typename F>
        void may_coalesce_colocated(forest_type<Tree>& forest, time_type const& t, Generator& gen, F binop) const noexcept
        {
          auto N = size_history();

          for(auto const & x : forest.positions())
          {
            auto range = forest.trees_at_same_position(x);
            std::vector<Tree> v;

            for(auto it = range.first; it != range.second; ++it)
            {
              v.push_back(it->second);
            }

            if(v.size() >= 2){
              auto last = merger_type::merge(v.begin(), v.end(), N(x, t), Tree(), binop, gen );
              forest.erase(x);
              for(auto it = v.begin(); it != last; ++it){
                forest.insert(x, *it);
              }
            }
          }
        }
        
      };



    } // namespace coalescence
  } // namespace quetzal

  #endif
