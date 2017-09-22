#!/bin/bash

g++ -o UTest -Wall -std=c++14 PopulationFlux_test.cpp
./UTest > PopulationFlux_test.output
rm UTest
