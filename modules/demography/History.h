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

#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/symmetric.hpp>
#include <boost/numeric/ublas/vector.hpp>

#include <vector>
#include <memory>


namespace quetzal {

namespace demography {

namespace strategy {

  /*!
   * @brief Class for demographic simulation of small populations.
   *
   * @ingroup demography
   *
   * \details Simulate the demographic history by sampling the location of each
   *          individuals in a proability distribution.
   */
  class individual_based {
  public:

    //! \typedef type used to represent effective population size and flows
    using value_type = unsigned int;
  };

  /*!
  * @brief Class for demographic simulation of large populations.
  *
  * @ingroup demography
  *
  * \details Simulate the demographic history by considering populations of
  *          individuals as divisible masses leading to faster simulations.
  */
  class mass_based {

    /*!
     * Interface for demographic expansion algorithm
     * @tparam M the internal stoarage of migration coefficients
     * @tparam Cont a container containing value with getID method giving a unique integer index
     */
    template<typename Cont, typename M>
    class Interface {
      using matrix_type = M;

      M _matrix;
      Cont const& _points;

      template<typename F>
      auto make_matrix(Cont const& points, F f){

        using namespace boost::numeric::ublas;

        matrix_type A (points.size(), points.size());
        for (unsigned i = 0; i < A.size1 (); ++ i)
        {
          for (unsigned j = 0; j <= i; ++ j)
          {
            A (i, j) = f(points.at(i), points.at(j));
          }
        }

        // Dividing each row by the sum of the row
        auto s = prod(scalar_vector<double>(A.size1(),1.), trans(A));
        std::for_each( s.begin(), s.end(), [](auto a){ assert(a>0); return 1.0/a;});
        return prod(diagonal_matrix<double>(s), A);

      }

    public:
      using point_type = typename Cont::value_type;

      template<typename F>
      Interface(Cont const& points, F const& f) :
      _matrix( make_matrix(points, f) ),
      _points(points)
      {}

      // interface with mass-based demographic history expand method
      // TODO iterate only over non zeros for sparse matrix
      Cont const& arrival_space(){
        return _points;
      }

      // interface with mass-based demographic history expand method
      double operator()(point_type const& x, point_type const& y){
        return _matrix(x.getID(), y.getID());
      }

    }; // inner class Interface

  public:

    //! \typedef type used to represent effective population size and flows
    using value_type = unsigned int;

    /*!
     * Contruct a dispersal kernel compatible with the mass-based strategy
     *
     * @param  points a container of points of arbitrary type T with a `getID` method giving a unique integer index
     * @param  f      a functor giving the migration coefficient between two points
     *                The signature should be equivalent to double f(T const& x, T const& Y)
     * @return        A dispersal kernel
     */
    template<typename Cont, typename F>
    static auto make_distance_based_dispersal(Cont const& points, F f)
    {
      using matrix_type = boost::numeric::ublas::symmetric_matrix<double>;
      return Interface<Cont, matrix_type>(points, f);
    }

    /*!
     * Contruct a dispersal kernel compatible with the mass-based strategy
     *
     * @details This implementation should be chosen when the migration coefficients
     *          are expect to be null over the majort part of the landscape.
     *
     * @param  points a container of points of arbitrary type T with a `getID` method giving a unique integer index
     * @param  f      a functor giving the migration coefficient between two points
     *                The signature should be equivalent to double f(T const& x, T const& Y)
     * @return        A dispersal kernel
     */
    template<typename Cont, typename F>
    static auto make_sparse_distance_based_dispersal(Cont const& points, F f){
      using matrix_type = boost::numeric::ublas::compressed_matrix<double>;
      return Interface<Cont, matrix_type>(points, f);
    }

  };

} // namespace strategy

/*!
* @brief Base class for spatially explicit and forward-in time population history simulators.
*
* @tparam Space    Demes identifiers.
* @tparam Time     EqualityComparable, CopyConstructible.
* @tparam Strategy    Strategy use for simulating populations dynamics
*
* @ingroup demography
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

public:

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
  BaseHistory(coord_type const& x, time_type const& t, typename strategy_type::value_type N):
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

};

  /*!
  * @brief Unspecialized class.
  *
  * @tparam Space    Demes identifiers.
  * @tparam Time     EqualityComparable, CopyConstructible.
  * @tparam Strategy    Strategy used for simulating populations dynamics
  *
  * @ingroup demography
  *
  */
  template<typename Space, typename Time, typename Strategy>
  class History : public BaseHistory<Space, Time, Strategy>
  {
  };


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
  class History<Space, Time, strategy::individual_based> : public BaseHistory<Space, Time, strategy::individual_based>
  {
    using BaseHistory<Space, Time, strategy::individual_based>::BaseHistory;

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
  class History<Space, Time, strategy::mass_based> : public BaseHistory<Space, Time, strategy::mass_based>{

    using BaseHistory<Space, Time, strategy::mass_based>::BaseHistory;

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

          for(auto y : kernel.arrival_space() )
          {
            auto m = kernel(x, y);
            assert(m >= 0.0 && m <= 1.0);

            double nb_migrants = std::ceil(m * static_cast<double>(N_tilde));
            landscape_individuals_count += nb_migrants;
            this->m_flows->set_flux_from_to(x, y, t, nb_migrants);
            this->m_sizes->operator()(y, t_next) += nb_migrants;
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
