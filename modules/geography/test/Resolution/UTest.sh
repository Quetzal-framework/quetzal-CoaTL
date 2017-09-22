#!/bin/bash

g++ -o UTest -Wall -std=c++14 Resolution_test.cpp
./UTest > Resolution_test.output
rm UTest
