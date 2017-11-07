#!/bin/bash

g++ -o UTest -Wall -std=c++14 merger_test.cpp
./UTest > merger_test.output
rm UTest
