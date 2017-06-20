#!/bin/bash

echo -e "UTest_coalescence\n|"

cd utils
./UTest_utils.sh
cd ../

cd occupancy_spectrum
./UTest_occupancy_spectrum.sh
cd ../

echo -e "|--\t merge_algorithm_test..................................."
g++ -o merge_algorithm_test -Wall -std=c++14 merge_algorithm_test.cpp
./merge_algorithm_test
rm merge_algorithm_test
echo -e "|--\t DONE \n|"

echo -e "|--\t merger_test..................................."
g++ -o merger_test -Wall -std=c++14 merger_test.cpp
./merger_test
rm merger_test
echo -e "|--\t DONE \n|"

echo -e "DONE\n"
