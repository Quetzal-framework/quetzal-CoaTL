// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/*********************************************************************** * This program is free software; you can
 *redistribute it and/or modify * it under the terms of the GNU General Public License as published by * the Free
 *Software Foundation; either version 2 of the License, or    * (at your option) any later version. *
 *                                                                      *
 ***************************************************************************/
#include <gtest/gtest.h>
#include <quetzal/demography.hpp>

struct transition_matrix
{
    using coord_type = std::string;
    std::vector<coord_type> arrival_space(coord_type)
    {
        return {"A", "B"};
    }
    double operator()(coord_type, coord_type)
    {
        return 0.5; // 1/2 probability to change of location
    }
};

TEST(population_size_default, test_population_size_default)
{
    // Data types declaration
    using coord_type = std::string;
    using time_type = unsigned int;
    using size_type = unsigned int;
    // Initializing some data
    time_type t0 = 2017;
    coord_type x0 = "A";
    size_type N0 = 12;

    quetzal::demography::PopulationSizeHashMapImplementation<coord_type, time_type, size_type> N;
    EXPECT_FALSE(N.is_defined(x0, t0));
    // Set first population size
    N.set(x0, t0, N0);
    EXPECT_TRUE(N.is_defined(x0, t0));
    EXPECT_EQ(N.get(x0, t0), N0);
    EXPECT_EQ(N.definition_space(t0).size(), 1);
    // Set second population time
    N.set("B", t0, 2 * N0);
    EXPECT_EQ(N.definition_space(t0).size(), 2);
    // Retrieve definition space (demes with population size > 0)
    auto X = N.definition_space(t0);
    std::cout << "Distribution area at time t0: {";
    std::copy(X.begin(), X.end(), std::ostream_iterator<coord_type>(std::cout, ","));
    std::cout << "}\n" << std::endl;
}

TEST(population_size_on_disk, test_population_size_on_disk)
{
    // Data types declaration
    using coord_type = std::string;
    using time_type = unsigned int;
    using size_type = unsigned int;
    // Initializing some data
    time_type t0 = 0;
    coord_type x0 = "A";
    size_type N0 = 12;

    quetzal::demography::PopulationSizeOnDiskImplementation<coord_type, time_type, size_type> N;
    EXPECT_FALSE(N.is_defined(x0, t0));
    // Set first population size
    N.set(x0, t0, N0);
    EXPECT_TRUE(N.is_defined(x0, t0));
    EXPECT_EQ(N.get(x0, t0), N0);
    EXPECT_EQ(N.definition_space(t0).size(), 1);
    // Set second population time
    N.set("B", t0, 2 * N0);
    EXPECT_EQ(N.definition_space(t0).size(), 2);
    // Retrieve definition space (demes with population size > 0)
    auto X = N.definition_space(t0);
}

TEST(population_size_on_disk_moving_windows, test_population_size_on_disk_moving_windows)
{
    // Data types declaration
    using coord_type = int;
    using time_type = unsigned int;
    using size_type = unsigned int;
    // Initializing some data
    time_type t0 = 0;
    coord_type x0 = 0;
    size_type N0 = 1;

    quetzal::demography::PopulationSizeOnDiskImplementation<coord_type, time_type, size_type> N;
    EXPECT_FALSE(N.is_defined(x0, t0));
    // Set first population size
    N.set(x0, t0, N0);
    EXPECT_TRUE(N.is_defined(x0, t0));
    EXPECT_EQ(N.get(x0, t0), N0);
    EXPECT_EQ(N.definition_space(t0).size(), 1);
    // Set other population sizes
    for (unsigned int i = 1; i <= 10; ++i)
    {
        N.set(i, i, 2 * N.get(i - 1, i - 1));
    }
    EXPECT_EQ(N.definition_space(10).size(), 1);
    EXPECT_EQ(N.get(10, 10), 1024);
    EXPECT_EQ(N.get(1, 1), 2);
    EXPECT_EQ(N.get(0, 0), 1);
    EXPECT_EQ(N.get(5, 5), 32);
}

