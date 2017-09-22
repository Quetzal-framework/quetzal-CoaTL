#!/bin/bash

g++ -o UTest -Wall -std=c++14 Extent_test.cpp
./UTest > Extent_test.output
rm UTest
