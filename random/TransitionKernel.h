// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr> Florence Jornod <florence@jornod.com>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
************************************************************************/

#ifndef __TRANSITION_KERNEL_H_INCLUDED__
#define __TRANSITION_KERNEL_H_INCLUDED__

#include <assert.h>
#include <unordered_map>

namespace quetzal{

 template<class...T>
 class TransitionKernel;

/*!
 * \brief Discrete markovian transition kernel for sampling the next state knowing the present state \f$x_0\f$.
 *
 * For each initial state \f$x_0\f$ of an initial state space \f$S_0\f$, defines the associated
 * discrete markovian probability \f$p( x_1 = X | x_0)\f$.
 *
 * \ingroup random
 * \tparam Distribution type of the transition kernels.
 * \section Example
 * \snippet random/test/TransitionKernel/TransitionKernel_test.cpp Example
 * \section Output
 * \include random/test/TransitionKernel/TransitionKernel_test.output
 */
template<typename Distribution>
class TransitionKernel<Distribution> {

private:

	std::unordered_map<typename Distribution::result_type, Distribution> m_distributions;

public:

  /*! \typedef State */
	using state_type = typename Distribution::result_type;

  /*!
   * \brief Default constructor
   * \section Example
   * \snippet random/test/TransitionKernel/initialization_test.cpp Example
   * \section Output
   * \include random/test/TransitionKernel/initialization_test.output
   */
	TransitionKernel(){}

  /*!
   * \brief Initialize a transition kernel with a state and its associated distribution
   * \param x the departure state
   * \param d the probability distribution on the arrival state
   * \section Example
   * \snippet random/test/TransitionKernel/initialization_test.cpp Example
   * \section Output
   * \include rabdom/test/TransitionKernel/initialization_test.output
   */
   TransitionKernel(state_type const& x, Distribution const& d){
		m_distributions[x] = d;
	}

  /*!
   * \brief Initialize a transition kernel with a state and its associated distribution
   * \param x the departure state
   * \param d the probability distribution on the arrival state
   * \section Example
   * \snippet random/test/TransitionKernel/initialization_test.cpp Example
   * \section Output
   * \include random/test/TransitionKernel/initialization_test.output
   */
   TransitionKernel(state_type const& x, Distribution&& d){
		m_distributions[x] = std::move(d);
	}

  /*!
   * \brief Copy constructor.
   * \section Example
   * \snippet random/test/TransitionKernel/initialization_test.cpp Example
   * \section Output
   * \include random/test/TransitionKernel/initialization_test.output
   */
	TransitionKernel(const TransitionKernel&) = default;

  /*!
   * \brief Move constructor.
   * \section Example
   * \snippet random/test/TransitionKernel/initialization_test.cpp Example
   * \section Output
   * \include random/test/TransitionKernel/initialization_test.output
   */
	TransitionKernel(TransitionKernel&&) = default;

  /*!
   * \brief Copy assignment operator.
   * \section Example
   * \snippet random/test/TransitionKernel/initialization_test.cpp Example
   * \section Output
   * \include random/test/TransitionKernel/initialization_test.output
   */
	TransitionKernel<Distribution>& operator=(const TransitionKernel<Distribution>&) = default;

  /*!
   * \brief Move assignment operator.
   * \section Example
   * \snippet random/test/TransitionKernel/initialization_test.cpp Example
   * \section Output
   * \include random/test/TransitionKernel/initialization_test.output
   */
	TransitionKernel<Distribution>& operator=(TransitionKernel<Distribution>&&) = default;

  /*!
   * \brief Add or replace a departure state and its associated distribution to the kernel
   * \param x the departure state
   * \param d the probability distribution on the arrival state
   * \section Example
   * \snippet random/test/TransitionKernel/TransitionKernel_test.cpp Example
   * \section Output
   * \include random/test/TransitionKernel/TransitionKernel_test.output
   */
	TransitionKernel<Distribution>& add(state_type const& x, Distribution const& d){
		m_distributions[x] = d;
		return *this;
	}

  /*!
   * \brief Add or replace a departure state and its associated distribution to the kernel
   * \param x the departure state
   * \param d the probability distribution on the arrival state
   * \section Example
   * \snippet random/test/TransitionKernel/TransitionKernel_test.cpp Example
   * \section Output
   * \include random/test/TransitionKernel/TransitionKernel_test.output
   */
	TransitionKernel<Distribution>& add(state_type const& x, Distribution&& d){
		m_distributions[x] = std::move(d);
		return *this;
	}

  /*!
   * \brief Checks if a probability distribution is associated to a departure state.
   * \return true if a distribution is defined, else returns false.
   * \param x the departure state
   * \section Example
   * \snippet random/test/TransitionKernel/TransitionKernel_test.cpp Example
   * \section Output
   * \include random/test/TransitionKernel/TransitionKernel_test.output
   */
	bool has_distribution(state_type const& x) const {
		return m_distributions.find(x) != m_distributions.end();
	}

  /*!
   * \brief Sample an arrival state conditionnaly to departure state.
   * \return the arrival state.
   * \param x the departure state
   * \section Example
   * \snippet random/test/TransitionKernel/TransitionKernel_test.cpp Example
   * \section Output
   * \include random/test/TransitionKernel/TransitionKernel_test.output
   */
  template<typename Generator>
	state_type operator()(Generator& g, state_type const& x) {
		assert(has_distribution(x));
		return m_distributions.at(x).operator()(g);
	}

};


/*!
 * \brief Time-variable discrete markovian transition kernel for sampling the next state knowing the present state \f$x_0\f$.
 *
 * For a given departure state \f$x_0\f$, defines the associated
 * discrete markovian probability \f$p( x_1 = X | x_0)\f$. Transition probabilities
 * vary in time.
 *
 * \ingroup random
 * \tparam Distribution type of the transition kernels.
 * \section Example
 * \snippet random/test/TimeTransitionKernel/TimeTransitionKernel_test.cpp Example
 * \section Output
 * \include random/test/TimeTransitionKernel/TimeTransitionKernel_test.output
 */
template<typename Time, typename Distribution>
class TransitionKernel<Time, Distribution> {

private:

