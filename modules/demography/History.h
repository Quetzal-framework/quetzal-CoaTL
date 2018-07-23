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

    /*!
     * \brief Traits class for individual based demographic history simulation, strategy suited for small number of individuals in the landscape (small populations).
     * \ingroup demography
     *
     * \details Simulate the demographic history with an individual-based strategy: each
     * individual is dispersed individually.
     * \par History Initialization:
     * The population size history\f$N\f$ is initialized by setting \f$N(.,t_0)\f$, the initial distribution of individuals across demes at the
     * first time of the history \f$t_0\f$. Typically for a biological invasion,
     * this is restricted to the introduction site(s) with the number of introduced
     * individuals. For endemic species, paleoclimatic distribution can be considered
     * as starting points.
     *
     * \par Growth:
     * The offspring number \f$ \tilde{N}_{x}^{t} \f$ in each deme is freely defined by the user. It can for example be
     * sampled in a distribution conditionally to a function of the local density of
     * parents. Non-overlapping generations are considered (the parents die just after reproduction).
     *
     * \par Dispersal:
     *  The children dispersal is done by sampling their destination in a multinomial
     *  law, that defines \f$ \Phi_{x,y}^t \f$, the number of individuals going from
     *  \f$x\f$ to \f$y\f$ at time \f$t\f$:
     *
     * \f[ (\Phi_{x,y}^{t})_{y\in X} \sim \mathcal{M}(\tilde{N}_{x}^{t},(m_{xy})_y) ~. \f]
     *
     * The term \f$ (m_{xy})_y \f$ denotes the parameters of the multinomial law,
     * giving for an individual in \f$x\f$ its proability to go to \f$y\f$.
     * These probabilities are given by the dispersal law with parameter \f$\theta\f$:
     *
     * \f[
     * \begin{array}{cclcl}
     * m  & : &  X^2 & \mapsto & R_{+} \\
     * &   &    (x,y)     & \mapsto & m^{\theta}(x,y)  ~. \\
     * \end{array}
     * \f]
     *
     * After migration, the number of individuals in deme \f$x\f$ is defined by
     * the total number of individuals converging to \f$x\f$:
     *
     * \f[
     * N(x,t+1) = \displaystyle \sum_{i\in X} \Phi_{i,x}^{t}~.
     * \f]
     * \section Example
     * \snippet demography/test/History/Individual_based_history_test.cpp Example
     * \section Output
     * \include demography/test/History/Individual_based_history_test.output
     */
    struct individual_based {
      using value_type = unsigned int;
    };

    /*!
    * \brief Traits class for simulating the demographic history of importnat masses of populations.
    * \ingroup demography
    *
    * \details Simulate the demographic history by considering that populations of individuals are divisible masses,
    *  leading to faster simulations.
    *
    * \par Intialization:
    * \f$N\f$ is initialized by setting \f$N(.,t_0)\f$ the initial distribution
    * of individuals across demes at the first time \f$t_0\f$.
    * Typically for a biological invasion, this is restricted to the introduction site(s)
    * with the number of introduced individuals. For endemic species, paleoclimatic
    * distribution can be considered as starting points.
    *
    * \par Growth:
    * The offspring number \f$\tilde{N}_{x}^{t}\f$ in each deme is freely defined by
    * the user. For example, it can be sampled in a distribution conditionally
    * to a function of the local density of parents. Non-overlapping generations
    * are considered (the parents die just after reproduction).
    *
    * \par Dispersal:
    * The children dispersal is done by splitting the population masses according
    * to their migration probabilities, defining
    * \f$ \Phi_{x,y}^t \f$, the population flow going from \f$x\f$ to \f$y\f$ at time \f$t\f$:
    * \f[ (\Phi_{x,y}^{t})_{y\in  X} = (\tilde{N}_{x}^{t} \times m_{xy})_{y\in  X} ~. \f]
    * The term \f$ m_{xy} \f$ denotes the parameters of the transition kernel,
    * giving for an individual in \f$x\f$ its probability to go to \f$y\f$.
    * These probabilities are given by the dispersal law with parameter \f$\theta\f$:
    * \f[
    * \begin{array}{cclcl}
    * m  & : &  X^2 & \mapsto & R_{+} \\
    * &   &    (x,y)     & \mapsto & m^{\theta}(x,y)  ~. \\
    * \end{array}
    * \f]
    * After migration, the population size in deme \f$x\f$ is defined by the sum of population flows converging to \f$x\f$:
    * \f[
    * N(x,t+1) = \displaystyle \sum_{i\in X} \Phi_{i,x}^{t}~.
    * \f]
    * \section Example
    * \snippet demography/test/History/Mass_based_history_test.cpp Example
    * \section Output
    * \include demography/test/History/Mass_based_history_test.output
    */
    struct mass_based {
      using value_type = double;
    };

  }

  /*!
   * \brief Base class for spatially explicit and forward-in time population history simulators.
   *
   * \tparam Space    Demes identifiers.
   * \tparam Time     EqualityComparable, CopyConstructible.
   * \tparam Strategy    Strategy use for simulating populations dynamics
   *
   * \details Is used as an implementation base of the specialized simulation strategies.
   *
   * \ingroup demography
   *
   */
  template<typename Space, typename Time, typename Strategy>
  class BaseHistory {

      public:

        //! \typedef strategy used for simulating populations dynamics
        using strategy_type = Strategy;

        //! \typedef type of the population flows database
        using flow_type = Flow<Space, Time, typename strategy_type::value_type>;

        //! \typedef type of the population size database
        using pop_sizes_type = PopulationSize<Space, Time, typename strategy_type::value_type>;

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
        std::unique_ptr<pop_sizes_type> m_sizes;
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
        BaseHistory(coord_type const& x, time_type const& t, typename strategy_type::value_type N):
        m_sizes(std::make_unique<pop_sizes_type>()),
        m_flows(std::make_unique<flow_type>()),
        m_kernel(std::make_unique<backward_kernel_type>())
        {
          m_sizes->operator()(x,t) = N;
          m_times.push_back(t);
        }

        /**
          * \brief Read-only access to the demographic flows database
          */
        flow_type const& flows() const
        {
          return *m_flows;
        }

        /**
          * \brief Read and write access to the demographic flows database
          */
        flow_type & flows()
        {
          return *m_flows;
        }

        /**
          * \brief Read-only access to the demographic sizes database.
          * \remark Can be used for composition into time dependent growth functions.
          */
        const pop_sizes_type & pop_sizes() const
        {
          return *m_sizes;
        }

        /**
          * \brief Read-and-write access to the demographic sizes database
          */
        pop_sizes_type & pop_sizes()
        {
          return *m_sizes;
        }

        /**
          * \brief First time recorded in the foward-in-time database history.
          */
        time_type const& first_time() const
        {
          return m_times.front();
        }

        /**
          * \brief Last time recorded in the foward-in-time database history.
          */
        time_type const& last_time() const
        {
          return m_times.back();
        }

        /**
          * \brief Samples a coordinate from the backward-in-time transition matrix
          *
          * \details The transition matrix is computed from the demographic flows
          * database. The returned coordinate object will basically answer the question:
          * when an individual is found in \f$x\f$ at time \f$t\f$, where could it
          * have been at time \f$t-1\f$ ?
          * Let \f$ X \f$ be the geographic space and \f$\Phi_{x,y}^t\f$ be the number of individuals going
          * from coordinate \f$x \in X \f$ to coordinate \f$y\f$ at time \f$t\f$.
          * Knowing that a child node \f$c\f$ is found in \f$ j \in X \f$, the probability for its parent
          * \f$p\f$ to be in \f$i\in X \f$ is: \f$ P( p \in i ~|~ e \in j) = \frac{\Phi_{i, j}^{t}}{ \sum_{k} \Phi_{k, j}^{t} } ~.\f$
          * \section Example
          * \snippet demography/test/History/Mass_based_history_test.cpp Example
          * \section Output
          * \include demography/test/History/Mass_based_history_test.output
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

  /**
  * \brief Demographic history simulated from an individual-based strategy (each individual is dispersed individually).
  *
  * \tparam Space    Demes identifiers.
  * \tparam Time     EqualityComparable, CopyConstructible.
  * \tparam Strategy    Strategy used for simulating populations dynamics
  *
  * \details Inherit from this class and specialize the Strategy template parameter
  *
  * \ingroup demography
  *
  */
  template<typename Space, typename Time, typename Strategy>
  class History : public BaseHistory<Space, Time, Strategy>
  {
  };

  /**
  * \brief Demographic history simulated from an individual-based strategy (each individual is dispersed individually).
  *
  * \tparam Space    Demes identifiers.
  * \tparam Time     EqualityComparable, CopyConstructible.
  *
  * \ingroup demography
  *
  * \details $N$ is initialized by setting \f$ N(.,t_0) \f$ the initial distribution
  * of individuals across demes at the first time \f$ t_0 \f$.
  * Typically for a biological invasion, this is restricted to the introduction site(s)
  * with the number of introduced individuals. For endemic species, paleoclimatic
  * distribution can be considered as starting points. The number of descendants
  * \f$ \tilde{N}_{x}^{t} \f$ in each deme is sampled in a distribution conditionally
  * to a function of the the local density of parents. Non-overlapping generations
  * are considered (the parents die just after reproduction). The children dispersal
  * is done by sampling their destination in a multinomial law, that defines
  * \f$ \Phi_{x,y}^t \f$ the number of individuals going from \f$ x \f$ to \f$ y \f$ at time \f$ t \f$:
  * \f[ (\Phi_{x,y}^{t})_{y\in X} \sim \mathcal{M}(\tilde{N}_{x}^{t},(m_{xy})_y) ~. \f]
  * The term \f$ (m_{xy})_y \f$ denotes the parameters of the multinomial law,
  * giving for an individual in \f$x\f$ its proability to go to \f$y\f$.
  * These probabilities are given by the dispersal law with parameter \f$\theta\f$:
  * \f[
  * \begin{array}{cclcl}
  * m  & : &  X^2 & \mapsto & R_{+} \\
  * &   &    (x,y)     & \mapsto & m^{\theta}(x,y)  ~. \\
  * \end{array}
  * \f]
  * After migration, the number of individuals in deme \f$x\f$ is defined by the total number of individuals converging to \f$x\f$:
  * \f[
  * N(x,t+1) = \displaystyle \sum_{i\in X} \Phi_{i,x}^{t}~.
  * \f\]
  *
  * \section Example
  * \snippet demography/test/History/History_test.cpp Example
  * \section Output
  * \include demography/test/History/History_test.output
  */
  template<typename Space, typename Time>
  class History<Space, Time, strategy::individual_based> : public BaseHistory<Space, Time, strategy::individual_based>
  {
    using BaseHistory<Space, Time, strategy::individual_based>::BaseHistory;

  public:

    /**
      * \brief Expands the demographic database.
      *
      * \tparam Space    Demes identifiers.
      * \tparam Time     EqualityComparable, CopyConstructible.
      *
      * \exception std::domain_error if the population goes extincted before the simulation is completed.
      *
      * \param nb_generations the number of generations to simulate
      * \param sim_growth a functor simulating \f$\tilde{N}_{x}^{t}\f$.
      *
      * The functor can possibly internally use a reference on the population sizes database to represent the time dependency.
      * The signature of the function should be equivalent to the following:
      * `unsigned int sim_growth(Generator &gen, const coord_type &x, const time_type &t);`
      *
      * \param kernel a functor representing the dispersal location kernel that simulates the coordinate of the next location conditionally to the current location.
      * The signature should be equivalent to `coord_type kernel(Generator &gen, const coord_type &x, const time_type &t);`
      *
      * \section Example
      * \snippet demography/test/History/History_test.cpp Example
      * \section Output
      * \include demography/test/History/History_test.output
      */
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


  /**
  * \brief Demographic history where populations levels are assumed high enough to be considered as divisible masses.
  *
  *  \ingroup demography
  *
  * \tparam Space    Demes identifiers.
  * \tparam Time     EqualityComparable, CopyConstructible.
  *
  * \details $N$ is initialized by setting \f$N(.,t_0)\f$ the initial distribution
  * of individuals across demes at the first time \f$t_0\f$.
  * Typically for a biological invasion, this is restricted to the introduction site(s)
  * with the number of introduced individuals. For endemic species, paleoclimatic
  * distribution can be considered as starting points. The number of descendants
  * \f$\tilde{N}_{x}^{t}\f$ in each deme is sampled in a distribution conditionally
  * to a function of the the local density of parents. Non-overlapping generations
  * are considered (the parents die just after reproduction). The children dispersal
  * is done by sampling their destination in a multinomial law, that defines
  * \f$ \Phi_{x,y}^t \f$ the number of individuals going from \f$x\f$ to \f$y\f$ at time \f$t\f$:
  * \f[ (\Phi_{x,y}^{t})_{y\in  X} \sim \mathcal{M}(\tilde{N}_{x}^{t},(m_{xy})_y) ~. \f]
  * The term \f$ (m_{xy})_y \f$ denotes the parameters of the multinomial law,
  * giving for an individual in \f$x\f$ its probability to go to \f$y\f$.
  * These probabilities are given by the dispersal law with parameter \f$\theta\f$:
  * \f[
  * \begin{array}{cclcl}
  * m  & : &  X^2 & \mapsto & R_{+} \\
  * &   &    (x,y)     & \mapsto & m^{\theta}(x,y)  ~. \\
  * \end{array}
  * \f]
  * After migration, the number of individuals in deme \f$x\f$ is defined by the total number of individuals converging to \f$x\f$:
  * \f[
  * N(x,t+1) = \displaystyle \sum_{i\in X} \Phi_{i,x}^{t}~.
  * \f\]
  *
  * \section Example
  * \snippet demography/test/History/History_test.cpp Example
  * \section Output
  * \include demography/test/History/History_test.output
  */
  template<typename Space, typename Time>
  class History<Space, Time, strategy::mass_based> : public BaseHistory<Space, Time, strategy::mass_based>{

    using BaseHistory<Space, Time, strategy::mass_based>::BaseHistory;

public:

  /**
    * \brief Expands the demographic database,
    * \details $N$ is initialized by setting \f$N(.,t_0)\f$ the initial distribution
    * of individuals across demes at the first time \f$t_0\f$.
    * Typically for a biological invasion, this is restricted to the introduction site(s)
    * with the number of introduced individuals. For endemic species, paleoclimatic
    * distribution can be considered as starting points. The number of descendants
    * \f$\tilde{N}_{x}^{t}\f$ in each deme is sampled in a distribution conditionally
    * to a function of the the local density of parents. Non-overlapping generations
    * are considered (the parents die just after reproduction). The children dispersal
    * is done by sampling their destination in a multinomial law, that defines
    * \f$ \Phi_{x,y}^t \f$ the population flow going from \f$x\f$ to \f$y\f$ at time \f$t\f$:
    * \f[ (\Phi_{x,y}^{t})_{y\in  X} = (\tilde{N}_{x}^{t}*m_{xy})_{y\in  X} ~. \f]
    * The term \f$ m_{xy} \f$ denotes the parameters of the transition kernel,
    * giving for an individual in \f$x\f$ its probability to go to \f$y\f$.
    * These probabilities are given by the dispersal law with parameter \f$\theta\f$:
    * \f[
    * \begin{array}{cclcl}
    * m  & : &  X^2 & \mapsto & R_{+} \\
    * &   &    (x,y)     & \mapsto & m^{\theta}(x,y)  ~. \\
    * \end{array}
    * \f]
    * After migration, the population size in deme \f$x\f$ is defined by the sum of population flows converging to \f$x\f$:
    * \f[
    * N(x,t+1) = \displaystyle \sum_{i\in X} \Phi_{i,x}^{t}~.
    * \f\]
    * \param nb_generations the number of generations to simulate
    * \param sim_growth a functor simulating \f$\tilde{N}_{x}^{t}\f$.
    * The functor can possibly internally use a reference on the population sizes database to represent the time dependency.
    * The signature of the function should be equivalent to the following:
    * `double sim_growth(Generator &gen, const coord_type &x, const time_type &t);`
    * \param kernel a functor representing the dispersal location kernel that simulates the coordinate of the next location conditionally to the current location.
    * The signature should be equivalent to `coord_type kernel(Generator &gen, const coord_type &x, const time_type &t);`.
    * The expression `kernel.support(time_type const& t)` must be valid and return an iterable container of geographic coordinates
    * indicating the transition kernel state space at time \f$t\f$.
    * \section Example
    * \snippet demography/test/History/History_test.cpp Example
    * \section Output
    * \include demography/test/History/History_test.output
    */
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
          for(auto y : kernel.state_space(t) )
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
