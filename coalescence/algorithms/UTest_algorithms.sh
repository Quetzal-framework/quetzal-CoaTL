#!/bin/bash

echo -e "UTest_algorithms\n|"


echo -e "|--\t merge_test..................................."
g++ -o Utest -Wall -std=c++14 merge_test.cpp
./Utest
rm Utest
echo -e "|--\t DONE \n|"

echo -e "DONE\n"
