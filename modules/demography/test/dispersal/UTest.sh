#!/bin/bash

g++ -o UTest -Wall -std=c++14 dispersal_test.cpp
./UTest > dispersal.output
rm UTest
