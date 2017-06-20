#!/bin/bash

echo -e "UTest_occupancy_spectrum\n|"

echo -e "|--\t Algorithm_test..................................."
g++ -o Algorithm_test -Wall -std=c++14 Algorithm_test.cpp
./Algorithm_test
rm Algorithm_test
echo -e "|--\t DONE \n|"

echo -e "|--\t occupancy_spectrum_distribution_test..................................."
g++ -o occupancy_spectrum_distribution_test -Wall -std=c++14 occupancy_spectrum_distribution_test.cpp
./occupancy_spectrum_distribution_test
rm occupancy_spectrum_distribution_test
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
