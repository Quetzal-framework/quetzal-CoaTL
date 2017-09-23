#!/bin/bash

echo -e "UTest_coalescence\n|"

cd containers
./UTest_containers.sh
cd ../

cd occupancy_spectrum
./UTest_occupancy_spectrum.sh
cd ../

cd algorithms
./UTest_algorithms.sh
cd ../

cd policies
./UTest_policies.sh
cd ../

echo -e "DONE\n"
