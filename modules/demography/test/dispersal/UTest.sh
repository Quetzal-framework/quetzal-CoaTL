#!/bin/bash

g++ -o UTest -Wall -std=c++14 dispersal_test.cpp
./UTest > dispersal_test.output
rm UTest
