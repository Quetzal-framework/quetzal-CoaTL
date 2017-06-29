#!/bin/bash

echo -e "UTest_occupancy_spectrum\n|"

echo -e "|--\t Generator_test..................................."
g++ -o Utest -Wall -std=c++14 Generator_test.cpp
./Utest
rm Utest
echo -e "|--\t DONE \n|"

echo -e "|--\t OccupancySpectrumDistribution_test..................................."
g++ -o Utest -Wall -std=c++14 OccupancySpectrumDistribution_test.cpp
./Utest
rm Utest
echo -e "|--\t DONE \n|"

echo -e "|--\t memoize_test..................................."
g++ -o memoize_test -Wall -std=c++14 memoize_test.cpp
./memoize_test
rm memoize_test
echo -e "|--\t DONE \n|"

echo -e "|--\t spectrum_creation_policy_test..................................."
g++ -o spectrum_creation_policy_test -Wall -std=c++14 spectrum_creation_policy_test.cpp
./spectrum_creation_policy_test
rm spectrum_creation_policy_test
echo -e "|--\t DONE \n|"

echo -e "DONE\n"
