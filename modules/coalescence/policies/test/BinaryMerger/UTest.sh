#!/bin/bash

g++ -o UTest -Wall -std=c++14 test.cpp
./UTest > test.output
rm UTest
