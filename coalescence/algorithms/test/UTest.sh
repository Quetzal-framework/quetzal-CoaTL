#!/bin/bash

g++ -o UTest -Wall -std=c++14 binary_merge_test.cpp
./UTest > binary_merge_test.output
rm UTest

g++ -o UTest -Wall -std=c++14 simultaneous_multiple_merge_test.cpp
./UTest > simultaneous_multiple_merge_test.output
rm UTest
