// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/////////////////////////////////////////////////////////////////////////// *
/// This program is free software; you can redistribute it and/or modify ///
/// it under the terms of the GNU General Public License as published by ///
/// the Free Software Foundation; either version 2 of the License, or    ///
/// (at your option) any later version.                                  ///
///                                                                     ///
///////////////////////////////////////////////////////////////////////////

#ifndef QUETZAL_TREE_UTILS_H_INCLUDED
#define QUETZAL_TREE_UTILS_H_INCLUDED

#include <tuple>

namespace quetzal::coalescence::detail
{
// Get a new tuple containing all but first element of a tuple
template <typename T1, typename... Ts> std::tuple<Ts...> unshift_tuple(const std::tuple<T1, Ts...> &tuple)
{
    return std::apply([](auto &&, const auto &...args) { return std::tie(args...); }, tuple);
}

} // namespace quetzal::coalescence::detail

#endif