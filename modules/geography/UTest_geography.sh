#!/bin/bash

echo -e "UTest_geography\n|"

echo -e "|--\t GeographicCoordinates_test..................................."
g++ -o GeographicCoordinates_test GeographicCoordinates_test.cpp -std=c++14 -Wall
./GeographicCoordinates_test
rm GeographicCoordinates_test
echo -e "|--\t DONE \n|"


echo -e "|--\t gdalcpp_test..................................."
g++ -o gdalcpp_test gdalcpp_test.cpp -std=c++14 -Wall -I/usr/include/gdal  -L/usr/lib/ -lgdal
./gdalcpp_test
rm gdalcpp_test
echo -e "|--\t DONE \n|"

echo -e "|--\t EnvironmentalQuantity_test..................................."
g++ -o EnvironmentalQuantity_test EnvironmentalQuantity_test.cpp -std=c++1y -Wall -I/usr/include/gdal  -L/usr/lib/ -lgdal
./EnvironmentalQuantity_test
rm EnvironmentalQuantity_test
echo -e "|--\t DONE \n|"

echo -e "|--\t DiscreteLandscape_test..................................."
g++ -o DiscreteLandscape_test DiscreteLandscape_test.cpp -std=c++1y -Wall -I/usr/include/gdal  -L/usr/lib/ -lgdal
./DiscreteLandscape_test
rm DiscreteLandscape_test
echo -e "|--\t DONE \n|"

echo -e "|--\t DONE \n|"
