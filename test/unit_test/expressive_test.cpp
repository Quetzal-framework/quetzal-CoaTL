// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/*********************************************************************** * This program is free software; you can
 *redistribute it and/or modify * it under the terms of the GNU General Public License as published by * the Free
 *Software Foundation; either version 2 of the License, or    * (at your option) any later version. *
 *                                                                      *
 ***************************************************************************/
#include <gtest/gtest.h>

#include <quetzal/expressive.hpp>

using namespace quetzal::expressive;

TEST(Expressive, UsingLambdasAndLiterals)
{
    using quetzal::expressive::use;
    using quetzal::expressive::literal_factory;

    literal_factory<int, int> lit;
    auto g = lit(5);

    auto f = [](int a, int b) { return a + b; };

    auto h = use(f) + g;
    EXPECT_EQ(h(2, 3), 10);
}

TEST(Expressive, ComposingFunctors)
{
    using quetzal::expressive::use;
    using quetzal::expressive::compose;

    // NxN -> N -> string
    auto inner = use([](int a, int b) { return a + b; });
    auto outer = use([](int) { return "c"; });

    auto composed = compose(outer, inner);

    EXPECT_EQ(composed(1, 2), "c");
}


