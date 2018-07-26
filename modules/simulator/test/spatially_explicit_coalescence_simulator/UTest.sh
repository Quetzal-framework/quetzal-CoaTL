#!/bin/bash

g++ -o UTest -Wall -std=c++17 mass_based_simulator_test.cpp
./UTest > mass_based_simulator_test.output
rm UTest
