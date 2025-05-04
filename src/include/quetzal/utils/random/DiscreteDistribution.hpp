// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/*********************************************************************** * This program is free software; you can
 *redistribute it and/or modify * it under the terms of the GNU General Public License as published by * the Free
 *Software Foundation; either version 2 of the License, or    * (at your option) any later version. *
 *                                                                      *
 ************************************************************************/

#ifndef __DISCRETE_DISTRIBUTION_H_INCLUDED__
#define __DISCRETE_DISTRIBUTION_H_INCLUDED__

#include <assert.h>
#include <random>
#include <vector>

namespace quetzal::utils::random
{
/*!
 * \brief Sampling (non) arithmetic values in discrete probability distribution.
 *
 * Produces random State objects, where the probability of each individual State
 * \f$i\f$ is defined as \f$w = i/S\f$, that is the weight of the ith integer
 * divided by the sum of all \f$n\f$ weights.
 *
 * \ingroup random
 * \tparam State is the support of the probability distribution.
 */
template <typename State> class DiscreteDistribution
{
  private:
    using SelfType = DiscreteDistribution<State>;
    class Param
    {
      private:
        std::vector<State> m_support;
        std::vector<double> m_weights;
        using distribution_type = DiscreteDistribution<State>;

      public:
        /*! DefaultConstructible */
        Param() : m_support(std::vector<State>(1)), m_weights(std::vector<double>(1, 1.0))
        {
        }
        /*! CopyConstructible */
        Param(Param const &other) = default;
        /*! Constructor 1 */
        Param(std::vector<State> const &support, std::vector<double> const &weights)
            : m_support(support), m_weights(weights)
        {
        }
        /*! Move constructor */
        Param(std::vector<State> &&support, std::vector<double> &&weights)
            : m_support(std::move(support)), m_weights(std::move(weights))
        {
        }
        /*! CopyAssignable */
        Param &operator=(Param const &other) = default;
        /*! EqualityComparable */
        bool operator==(Param const &other)
        {
            return (m_support == other.m_support && m_weights == other.m_weights);
        }
        /*! Get support of the distribution */
        std::vector<State> const &support() const
        {
            return m_support;
        }
        /*! Get weights of  the distribution */
        std::vector<double> const &weights() const
        {
            return m_weights;
        }
    }; // end inner class Param
    Param m_param;

  public:
    /*! \typedef State */
    using result_type = State;
    /*! \typedef the type of the parameter set, see
     *  <a href="http://en.cppreference.com/w/cpp/concept/RandomNumberDistribution">RandomNumberDistribution</a>.
     */
    using param_type = Param;
    /**
     * \brief Default constructor
     * \remark result_type should be DefaultConstructible
     *
     * Construct a probability distribution which is a Dirac in the default-constructed object.
     */
    DiscreteDistribution() = default;
    /**
     * \brief Constructor
     */
    explicit DiscreteDistribution(const param_type &p) : m_param(p)
    {
    }
    /**
     * \brief Copy constructor.
     */
    DiscreteDistribution(DiscreteDistribution<State> const &other) = default;
    /**
     * \brief Move constructor.
     */
    DiscreteDistribution(DiscreteDistribution<State> &&other) = default;
    /**
     * \brief Constructor.
     */
    DiscreteDistribution(std::vector<result_type> const &support, std::vector<double> const &weights)
        : m_param(support, weights)
    {
    }
    /**
     * \brief Constructor.
     */
    DiscreteDistribution(std::vector<result_type> &&support, std::vector<double> &&weights) noexcept
        : m_param(std::move(support), std::move(weights))
    {
    }
    /**
     * \brief Copy assignment operator.
     */
    DiscreteDistribution<State> &operator=(DiscreteDistribution<State> const &other) = default;
    /**
     * \brief Move assignment operator.
     */
    DiscreteDistribution<State> &operator=(DiscreteDistribution<State> &&other) = default;
    /** \brief Generates random objects that are distributed according to the associated parameter set. The entropy is
     * acquired by calling g.operator(). \tparam Generator must meet the requirements of <a
     * href="http://en.cppreference.com/w/cpp/concept/UniformRandomBitGenerator">UniformRandomBitGenerator</a>. \param g
     * an uniform random bit generator object
     */
    template <typename Generator> result_type operator()(Generator &g) const
    {
        std::discrete_distribution<int> d(m_param.weights().cbegin(), m_param.weights().cend());
        return m_param.support()[d(g)];
    }
    /** \brief Generates random numbers that are distributed according to params
     * The entropy is acquired by calling g.operator().
     * \tparam Generator must meet the requirements of <a
     * href="http://en.cppreference.com/w/cpp/concept/UniformRandomBitGenerator">UniformRandomBitGenerator</a>. \param g
     * an uniform random bit generator object \param params	distribution parameter set to use instead of the associated
     * one
     */
    template <typename Generator> result_type operator()(Generator &g, const param_type &params) const
    {
        std::discrete_distribution<int> d(params.weights().cbegin(), params.weights().cend());
        return params.support()[d(g)];
    }
}; // end DiscreteDistribution
} // namespace quetzal::utils::random

#endif
