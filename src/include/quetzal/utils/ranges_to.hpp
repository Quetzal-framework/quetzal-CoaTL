// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/////////////////////////////////////////////////////////////////////////// *
/// This program is free software; you can redistribute it and/or modify ///
/// it under the terms of the GNU General Public License as published by ///
/// the Free Software Foundation; either version 2 of the License, or    ///
/// (at your option) any later version.                                  ///
///                                                                     ///
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <ranges>

namespace quetzal::utils
{
namespace details
{
    // Type acts as a tag to find the correct operator| overload
    template <typename C>
    struct to_helper {
    };
    
    // This actually does the work
    template <typename Container, std::ranges::range R>
    requires std::convertible_to<std::ranges::range_value_t<R>, typename Container::value_type>
    Container operator|(R&& r, to_helper<Container>) 
    {
        return Container{r.begin(), r.end()};
    }
}

// Couldn't find an concept for container, however a
// container is a range, but not a view.
template <std::ranges::range Container>
requires (!std::ranges::view<Container>)
auto to() {
    return details::to_helper<Container>{};
}
}