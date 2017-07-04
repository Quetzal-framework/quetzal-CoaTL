#!/bin/bash

g++ -o UTest -Wall -std=c++14 PopulationSize_test.cpp
./UTest > PopulationSize_test.output
rm UTest
