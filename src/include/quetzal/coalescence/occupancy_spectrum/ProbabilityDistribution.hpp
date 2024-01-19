// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/************************************************************************ * This program is free software; you can
 *redistribute it and/or modify * it under the terms of the GNU General Public License as published by * the Free
 *Software Foundation; either version 2 of the License, or    * (at your option) any later version. *
 *                                                                      *
 ***************************************************************************/

#ifndef __OCCUPANCY_SPECTRUM_DISTRIBUTION_H_INCLUDED__
#define __OCCUPANCY_SPECTRUM_DISTRIBUTION_H_INCLUDED__

#include "Support.hpp"
#include "editor_policy.hpp"
#include "filter_policy.hpp"

#include <assert.h>
#include <cmath>    // std::pow
#include <iostream> // << operator implementation
#include <random>   // discrete_distribution
#include <utility>  // std::forward, std::move
#include <vector>

#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/random.hpp> // boost::multiprecision::pow

namespace quetzal
{
namespace coalescence
{
namespace occupancy_spectrum
{
///
/// @brief Occupancy spectrum distributed according
///        to the probability function given by von Mises (1939).
///
/// @tparam FilterPolicy Function object class acting as a unary predicate, taking
///         the probability of the spectrum as argument and returning a boolean.
///         The signature should be `bool FilterPolicy::operator()(double)`
///
/// @tparam EditorPolicy Policy class to edit the occupancy spectrum for optimal speed and
///         memory usage. The class should expose a member with signature
///        `OccupancySpectrum EditorPolicy::edit(OccupancySpectrum &&)`.
///
/// @remark The probability of an occupancy spectrum is described in the book
///         "Urn models and their application: an approach
///         to modern discrete probability theory" (by John and Kotz, 1977, p.115).
///
/// @remark The number of possible configurations grows very fast with \f$n\f$ and \f$m\f$,
///         so memorizing all of them quickly becomes intractable.
///         As most of the configurations have extremely low probability,
///         you can choose to use a filter_policy class to filter the spectrum
///         according to their sampling
///         probability, keeping only those which probability is superior to a
///         given threshold (or any other arbitrary criterion).
///         - If the predicate is evaluated
///         to true, the spectrum and its probability will be saved in the
///         distribution.
///         - If the predicate is evaluated to false, the spectrum
///         and its probability will be discarded.
///
/// @remark By default, the filter_policy::return_always_true
///         filter policy is used, so all spectra are kept.
///
/// @remark By default the editor_policy::identity policy is used and wil not modify the spectrum.
///         The editor_policy::truncate_tail will remove all the last urns that are
///         empty, making later iteration over a spectrum faster.
///
template <class FilterPolicy = filter_policy::return_always_true, class EditorPolicy = editor_policy::identity>
class ProbabilityDistribution
{
  private:
    /// @typedef Type to represent big integers in factorial function
    using BigInt = boost::multiprecision::cpp_int;
    /// @typedef Type to represent big float probabilities
    using BigFloat = boost::multiprecision::cpp_dec_float_50;
    /// @typedef Self type
    using self_type = ProbabilityDistribution<FilterPolicy, EditorPolicy>;
    /// @typedef Internal representation of an occupancy spectrum
    using spectrum_type = OccupancySpectrum;
    /// @typedef Representation of the support of the occupancy spectrum distribution
    using support_type = std::vector<spectrum_type>;
    /// @typedef Representation of the occupancy spectrum distribution probabilities
    using probabilities_type = std::vector<double>;
    /// @brief Number of balls (lineages) used in the random experience
    int m_k;
    /// @brief Number of urns (parents) used in the random experience
    int m_N;
    /// @brief Filter policy used
    FilterPolicy m_pred;
    /// @brief Set of occupancy spectrums
    support_type m_support;
    /// @brief Probabilities associated to the support
    probabilities_type m_probas;
    /// @brief Internal representation of the random sampler
    mutable std::discrete_distribution<size_t> m_distribution;

