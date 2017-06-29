#!/bin/bash

echo -e "UTest_policies\n|"

echo -e "|--\t merger_test..................................."
g++ -o Utest -Wall -std=c++14 merger_test.cpp
./Utest
rm Utest
echo -e "|--\t DONE \n|"

echo -e "DONE\n"
