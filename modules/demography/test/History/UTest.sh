#!/bin/bash

g++ -o UTest -Wall -std=c++14 History_test.cpp
./UTest > History_test.output
rm UTest
