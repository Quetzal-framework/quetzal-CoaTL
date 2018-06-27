#!/bin/bash

g++ -o UTest -Wall -std=c++14 Flow_test.cpp
./UTest > Flow_test.output
rm UTest
