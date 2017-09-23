#!/bin/bash

g++ -o UTest -Wall -std=c++14 GeographicCoordinates_test.cpp
./UTest > GeographicCoordinates_test.output
rm UTest