TEST(flow, test_flow)
{
    using coord_type = std::string;
    using time_type = unsigned int;
    using value_type = int;

    time_type t = 2017;
    coord_type i = "A";
    coord_type j = "B";

    quetzal::demography::FlowHashMapImplementation<coord_type, time_type, value_type> Phi;

    EXPECT_FALSE(Phi.flow_to_is_defined(i, t));
    Phi.set_flow_from_to(i, j, t, 12);
    Phi.add_to_flow_from_to(j, j, t, 1);
    EXPECT_TRUE(Phi.flow_to_is_defined(j, t));
    EXPECT_EQ(Phi.flow_from_to(i, j, t), 12);
    EXPECT_EQ(Phi.flow_from_to(j, j, t), 1);

    std::cout << "Flows converging to " << j << " at time t=" << t << ":" << std::endl;
    for (auto const &it : Phi.flow_to(j, t))
    {
        std::cout << "From " << it.first << " to " << j << " = " << it.second << std::endl;
    }
}

TEST(flow_on_disk_implementation, test_flow_on_disk_implementation)
{
    using coord_type = std::string;
    using time_type = unsigned int;
    using value_type = int;

    time_type t = 0;
    coord_type i = "A";
    coord_type j = "B";

    quetzal::demography::FlowOnDiskImplementation<coord_type, time_type, value_type> Phi;

    EXPECT_FALSE(Phi.flow_to_is_defined(i, t));
    Phi.set_flow_from_to(i, j, t, 12);
    Phi.add_to_flow_from_to(j, j, t, 1);
    EXPECT_TRUE(Phi.flow_to_is_defined(j, t));
    EXPECT_EQ(Phi.flow_from_to(i, j, t), 12);
    EXPECT_EQ(Phi.flow_from_to(j, j, t), 1);

    std::cout << "Flows converging to " << j << " at time t=" << t << ":" << std::endl;
    for (auto const &it : Phi.flow_to(j, t))
    {
        std::cout << "From " << it.first << " to " << j << " = " << it.second << std::endl;
    }
}

TEST(flow_on_disk_implementation_moving_windows, test_flow_on_disk_implementation_moving_windows)
{
    std::cout << "here" << std::endl;
    using coord_type = unsigned int;
    using time_type = unsigned int;
    using value_type = unsigned int;

    quetzal::demography::FlowOnDiskImplementation<coord_type, time_type, value_type> Phi;

    for (unsigned int t = 0; t <= 5; ++t)
    {
        for (unsigned int from = 0; from <= 5; ++from)
        {
            for (unsigned int to = 0; to <= 5; ++to)
            {
                Phi.set_flow_from_to(from, to, t, from + to + t);
            }
        }
    }

    EXPECT_TRUE(Phi.flow_to_is_defined(5, 5));
    EXPECT_EQ(Phi.flow_from_to(5, 5, 5), 15);
    EXPECT_EQ(Phi.flow_from_to(2, 4, 5), 11);
    EXPECT_EQ(Phi.flow_from_to(0, 0, 4), 4);
    EXPECT_EQ(Phi.flow_from_to(0, 0, 3), 3);
    EXPECT_EQ(Phi.flow_from_to(0, 0, 2), 2);
    EXPECT_EQ(Phi.flow_from_to(0, 0, 1), 1);
    EXPECT_EQ(Phi.flow_from_to(0, 0, 0), 0);
    EXPECT_EQ(Phi.flow_from_to(1, 2, 5), 8);
    EXPECT_EQ(Phi.flow_from_to(0, 0, 0), 0);
}

TEST(individual_based_history_default_storage, test_individual_based_history_default_storage)
{
    //! [individual_based_history_default_storage example]
    // Here we simulate a population oscillation between 2 demes: -1 and 1
    using coord_type = int;
    using generator_type = std::mt19937;
    generator_type gen;
    // Number of non-overlapping generations for the demographic simulation
    coord_type x_0 = 1;
    unsigned int nb_generations = 3; // times: 0 - 1 - 2
    unsigned int N_0 = 10;
    // Declare an individual-based history where each gene copy is dispersed independently
    using quetzal::demography::demographic_policy::individual_based;
    // Declare memory should be allocated only on demand: ideal for very short histories

