#!/bin/bash

echo -e "UTest_containers\n|"


echo -e "|--\t Tree_test..................................."
g++ -o Utest -Wall -std=c++14 Tree_test.cpp
./Utest
rm Utest
echo -e "|--\t DONE \n|"

echo -e "|--\t Forest_test..................................."
g++ -o Utest -Wall -std=c++14 Forest_test.cpp
./Utest
rm Utest
echo -e "|--\t DONE \n|"

echo -e "DONE\n"