	using InsiderType = TransitionKernel<Distribution>;

	std::unordered_map<Time, InsiderType> m_kernels;

public:

  /*! \typedef State */
	using state_type = typename Distribution::result_type;

  /*!
   * \brief Default constructor
   * \section Example
   * \snippet random/test/TimeTransitionKernel/initialization_test.cpp Example
   * \section Output
   * \include random/test/TimeTransitionKernel/initialization_test.output
   */
	TransitionKernel(){}

  /*!
   * \brief Initialize a transition kernel with a state/time couple and its associated distribution
   * \param x the departure state
   * \param t the departure time
   * \param d the probability distribution on the arrival state
   * \section Example
   * \snippet random/test/TimeTransitionKernel/initialization_test.cpp Example
   * \section Output
   * \include random/test/TimeTransitionKernel/initialization_test.output
   */
	TransitionKernel(state_type const& x, Time const& t, Distribution const& d){
		m_kernels[t] = InsiderType(x, d);
	}

  /*!
   * \brief Initialize a transition kernel with a state/time couple and its associated distribution
   * \param x the departure state
   * \param t the departure time
   * \section Example
   * \snippet random/test/TimeTransitionKernel/initialization_test.cpp Example
   * \section Output
   * \include random/test/TimeTransitionKernel/initialization_test.output
   */
	TransitionKernel(state_type const& x, Time const& t, Distribution&& d){
		m_kernels[t] = InsiderType(x, std::move(d));
	}

  /*!
   * \brief Copy constructor
   * \section Example
   * \snippet random/test/TimeTransitionKernel/initialization_test.cpp Example
   * \section Output
   * \include random/test/TimeTransitionKernel/initialization_test.output
   */
	TransitionKernel(const TransitionKernel&) = default;

  /*!
   * \brief Move constructor
   * \section Example
   * \snippet random/test/TimeTransitionKernel/initialization_test.cpp Example
   * \section Output
   * \include random/test/TimeTransitionKernel/initialization_test.output
   */
	TransitionKernel(TransitionKernel&&) = default;

  /*!
   * \brief Copy assignment operator
   * \section Example
   * \snippet random/test/TimeTransitionKernel/initialization_test.cpp Example
   * \section Output
   * \include random/test/TimeTransitionKernel/initialization_test.output
   */
	TransitionKernel<Time, Distribution>& operator=(const TransitionKernel<Time, Distribution>&) = default;

  /*!
   * \brief Move assignment operator
   * \section Example
   * \snippet random/test/TimeTransitionKernel/initialization_test.cpp Example
   * \section Output
   * \include random/test/TimeTransitionKernel/initialization_test.output
   */
	TransitionKernel<Time, Distribution>& operator=(TransitionKernel<Time, Distribution>&&) = default;

  /*!
   * \brief Add a probability distribution for a departure state and time.
   * \param x the departure state
   * \param t the departure time
   * \param d the associated probability distribution.
   * \section Example
   * \snippet random/test/TimeTransitionKernel/TimeTransitionKernel_test.cpp Example
   * \section Output
   * \include random/test/TimeTransitionKernel/TimeTransitionKernel_test.output
   */
	TransitionKernel<Time, Distribution>& add(state_type const& x, Time const& t, Distribution const& d){
		m_kernels[t] = InsiderType(x, d);
		return *this;
	}

  /*!
   * \brief Add a probability distribution for a departure state and time.
   * \param x the departure state.
   * \param t the departure time.
   * \param d the associated probability distribution.
   * \section Example
   * \snippet random/test/TimeTransitionKernel/TimeTransitionKernel_test.cpp Example
   * \section Output
   * \include random/test/TimeTransitionKernel/TimeTransitionKernel_test.output
   */
	TransitionKernel<Time, Distribution>& add(state_type const& x, Time const& t, Distribution&& d){
		m_kernels[t] = InsiderType(x, std::move(d));
		return *this;
	}

  /*!
   * \brief Checks if a probability distribution is associated to a departure state x at time t.
   * \return true if a distribution is defined, else returns false.
   * \param x the departure state.
   * \param t the departure time.
   * \section Example
   * \snippet random/test/TransitionKernel/TransitionKernel_test.cpp Example
   * \section Output
   * \include random/test/TransitionKernel/TransitionKernel_test.output
   */
	bool has_distribution(state_type const& x, Time const& t) const {
		bool answer = false;
		if(m_kernels.count(t) == 1){
			if(m_kernels.at(t).has_distribution(x)){
				answer = true;
			}
		}
		return answer;
	}

  /*!
   * \brief Sample an arrival state conditionnaly to departure state and a time.
   * \return the arrival state.
   * \param x the departure state.
   * \param t the departure time.
   * \section Example
   * \snippet random/test/TransitionKernel/TransitionKernel_test.cpp Example
   * \section Output
   * \include random/test/TransitionKernel/TransitionKernel_test.output
   */
	template<typename Generator>
	state_type operator()(Generator& g, state_type const& x, Time const& t) {
		assert(this->has_distribution(x, t));
		return m_kernels.at(t).operator()(g, x);
	}
};


} // namespace quetzal

#endif
