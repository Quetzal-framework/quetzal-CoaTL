#!/bin/bash

echo -e "UTest_geography\n|"

echo -e "|--\t Coords_test..................................."
g++ -o Coords_test Coords_test.cpp -std=c++14 -Wall
./Coords_test
rm Coords_test
echo -e "|--\t DONE \n|"


echo -e "|--\t GeoData_test..................................."
g++ -o GeoData_test GeoData_test.cpp -std=c++14 -Wall -I/usr/include/gdal  -L/usr/lib/ -lgdal
./GeoData_test
rm GeoData_test
echo -e "|--\t DONE \n|"


echo -e "|--\t Env_test..................................."
g++ -o Env_test Env_test.cpp -std=c++14 -Wall -I/usr/include/gdal  -L/usr/lib/ -lgdal
./Env_test
rm Env_test
echo -e "|--\t DONE \n|"
