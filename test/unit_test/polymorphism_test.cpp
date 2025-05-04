// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/*********************************************************************** * This program is free software; you can
 *redistribute it and/or modify * it under the terms of the GNU General Public License as published by * the Free
 *Software Foundation; either version 2 of the License, or    * (at your option) any later version. *
 *                                                                      *
 ***************************************************************************/

#include <gtest/gtest.h>

#include <quetzal/polymorphism.hpp>

namespace utf = ::testing;

TEST(polymorphism_statistics, tajimasD)
{
    double mean_pairwise_difference = 3.888889;
    int nb_segregating_sites = 16;
    int nb_sequences = 10;

    // Compute stats
    using quetzal::polymorphism::statistics::tajimasD;
    auto stat = tajimasD(mean_pairwise_difference, nb_segregating_sites, nb_sequences);

    auto a1 = 2.828968;
    auto a2 = 1.539768;
    auto b1 = 0.407407;
    auto b2 = 0.279012;
    auto c1 = 0.053922;
    auto c2 = 0.047227;
    auto e1 = 0.019061;
    auto e2 = 0.004949;
    auto D  = -1.446172;

    EXPECT_NEAR(stat.a1(), a1, 1e-5);
    EXPECT_NEAR(stat.a2(), a2, 1e-5);
    EXPECT_NEAR(stat.b1(), b1, 1e-5);
    EXPECT_NEAR(stat.b2(), b2, 1e-5);
    EXPECT_NEAR(stat.c1(), c1, 1e-5);
    EXPECT_NEAR(stat.c2(), c2, 1e-5);
    EXPECT_NEAR(stat.e1(), e1, 1e-5);
    EXPECT_NEAR(stat.e2(), e2, 1e-5);
    EXPECT_NEAR(stat.D(), D, 1e-5);
}

