#!/bin/bash

echo -e "UTest_utils\n|"


echo -e "|--\t Tree_test..................................."
g++ -o Tree_test -Wall -std=c++14 Tree_test.cpp
./Tree_test
rm Tree_test
echo -e "|--\t DONE \n|"

echo -e "|--\t Forest_test..................................."
g++ -o Forest_test -Wall -std=c++14 Forest_test.cpp
./Forest_test
rm Forest_test
echo -e "|--\t DONE \n|"

echo -e "DONE\n"
