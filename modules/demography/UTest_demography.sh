#!/bin/bash

echo -e "UTest_demography\n|"

cd test/PopulationSize

echo -e "|--\t PopulationSize_test....................................."
./UTest.sh
echo -e "|--\t DONE \n|"

cd ../PopulationFlux

echo -e "|--\t PopulationFlux_test....................................."
./UTest.sh
echo -e "|--\t DONE \n|"

cd ../Flow

echo -e "|--\t Flow_test....................................."
./UTest.sh
echo -e "|--\t DONE \n|"

cd ../SymmetricDistanceMatrix

echo -e "|--\t SymmetricDistanceMatrix_test....................................."
./UTest.sh
echo -e "|--\t DONE \n|"

cd ../History

echo -e "|--\t History_test....................................."
./UTest.sh
echo -e "|--\t DONE \n|"

echo -e "DONE\n"
