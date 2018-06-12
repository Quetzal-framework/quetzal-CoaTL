#!/bin/bash

g++ -o UTest -Wall -std=c++14 SymmetricDistanceMatrix_test.cpp
./UTest > SymmetricDistanceMatrix.output
rm UTest
