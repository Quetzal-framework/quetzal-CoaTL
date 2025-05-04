// Copyright 2021 Arnaud Becheler    <abechele@umich.edu> Florence Jornod <florence@jornod.com>

/*********************************************************************** * This program is free software; you can
 *redistribute it and/or modify * it under the terms of the GNU General Public License as published by * the Free
 *Software Foundation; either version 2 of the License, or    * (at your option) any later version. *
 *                                                                      *
 ************************************************************************/

#ifndef __TRANSITION_KERNEL_H_INCLUDED__
#define __TRANSITION_KERNEL_H_INCLUDED__

#include <assert.h>
#include <unordered_map>

namespace quetzal::utils::random
{

template <class... T> class TransitionKernel;

/*!
 * \brief Discrete markovian transition kernel for sampling the next state knowing the present state \f$x_0\f$.
 *
 *
 * Considering a Markov chain \f$(X_k)_{k\in Z_+}\f$ with discrete state space
 * \f$S\f$, and given each initial state \f$x_0\f$ of \f$S\f$, defines the markovian probability
 * distribution \f$P(X_1=\cdot|X_0=x_0)\f$, and samples the stochastic value \f$X_1\f$,
 * according to this probability law.
 *
 * \ingroup random
 * \tparam Distribution type of the transition kernels.
 */
template <typename Distribution> class TransitionKernel<Distribution>
{

  private:
    std::unordered_map<typename Distribution::result_type, Distribution> m_distributions;

  public:
    /*! \typedef State */
    using state_type = typename Distribution::result_type;

    /*!
     * \brief Default constructor
     */
    TransitionKernel()
    {
    }

    /*!
     * \brief Initialize a transition kernel with a state and its associated distribution
     * \param x the departure state
     * \param d the probability distribution on the arrival state
     */
    TransitionKernel(state_type const &x, Distribution const &d)
    {
        m_distributions[x] = d;
    }

    /*!
     * \brief Initialize a transition kernel with a state and its associated distribution
     * \param x the departure state
     * \param d the probability distribution on the arrival state
     */
    TransitionKernel(state_type const &x, Distribution &&d)
    {
        m_distributions[x] = std::move(d);
    }

    /*!
     * \brief Copy constructor.
     */
    TransitionKernel(const TransitionKernel &) = default;

    /*!
     * \brief Move constructor.
     */
    TransitionKernel(TransitionKernel &&) = default;

    /*!
     * \brief Copy assignment operator.
     */
    TransitionKernel<Distribution> &operator=(const TransitionKernel<Distribution> &) = default;

    /*!
     * \brief Move assignment operator.
     */
    TransitionKernel<Distribution> &operator=(TransitionKernel<Distribution> &&) = default;

    /*!
     * \brief Set a departure state and its associated distribution to the kernel
     * \param x the departure state
     * \param d the probability distribution on the arrival state
     */
    TransitionKernel<Distribution> &set(state_type const &x, Distribution const &d)
    {
        m_distributions[x] = d;
        return *this;
    }

    /*!
     * \brief Set a departure state and its associated distribution to the kernel
     * \param x the departure state
     * \param d the probability distribution on the arrival state
     */
    TransitionKernel<Distribution> &set(state_type const &x, Distribution &&d)
    {
        m_distributions[x] = std::move(d);
        return *this;
    }

    /*!
     * \brief Checks if a probability distribution is associated to a departure state.
     * \return true if a distribution is defined, else returns false.
     * \param x the departure state
     */
    bool has_distribution(state_type const &x) const
    {
        return m_distributions.find(x) != m_distributions.end();
    }

    /*!
     * \brief Sample an arrival state conditionnaly to departure state.
     * \return the arrival state.
     * \param x the departure state
     */
    template <typename Generator> state_type operator()(Generator &g, state_type const &x)
    {
        assert(has_distribution(x));
        return m_distributions.at(x).operator()(g);
    }
};

/*!
 * \brief Time-variable discrete markovian transition kernel for sampling the next state knowing the present state
 * \f$x_0\f$.
 *
 * Considering a Markov chain \f$(X_k)_{k\in Z_+}\f$ with discrete state space
 * \f$S\f$, and given each initial state \f$x_0\f$ of \f$S\f$, defines the markovian probability
 * distribution at time \f$t\f$ : \f$P_t(X_1=\cdot|X_0=x_0)\f$, and samples the stochastic value \f$X_1\f$,
 * according to this probability law.
 *
 * \ingroup random
 * \tparam Distribution type of the transition kernels.
 */
template <typename Time, typename Distribution> class TransitionKernel<Time, Distribution>
{

  private:
    using InsiderType = TransitionKernel<Distribution>;

    std::unordered_map<Time, InsiderType> m_kernels;

  public:
    /*! \typedef State */
    using state_type = typename Distribution::result_type;

    /*!
     * \brief Default constructor
     */
    TransitionKernel()
    {
    }

    /*!
     * \brief Initialize a transition kernel with a state/time couple and its associated distribution
     * \param x the departure state
     * \param t the departure time
     * \param d the probability distribution on the arrival state
     */
    TransitionKernel(state_type const &x, Time const &t, Distribution const &d)
    {
        m_kernels[t] = InsiderType(x, d);
    }

    /*!
     * \brief Initialize a transition kernel with a state/time couple and its associated distribution
     * \param x the departure state
     * \param t the departure time
     */
    TransitionKernel(state_type const &x, Time const &t, Distribution &&d)
    {
        m_kernels[t] = InsiderType(x, std::move(d));
    }

    /*!
     * \brief Copy constructor
     */
    TransitionKernel(const TransitionKernel &) = default;

    /*!
     * \brief Move constructor
     * \section Example
     */
    TransitionKernel(TransitionKernel &&) = default;

    /*!
     * \brief Copy assignment operator
     */
    TransitionKernel<Time, Distribution> &operator=(const TransitionKernel<Time, Distribution> &) = default;

    /*!
     * \brief Move assignment operator
     */
    TransitionKernel<Time, Distribution> &operator=(TransitionKernel<Time, Distribution> &&) = default;

    /*!
     * \brief Set a probability distribution for a departure state and time.
     * \param x the departure state
     * \param t the departure time
     * \param d the associated probability distribution.
     */
    TransitionKernel<Time, Distribution> &set(state_type const &x, Time const &t, Distribution const &d)
    {
        m_kernels[t] = InsiderType(x, d);
        return *this;
    }

    /*!
     * \brief Set a probability distribution for a departure state and time.
     * \param x the departure state.
     * \param t the departure time.
     * \param d the associated probability distribution.
     */
    TransitionKernel<Time, Distribution> &set(state_type const &x, Time const &t, Distribution &&d)
    {
        m_kernels[t] = InsiderType(x, std::move(d));
        return *this;
    }

    /*!
     * \brief Checks if a probability distribution is associated to a departure state x at time t.
     * \return true if a distribution is defined, else returns false.
     * \param x the departure state.
     * \param t the departure time.
     */
    bool has_distribution(state_type const &x, Time const &t) const
    {
        bool answer = false;
        if (m_kernels.count(t) == 1)
        {
            if (m_kernels.at(t).has_distribution(x))
            {
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
     */
    template <typename Generator> state_type operator()(Generator &g, state_type const &x, Time const &t)
    {
        assert(this->has_distribution(x, t));
        return m_kernels.at(t).operator()(g, x);
    }
};

} // namespace quetzal::utils::random

#endif
