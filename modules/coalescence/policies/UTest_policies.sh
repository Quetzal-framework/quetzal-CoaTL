#!/bin/bash

echo -e "UTest_policies\n|"

cd test/BinaryMerger

echo -e "|--\t BinaryMerger_test..................................."
./UTest.sh
echo -e "|--\t DONE \n|"

cd ../SimultaneousMultipleMerger

echo -e "|--\t SimultaneousMultipleMerger_test..................................."
./UTest.sh
echo -e "|--\t DONE \n|"

echo -e "DONE\n"
