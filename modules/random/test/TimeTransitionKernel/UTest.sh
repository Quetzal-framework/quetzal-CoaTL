#!/bin/bash

g++ -o UTest -Wall -std=c++14 TimeTransitionKernel_test.cpp
./UTest > TimeTransitionKernel_test.output
rm UTest

g++ -o UTest -Wall -std=c++14 initialization_test.cpp
./UTest > initialization_test.output
rm UTest
