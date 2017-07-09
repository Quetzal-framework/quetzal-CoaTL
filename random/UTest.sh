#!/bin/bash

echo -e "UTest_random\n|"

echo -e "DiscreteDistribution\n|"

cd test/DiscreteDistribution
./UTest.sh
cd ../

echo -e "DONE\n"
