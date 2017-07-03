#!/bin/bash

g++ -o UTest -Wall -std=c++14 Generator_test.cpp
./UTest > Generator_test.output
rm UTest
