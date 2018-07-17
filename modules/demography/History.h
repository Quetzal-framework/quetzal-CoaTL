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

#include "Flow.h"
#include "PopulationSize.h"
#include "../../random.h"

#include <vector>
#include <memory>

namespace quetzal {
namespace demography {

  namespace strategy {

    struct individual_based {
      using value_type = unsigned int;
    };

    struct mass_based {
      using value_type = double;
    };

  }

  // https://stackoverflow.com/questions/17056579/template-specialization-and-inheritance

  /*!
   * \brief Spatially explicit and forward-in time population history simulator.
   *
   * \tparam Space    Demes identifiers.
   * \tparam Time     EqualityComparable, CopyConstructible.
   * \tparam Strategy    Strategy use for simulating populations dynamics
   * \ingroup demography
   * \section Example
   * \snippet demography/test/History/History_test.cpp Example
   * \section Output
   * \include demography/test/History/History_test.output
   */
  template<typename Space, typename Time, typename Strategy>
  class CommonBaseHistory {

      public:

        //! \typedef strategy used for simulating populations dynamics
        using strategy_type = Strategy;

        //! \typedef type of the population flows database
        using flow_type = Flow<Space, Time, typename strategy_type::value_type>;

        //! \typedef type of the population size database
        using N_type = PopulationSize<Space, Time, typename strategy_type::value_type>;

        //! \typedef space type
        using coord_type = Space;

        //! \typedef time type
        using time_type = Time;

        //! \typedef type of the discrete distribution used inside the backward dispersal kernel
        using discrete_distribution_type = quetzal::random::DiscreteDistribution<coord_type>;

        //! \typedef Backward dispersal kernel type
        using backward_kernel_type = quetzal::random::TransitionKernel<time_type, discrete_distribution_type>;

      protected:

        // Need to be accessed by the expand method
        std::unique_ptr<N_type> m_sizes;
        std::unique_ptr<flow_type> m_flows;
        std::vector<Time> m_times;
        std::unique_ptr<backward_kernel_type> m_kernel;

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

      public:

        /**
          * \brief Constructor initializing the demographic database.
          * \param x the coordinate of introduction
          * \param t the introduction time
          * \param N the population size at coordinate x at time t
          * \section Example
          * \snippet demography/test/History/History_test.cpp Example
          * \section Output
          * \include demography/test/History/History_test.output
          */
        CommonBaseHistory(coord_type const& x, time_type const& t, typename strategy_type::value_type N):
        m_sizes(std::make_unique<N_type>()),
        m_flows(std::make_unique<flow_type>()),
        m_kernel(std::make_unique<backward_kernel_type>())
        {
          m_sizes->operator()(x,t) = N;
          m_times.push_back(t);
        }

        /**
          * \brief Read-only access to the demographic flows database
          * \section Example
          * \snippet demography/test/History/History_test.cpp Example
          * \section Output
          * \include demography/test/History/History_test.output
          */
        flow_type const& flows() const {return *m_flows;}

        /**
          * \brief Read and write access to the demographic flows database
          * \section Example
          * \snippet demography/test/History/History_test.cpp Example
          * \section Output
          * \include demography/test/History/History_test.output
          */
        flow_type & flows() {return *m_flows;}

        /**
          * \brief Read-only access to the demographic sizes database
          * \section Example
          * \snippet demography/test/History/History_test.cpp Example
          * \section Output
          * \include demography/test/History/History_test.output
          */
        N_type const& N() const {return *m_sizes;}

        /**
          * \brief Read-and-write access to the demographic sizes database
          * \section Example
          * \snippet demography/test/History/History_test.cpp Example
          * \section Output
          * \include demography/test/History/History_test.output
          */
        N_type & N() {return *m_sizes;}

        /**
          * \brief First time recorded in the foward-in-time database history.
          * \section Example
          * \snippet demography/test/History/History_test.cpp Example
          * \section Output
          * \include demography/test/History/History_test.output
          */
        time_type const& first_time() const {return m_times.front(); }

        /**
          * \brief Last time recorded in the foward-in-time database history.
          * \section Example
          * \snippet demography/test/History/History_test.cpp Example
          * \section Output
          * \include demography/test/History/History_test.output
          */
        time_type const& last_time() const {return m_times.back(); }

        /**
          * \brief Samples from the backward-in-time dispersal kernel from the demographic flows database.
          * The returned coordinate object will basically answer the question: when I am in \f$x\f$ at time \f$t\f$, where could I have been at \f$t-1\f$ ?
          * Let \f$ \mathds{X} \f$ be the geographic space and \f$\Phi_{x,y}^t\f$ be the number of individuals going
          * from coordinate \f$x \in \mathds{X}\f$ to coordinate \f$y\f$ at time \f$t\f$.
          * Knowing that a child node \f$c\f$ is found in \f$ j \in \mathds{X} \f$, the probability for its parent
          * \f$p\f$ to be in \f$i\in \mathds{X}\f$ is: \f$ P( p \in i ~|~ e \in j) = \frac{\Phi_{i, j}^{t}}{ \sum_{k} \Phi_{k, j}^{t} } ~.\f$
          * \section Example
          * \snippet demography/test/History/History_test.cpp Example
          * \section Output
          * \include demography/test/History/History_test.output
          */
        template<typename Generator>
        coord_type backward_kernel(coord_type const& x, time_type t, Generator& gen)
        {
          --t;
          assert(m_flows->flux_to_is_defined(x,t));
          if( ! m_kernel->has_distribution(x, t))
          {
            m_kernel->set(x, t, make_backward_distribution(x, t));
          }
          return m_kernel->operator()(gen, x, t);
        }

  };

  template<typename Space, typename Time, typename Strategy>
  class History : public CommonBaseHistory<Space, Time, Strategy>
  {
  };


  template<typename Space, typename Time>
  class History<Space, Time, strategy::individual_based> : public CommonBaseHistory<Space, Time, strategy::individual_based>
  {
    using CommonBaseHistory<Space, Time, strategy::individual_based>::CommonBaseHistory;

  public:

    template<typename Growth, typename Dispersal, typename Generator>
    void expand(unsigned int nb_generations, Growth sim_growth, Dispersal kernel, Generator& gen)
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
              auto y = kernel(gen, x, t);
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

  template<typename Space, typename Time>
  class History<Space, Time, strategy::mass_based> : public CommonBaseHistory<Space, Time, strategy::mass_based>{

    using CommonBaseHistory<Space, Time, strategy::mass_based>::CommonBaseHistory;

public:

    template<typename Growth, typename Dispersal, typename Generator>
    void expand(unsigned int nb_generations, Growth sim_growth, Dispersal kernel, Generator& gen)
    {
      for(unsigned int g = 0; g < nb_generations; ++g)
      {
        auto t = this->last_time();
        auto t_next = t; ++ t_next;

        this->m_times.push_back(t_next);

        for(auto x : this->m_sizes->definition_space(t) )
        {
          auto N_tilde = sim_growth(gen, x, t);
          for(auto y : kernel.support(t) )
          {
            auto m = kernel(x, y, t);
            this->m_flows->set_flux_from_to(x, y, t, m*N_tilde);
            this->m_sizes->operator()(y, t_next) += m*N_tilde;
          }
        }
      }
    }

  };

} // namespace demography
} // namespace quetzal


#endif
