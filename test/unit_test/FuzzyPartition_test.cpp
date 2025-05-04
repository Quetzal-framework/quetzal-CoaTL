// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/*********************************************************************** * This program is free software; you can
 *redistribute it and/or modify * it under the terms of the GNU General Public License as published by * the Free
 *Software Foundation; either version 2 of the License, or    * (at your option) any later version. *
 *                                                                      *
 ***************************************************************************/

#include <gtest/gtest.h>

#include "quetzal/polymorphism/fuzzy_transfer_distance.hpp"

using quetzal::polymorphism::fuzzy_transfer_distance::FuzzyPartition;
using quetzal::polymorphism::fuzzy_transfer_distance::RestrictedGrowthString;

TEST(FuzzyPartitionTest, construction)
{
    std::map<int, std::vector<double>> coeffs = {
        {0, {0.0, 0.1, 0.9, 0.0}}, {1, {0.4, 0.1, 0.2, 0.3}}, {2, {0.0, 0.3, 0.6, 0.1}}};

    FuzzyPartition<int> A(coeffs);
    EXPECT_EQ(A.elements(), std::set<int>({0, 1, 2}));
    EXPECT_EQ(A.clusters(), std::set<unsigned int>({0, 1, 2, 3}));
    EXPECT_EQ(A.nElements(), 3);
    EXPECT_EQ(A.nClusters(), 4);
}

TEST(FuzzyPartitionTest, merging_clusters)
{
    std::map<int, std::vector<double>> coeffs = {
        {0, {0.0, 0.1, 0.9, 0.0}}, {1, {0.4, 0.1, 0.2, 0.3}}, {2, {0.0, 0.3, 0.6, 0.1}}};

    FuzzyPartition<int> A(coeffs);
    RestrictedGrowthString RGS({0, 0, 1, 2});
    A.merge_clusters(RGS); // merging cluster 1 and 2, summing membership coefficients
    EXPECT_EQ(A, FuzzyPartition<int>({{0, {0.1, 0.9, 0.0}}, {1, {0.5, 0.2, 0.3}}, {2, {0.3, 0.6, 0.1}}}));
}

TEST(FuzzyPartitionTest, fuzzy_transfer_distance)
{
    std::map<int, std::vector<double>> coeffs1 = {
        {0, {0.0, 0.1, 0.9, 0.0}}, {1, {0.4, 0.1, 0.2, 0.3}}, {2, {0.0, 0.3, 0.6, 0.1}}};
    std::map<int, std::vector<double>> coeffs2 = {
        {0, {0.1, 0.9, 0.0}}, {1, {0.5, 0.2, 0.3}}, {2, {0.3, 0.6, 0.1}}};

    FuzzyPartition<int> A(coeffs1);
    FuzzyPartition<int> B(coeffs2);

    double d = A.fuzzy_transfer_distance(B);
    EXPECT_DOUBLE_EQ(d, 0.4);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

