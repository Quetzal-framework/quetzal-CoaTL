// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/*********************************************************************** * This program is free software; you can
 *redistribute it and/or modify * it under the terms of the GNU General Public License as published by * the Free
 *Software Foundation; either version 2 of the License, or    * (at your option) any later version. *
 *                                                                      *
 ***************************************************************************/

#include "gtest/gtest.h"

#include "assert.h"
#include <iostream>
#include <vector>

#include "quetzal/polymorphism/fuzzy_transfer_distance.hpp"

TEST(PartitionerTest, construct_all_m_blocks_partitions_of_the_set_by_algorithm_u)
{
    using quetzal::polymorphism::fuzzy_transfer_distance::Partitioner;
    using quetzal::polymorphism::fuzzy_transfer_distance::RestrictedGrowthString;

    std::vector<unsigned int> set = {1, 2, 3, 4, 5};

    Partitioner partitioner(set);

    auto const &three_blocks_partitions = partitioner.construct_all_m_blocks_partitions_of_the_set_by_algorithm_u(3);
    ASSERT_EQ(three_blocks_partitions.size(), 25);

    for (auto const &it : three_blocks_partitions)
    {
        ASSERT_EQ(it.size(), 5);
    }

    // http://www.computing-wisdom.com/computingreadings/fasc3b.pdf : page 28, 7.2.1.5
    std::vector<std::vector<unsigned int>> expectations = {
        {0, 0, 0, 1, 2}, {0, 0, 1, 1, 2}, {0, 1, 1, 1, 2}, {0, 1, 0, 1, 2}, {0, 1, 0, 0, 2},
        {0, 1, 1, 0, 2}, {0, 0, 1, 0, 2}, {0, 0, 1, 2, 2}, {0, 1, 1, 2, 2}, {0, 1, 0, 2, 2},
        {0, 1, 2, 2, 2}, {0, 1, 2, 1, 2}, {0, 1, 2, 0, 2}, {0, 1, 2, 0, 1}, {0, 1, 2, 1, 1},
        {0, 1, 2, 2, 1}, {0, 1, 0, 2, 1}, {0, 1, 1, 2, 1}, {0, 0, 1, 2, 1}, {0, 0, 1, 2, 0},
        {0, 1, 1, 2, 0}, {0, 1, 0, 2, 0}, {0, 1, 2, 2, 0}, {0, 1, 2, 1, 0}, {0, 1, 2, 0, 0}};

    std::vector<RestrictedGrowthString> expected_three_blocks_partitions;
    for (auto const &it : expectations)
    {
        expected_three_blocks_partitions.emplace_back(it);
    }

    ASSERT_EQ(three_blocks_partitions, expected_three_blocks_partitions);
}

TEST(PartitionerTest, construct_all_m_blocks_partitions_of_the_set_by_algorithm_u_set_size_4)
{
    using quetzal::polymorphism::fuzzy_transfer_distance::Partitioner;
    using quetzal::polymorphism::fuzzy_transfer_distance::RestrictedGrowthString;

    std::vector<unsigned int> other = {1, 2, 3, 4};
    Partitioner partitioner2(other);
    auto const &ps = partitioner2.construct_all_m_blocks_partitions_of_the_set_by_algorithm_u(3);
    /* Expected :
    [0, 0, 1, 2]
    [0, 1, 1, 2]
    [0, 1, 0, 2]
    [0, 1, 2, 2]
    [0, 1, 2, 1]
    [0, 1, 2, 0]
    */
    ASSERT_EQ(ps.size(), 6);
}

TEST(PartitionerTest, construct_all_m_blocks_partitions_of_the_set_by_algorithm_u_set_size_4_sorted)
{
    using quetzal::polymorphism::fuzzy_transfer_distance::Partitioner;
    using quetzal::polymorphism::fuzzy_transfer_distance::RestrictedGrowthString;

    std::set<int> other = {1, 2, 3, 4};
    Partitioner partitioner3(other);
    auto all = partitioner3.construct_all_m_blocks_partitions_of_the_set_by_algorithm_u(3);
    ASSERT_EQ(all.size(), 6);
}

TEST(PartitionerTest, construct_all_m_blocks_partitions_of_the_set_by_algorithm_u_set_size_3)
{
    using quetzal::polymorphism::fuzzy_transfer_distance::Partitioner;
    using quetzal::polymorphism::fuzzy_transfer_distance::RestrictedGrowthString;

    std::set<int> other = {1, 2, 3};
    Partitioner partitioner4(other);
    auto all2 = partitioner4.construct_all_m_blocks_partitions_of_the_set_by_algorithm_u(3);
    ASSERT_EQ(all2.size(), 1);
}

TEST(PartitionerTest, construct_all_m_blocks_partitions_of_the_set_by_algorithm_u_set_size_4_nb_blocks_4)
{
    using quetzal::polymorphism::fuzzy_transfer_distance::Partitioner;
    using quetzal::polymorphism::fuzzy_transfer_distance::RestrictedGrowthString;

    std::set<int> other = {1, 2, 3, 4};
    Partitioner partitioner5(other);
    auto all3 = partitioner5.construct_all_m_blocks_partitions_of_the_set_by_algorithm_u(4);
    ASSERT_EQ(all3.size(), 1);
}

