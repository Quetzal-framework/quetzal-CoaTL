// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/*********************************************************************** * This program is free software; you can
 *redistribute it and/or modify * it under the terms of the GNU General Public License as published by * the Free
 *Software Foundation; either version 2 of the License, or    * (at your option) any later version. *
 *                                                                      *
 ***************************************************************************/

// g++ -o FuzzyPartition_test FuzzyPartition_test.cpp -std=c++14 -Wall

#include "quetzal/polymorphism/fuzzy_transfer_distance.hpp"

#include "assert.h"
#include <iostream>
#include <map>
#include <set>
#include <vector>

int main()
{
    using quetzal::polymorphism::fuzzy_transfer_distance::FuzzyPartition;
    using quetzal::polymorphism::fuzzy_transfer_distance::RestrictedGrowthString;

    std::map<int, std::vector<double>> coeffs = {
        {0, {0.0, 0.1, 0.9, 0.0}}, {1, {0.4, 0.1, 0.2, 0.3}}, {2, {0.0, 0.3, 0.6, 0.1}}};

    FuzzyPartition<int> A(coeffs);
    assert(A.elements() == std::set<int>({0, 1, 2}));
    assert(A.clusters() == std::set<unsigned int>({0, 1, 2, 3}));
    assert(A.nElements() == 3);
    assert(A.nClusters() == 4);

    auto B = A;
    RestrictedGrowthString RGS({0, 0, 1, 2});
    B.merge_clusters(RGS); // merging cluster 1 and 2, summing membership coefficients
    assert(B == FuzzyPartition<int>({{0, {0.1, 0.9, 0.0}}, {1, {0.5, 0.2, 0.3}}, {2, {0.3, 0.6, 0.1}}}));

    double d = A.fuzzy_transfer_distance(B);
    assert(d == 0.4);

    return 0;
}
