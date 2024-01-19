// Copyright 2022 Arnaud Becheler    <arnaud.becheler@gmail.com>

/*********************************************************************** * This program is free software; you can
 *redistribute it and/or modify * it under the terms of the GNU General Public License as published by * the Free
 *Software Foundation; either version 2 of the License, or    * (at your option) any later version. *
 *                                                                      *
 ***************************************************************************/

#ifndef __QUETZAL_ACCUMULATOR_H_INCLUDED__
#define __QUETZAL_ACCUMULATOR_H_INCLUDED__

#include <boost/accumulators/framework/accumulator_base.hpp>
#include <boost/accumulators/framework/parameters/sample.hpp>

///
/// @brief Putting the accumulator implementations in this namespace is recommended.
///
/// @details This namespace pulls in any operators defined in the boost::numeric::operators
///          namespace with a using directive. The Numeric Operators Sub-Library
///          defines some additional overloads that will make your accumulators
///          work with all sorts of data types.
///
namespace boost::accumulators::impl
{

template <typename Sample> class tajima_accumulator : accumulator_base
{
  public:
    ///
    /// @brief The type returned by result() below.
    ///
    typedef Sample result_type;

    ///
    /// @brief Constructor with a Boost.Parameter argument pack.
    ///
    /// @note Maybe there is an initial value in the argument pack.
    ///       ('sample' is defined in sample.hpp, included above.)
    template <typename Args> tajima_accumulator(Args const &args) : sum(args[sample | Sample()])
    {
    }

    ///
    /// @brief The accumulate function is the function call operator, accepting an argument pack.
    ///
    template <typename Args> void operator()(Args const &args)
    {
        this->tajima += args[sample];
    }

    ///
    /// @brief The result function will also be passed an argument pack, but we don't use it here.
    ///
    template <typename Args> result_type result(Args const &args) const
    {
        using tajimasD = quetzal::polymophism::statistics::tajimasD;
        return tajimasD(mean_pairwise_differences(args[accumulator]), nb_segregating_site(args[accumulator]),
                        count(args[accumulator]));
    }

  private:
    Sample sum;
};

template <typename Sample> struct mean_accumulator : accumulator_base
{
    typedef Sample result_type;
    mean_accumulator(dont_care)
    {
    }

    template <typename Args> result_type result(Args const &args) const
    {
        return sum(args[accumulator]) / count(args[accumulator]);
    }
};

} // namespace boost::accumulators::impl

namespace boost::accumulators::tag
{
struct segregating_sites;
struct pairwise_differences;

///
/// @brief Tajima's statistics accumulator tag
///
struct tajima : depends_on<count, pairwise_differences, segregating_sites>
{
    // Define a nested typedef called 'impl' that specifies which accumulator implements this feature.
    typedef accumulators::impl::tajima_accumulator<mpl::_1> impl;
};
} // namespace boost::accumulators::tag

namespace boost::accumulators
{
namespace extract
{
///
/// @brief Define the tajima extractor
///
extractor<tag::tajima> const tajima = {};
} // namespace extract

///
/// @brief Pull the extractor into the enclosing namespace.
///
using extract::tajima;

} // namespace boost::accumulators

#endif
