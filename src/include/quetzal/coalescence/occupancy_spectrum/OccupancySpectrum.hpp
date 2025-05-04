// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/*********************************************************************** * This program is free software; you can
 *redistribute it and/or modify * it under the terms of the GNU General Public License as published by * the Free
 *Software Foundation; either version 2 of the License, or    * (at your option) any later version. *
 *                                                                      *
 ***************************************************************************/

#ifndef __OCCUPANCY_SPECTRUM_CLASS_H_INCLUDED__
#define __OCCUPANCY_SPECTRUM_CLASS_H_INCLUDED__

#include <random>
#include <utility> // std::forward, std::move
#include <vector>

namespace quetzal
{
namespace coalescence
{
namespace occupancy_spectrum
{
///
/// @brief An occupancy spectrum as defined in [Becheler & Knowles,
/// 2020](https://academic.oup.com/bioinformatics/article/36/10/3279/5734642).
///
/// @details To understand the implementation of occupancy spectrum concept as
///          applied to the coalescent, it is useful to consider the coalescence
///          process as an urn and balls experiment where \f$n\f$ balls (lineages)
///          are randomly assigned to \f$m\f$ urns (parents), each urn having an assignment
///          probability of \f$m−1\f$⁠. If an urn contains \f$r\f$ balls at the end
///          of a random experiment, \f$r\f$ is said to be the occupancy number
///          of the urn (see Johnson and Kotz, 1977).
///
///          The number of urns having
///          the same occupancy number \f$r\f$ is denoted \f$M_r\f$ and the term occupancy spectrum refers to the vector
///          \f$M_0, M_1,\dots , M_n\f$⁠.
///
///          This is an useful abstraction to separate modules that are responsible for randomely producing
///          spectrums (coalescence::occupancy_spectrum::generation_policy) from modules that
///          consume them (coalescence::algorithm::simultaneous_multiple_merge).
class OccupancySpectrum
{
  private:
    /// @typedef Type of the internal representation of an occupancy spectrum
    using occupancy_spectrum_type = std::vector<int>;
    /// @brief The vector of occupancy numbers
    occupancy_spectrum_type M_j;
    /// @brief Number of balls (lineages) in the experiment that generated the spectrum
    int k;
    /// @brief Number of urns (parents) in the experiment that generated the spectrum
    int N;
    ///
    /// @brief Class invariant: number of balls should be conserved
    ///
    bool test_number_of_balls_conservation()
    {
        int sum = 0;
        for (int i = 0; i < this->M_j.size(); ++i)
        {
            assert(this->M_j.at(i) >= 0);
            sum += i * (this->M_j.at(i));
        }
        return (sum == this->k);
    }
    ///
    /// @brief Class invariant: number of urns should be conserved
    ///
    bool test_number_of_urns_conservation()
    {
        int sum = 0;
        for (int i = 0; i < this->M_j.size(); ++i)
        {
            assert(this->M_j.at(i) >= 0);
            sum += this->M_j.at(i);
        }
        return (sum == this->N);
    }

  public:
    //! \typedef value type when iterating along a spectrum
    using value_type = typename occupancy_spectrum_type::value_type;
    //! \typedef iterator type for iterating along a spectrum
    using iterator = typename occupancy_spectrum_type::iterator;
    //! \typedef const iterator type
    using const_iterator = typename occupancy_spectrum_type::const_iterator;
    ///
    /// @brief Constructor by copy
    ///
    /// @param v A vector of occupancy numbers
    /// @param N the number of urns (parents) of the random experience
    /// @param k the number of balls (coalescing lineages) of the random experience
    OccupancySpectrum(std::vector<int> const &v, int k, int N) : M_j(v), k(k), N(N)
    {
        assert(this->N >= 0);
        assert(this->k >= 0);
        assert(test_number_of_balls_conservation());
        assert(test_number_of_urns_conservation());
    }
    ///
    /// @brief Move constructor
    ///
    /// @param v A vector of occupancy numbers
    /// @param N the number of urns (parents) of the random experience
    /// @param k the number of balls (coalescing lineages) of the random experience
    OccupancySpectrum(std::vector<int> &&v, int k, int N) : M_j(std::move(v)), k(k), N(N)
    {
        assert(this->N >= 0);
        assert(this->k >= 0);
        assert(test_number_of_balls_conservation());
        assert(test_number_of_urns_conservation());
    }
    ///
    /// @brief Deleted default constructor
    ///
    /// @remark this would break class invariants as there is no good way to initialize by default
    OccupancySpectrum() = delete;
    ///
    /// @brief Default copy constructor
    ///
    /// @param other An occupancy spectrum to copy content from
    OccupancySpectrum(const OccupancySpectrum &other) = default;
    ///
    /// @brief Move constructor
    ///
    /// @param other An occupancy spectrum to move content from
    ///
    OccupancySpectrum(OccupancySpectrum &&other) noexcept : M_j(std::move(other.M_j)), N(other.N), k(other.k)
    {
        assert(this->N >= 0);
        assert(this->k >= 0);
        assert(test_number_of_balls_conservation());
        assert(test_number_of_urns_conservation());
    }
    ///
    /// @brief Copy assignment operator
    ///
    OccupancySpectrum &operator=(const OccupancySpectrum &other)
    {
        OccupancySpectrum otherCopy(other);
        this->M_j = std::move(otherCopy.M_j);
        this->N = otherCopy.N;
        this->k = otherCopy.k;
        return *this;
    }
    ///
    /// @brief Move assignment operator
    ///
    /// @param other the spectrum to move data from
    ///
    /// @remark other will be emptied and can not be reused.
    ///
    OccupancySpectrum &operator=(OccupancySpectrum &&other)
    {
        this->M_j = std::move(other.M_j);
        this->N = other.N;
        this->k = other.k;
        return *this;
    }
    ///
    ///@brief returns iterator to the beginning of the spectrum
    ///
    iterator begin()
    {
        return M_j.begin();
    }
    ///
    ///@brief returns iterator to the end of the spectrum
    ///
    iterator end()
    {
        return M_j.end();
    }
    ///
    ///@brief returns iterator to the beginning of the spectrum
    ///
    const_iterator begin() const
    {
        return M_j.begin();
    }
    ///
    ///@brief returns iterator to the end of the spectrum
    ///
    const_iterator end() const
    {
        return M_j.end();
    }
    ///
    ///@brief returns iterator to the beginning of the spectrum
    ///
    const_iterator cbegin() const
    {
        return M_j.cbegin();
    }
    ///
    ///@brief returns iterator to the end of the spectrum
    ///
    const_iterator cend() const
    {
        return M_j.cend();
    }
    ///
    ///@brief erase a range of elements of the spectrum.
    ///
    void erase(iterator first, iterator last)
    {
        M_j.erase(first, last);
    }
    ///
    ///@brief Returns whether the spectrum is empty (i.e. whether its size is 0).
    ///
    bool empty() const
    {
        return M_j.empty();
    }
}; // end class OccupancySpectrum
} // end namespace occupancy_spectrum
} // end namespace coalescence
} // end namespace quetzal

#endif
