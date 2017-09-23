#!/bin/bash

g++ -o UTest -Wall -std=c++14 TransitionKernel_test.cpp
./UTest > TransitionKernel_test.output
rm UTest

g++ -o UTest -Wall -std=c++14 initialization_test.cpp
./UTest > initialization_test.output
rm UTest
