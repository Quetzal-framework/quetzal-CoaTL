#!/bin/bash

echo -e "UTest_random\n|"

cd test/spatially_explicit_coalescence_simulator

echo -e "|--\t SpatiallyExplicitCoalescenceSimulator_test....................................."
./UTest.sh
echo -e "|--\t DONE \n|"

echo -e "DONE\n"
