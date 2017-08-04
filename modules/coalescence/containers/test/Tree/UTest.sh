#!/bin/bash

g++ -o UTest -Wall -std=c++14 initialization_test.cpp
./UTest > initialization_test.output
rm UTest

g++ -o UTest -Wall -std=c++14 pre_order_DFS_test.cpp
./UTest > pre_order_DFS_test.output
rm UTest

g++ -o UTest -Wall -std=c++14 access_leaves_by_DFS_test.cpp
./UTest > access_leaves_by_DFS_test.output
rm UTest