  public:
    ///
    /// @brief Default constructor
    ///
    ProbabilityDistribution() = default;
    ///
    /// @brief Construct the occupancy spectrum distribution resulting from throwing \f$n\f$ balls into \f$m\f$ urns.
    ///
    /// @remark Object can be huge if large \f$k\f$
    ///
    /// @param n number of balls (lineages)
    /// @param m number of urns (parents)
    /// @param pred A filter policy, that is a unary predicate
    ///
    ProbabilityDistribution(int n, int m, FilterPolicy pred = FilterPolicy()) : m_k(n), m_N(m), m_pred(pred)
    {
        auto callback = make_callback(m_k, m_N);
        Support s(m_k, m_N);
        s.generate(callback);
        m_distribution = std::discrete_distribution<size_t>(m_probas.cbegin(), m_probas.cend());
        assert(m_support.size() == m_probas.size());
    }
    ///
    /// @brief Copy constructor
    ///
    ProbabilityDistribution(const self_type &) = delete;
    ///
    /// @brief Move constructor
    ///
    ProbabilityDistribution(self_type &&) = default;
    ///
    /// @brief Move assignment operator
    ///
    self_type &operator=(self_type &&) = default;
    ///
    /// @brief Deleted copy assignment operator
    ///
    /// @remark Deleted for avoiding copying huge amount of data.
    ///
    self_type &operator=(const self_type &) = delete;
    ///
    /// @brief Generates random occupancy spectrum that are distributed according
    ///        to the associated probability function given by von Mises (1939).
    ///
    /// @tparam Generator A random number generator typename
    ///
    /// @param g The random number generator.
    ///
    template <typename Generator> const spectrum_type &operator()(Generator &g) const
    {
        return m_support[m_distribution(g)];
    }
    ///
    /// @brief The number of balls (coalescing lineages) that are thrown in the random experience
    ///
    int n() const
    {
        return m_k;
    }
    ///
    /// @brief The number of urns (parents) used in the random experience
    ///
    int m() const
    {
        return m_N;
    }
    ///
    /// @brief Get a constant reference on the support of occupancy spectrum distribution
    ///
    const support_type &support() const
    {
        return m_support;
    }
    ///
    /// @brief Get a constant reference the weights associated to each spectrum.
    ///
    /// @remark As filters can be applied, weights do not necessarily sum to 1 before sampling.
    ///
    const probabilities_type &weights() const
    {
        return m_probas;
    }
    ///
    /// @brief Computes the probability of a given spectrum for a given experiment
    ///
    static double compute_probability(int n, int m, spectrum_type const &m_js)
    {
        BigFloat numerator(Factorial(m) * Factorial(n));
        BigFloat denominator(1.0);
        int j = 0;
        for (auto m_j : m_js)
        {
            denominator *= boost::multiprecision::pow(BigFloat(Factorial(j)), BigFloat(m_j)) * BigFloat(Factorial(m_j));
            ++j;
        }
        denominator *= BigFloat(std::pow(m, n));
        BigFloat result = numerator / denominator;
        return static_cast<double>(result);
    }
    ///
    /// @brief Stream operator to print the distribution support and probabilities.
    ///
    /// @remark As filters can be applied, weights do not necessarily sum to 1 before sampling.
    ///
    friend std::ostream &operator<<(std::ostream &os, self_type const &dist)
    {
        for (int i = 0; i < dist.support().size(); ++i)
        {
            // os << "P( \t" << dist.support()[i] << "\t) = " << dist.weights()[i] << "\n";
            auto v = dist.support()[i];
            os << "P( \t";
            if (!v.empty())
            {
                os << '[';
                std::copy(v.begin(), v.end(), std::ostream_iterator<int>(os, ", "));
                os << "\b\b]";
            }
            os << "\t) = " << dist.weights()[i] << "\n";
        }
        return os;
    }

  private:
    ///
    /// @brief Produces a callback passed to the Support class, that automatically filters and builds the distribution.
    ///
    auto make_callback(int k, int N)
    {
        return [this, k, N](spectrum_type &&M_j) {
            auto p = compute_probability(k, N, M_j);
            if (this->m_pred(p))
            {
                this->m_support.push_back(EditorPolicy::edit(std::move(M_j)));
                this->m_probas.push_back(p);
            }
        };
    }
    ///
    /// @brief Factorial function, using large integers
    ///
    static BigInt Factorial(int num)
    {
        BigInt fact = 1;
        for (int i = num; i > 1; --i)
        {
            fact *= i;
        }
        return fact;
    }
};
} // namespace occupancy_spectrum
} // namespace coalescence
} // namespace quetzal

#endif
