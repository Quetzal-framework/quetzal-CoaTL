#!/bin/bash

g++ -o UTest -Wall -std=c++14 initialization_test.cpp
./UTest > initialization_test.output
rm UTest

g++ -o UTest -Wall -std=c++14 insertion_test.cpp
./UTest > insertion_test.output
rm UTest

g++ -o UTest -Wall -std=c++14 Forest_test.cpp
./UTest > Forest_test.output
rm UTest
