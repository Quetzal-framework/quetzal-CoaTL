#!/bin/bash

cd test/GeographicCoordinates

echo -e "|--\t GeographicCoordinates_test....................................."
./UTest.sh
echo -e "|--\t DONE \n|"


cd ../Extent

echo -e "|--\t Extent_test....................................."
./UTest.sh
echo -e "|--\t DONE \n|"


cd ../Resolution

echo -e "|--\t Resolution_test....................................."
./UTest.sh
echo -e "|--\t DONE \n|"

cd ../EnvironmentalQuantity

echo -e "|--\t EnvironmentalQuantity_test....................................."
./UTest.sh
echo -e "|--\t DONE \n|"

echo -e "DONE\n"
