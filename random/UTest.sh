#!/bin/bash

echo -e "UTest_random\n|"

echo -e "DiscreteDistribution\n|"
cd test/DiscreteDistribution
./UTest.sh
cd ../
echo -e "DONE\n"

cd ../

echo -e "TransitionKernel\n|"
cd test/TransitionKernel
./UTest.sh
cd ../
echo -e "DONE\n"

cd ../

echo -e "TimeTransitionKernel\n|"
cd test/TimeTransitionKernel
./UTest.sh
cd ../
echo -e "DONE\n"
