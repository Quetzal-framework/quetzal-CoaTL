#!/bin/bash

g++ -o UTest -Wall -std=c++14 EnvironmentalQuantity_test.cpp -I/usr/include/gdal  -L/usr/lib/ -lgdal
./UTest > EnvironmentalQuantity_test.output
rm UTest
