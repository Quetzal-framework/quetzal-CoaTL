#!/bin/bash

g++ -o UTest -Wall -std=c++14 Individual_based_history_test.cpp
./UTest > Individual_based_history_test.output
rm UTest

g++ -o UTest -Wall -std=c++14 Mass_based_history_test.cpp
./UTest > Mass_based_history_test.output
rm UTest
