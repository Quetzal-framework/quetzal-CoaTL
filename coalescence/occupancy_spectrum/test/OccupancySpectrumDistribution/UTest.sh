#!/bin/bash

g++ -o UTest -Wall -std=c++14 initialization_test.cpp
./UTest > initialization_test.output
rm UTest

g++ -o UTest -Wall -std=c++14 OccupancySpectrumDistribution_test.cpp
./UTest > OccupancySpectrumDistribution_test.output
rm UTest
