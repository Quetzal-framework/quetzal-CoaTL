#!/bin/bash

echo -e "UTest_demography\n|"


echo -e "|--\t PopulationSize_test....................................."
g++ -o Utest PopulationSize_test.cpp -std=c++14 -Wall
./Utest
rm Utest
echo -e "|--\t DONE \n|"

echo -e "DONE\n"
