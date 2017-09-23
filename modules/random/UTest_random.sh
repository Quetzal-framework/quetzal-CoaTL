#!/bin/bash

echo -e "UTest_random\n|"

cd test/DiscreteDistribution

echo -e "|--\t DiscreteDistribution_test....................................."
./UTest.sh
echo -e "|--\t DONE \n|"

cd ../TransitionKernel

echo -e "|--\t TransitionKernel_test....................................."
./UTest.sh
echo -e "|--\t DONE \n|"

cd ../TimeTransitionKernel

echo -e "|--\t TimeTransitionKernel_test....................................."
./UTest.sh
echo -e "|--\t DONE \n|"

echo -e "DONE\n"
