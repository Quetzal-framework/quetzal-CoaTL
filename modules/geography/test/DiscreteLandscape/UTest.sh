#!/bin/bash

g++ -o UTest -Wall -std=c++14 DiscreteLandscape_test.cpp -I/usr/include/gdal  -L/usr/lib/ -lgdal
./UTest > DiscreteLandscape_test.output
rm UTest
