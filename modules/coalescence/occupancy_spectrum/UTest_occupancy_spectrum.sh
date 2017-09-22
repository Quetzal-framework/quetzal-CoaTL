#!/bin/bash

echo -e "UTest_occupancy_spectrum\n|"

cd test/Generator/

echo -e "|--\t Generator_test..................................."
./UTest.sh
echo -e "|--\t DONE \n|"

cd ../OccupancySpectrumDistribution

echo -e "|--\t OccupancySpectrumDistribution_test..................................."
./UTest.sh
echo -e "|--\t DONE \n|"

echo -e "DONE\n"
