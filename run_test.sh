#!/bin/bash

echo -e "Tests\n|"

cd modules

cd geography
./UTest_geography.sh
cd ../

cd demography
./UTest_demography.sh
cd ../

cd random
./UTest_random.sh
cd ../

cd coalescence
./UTest_coalescence.sh
cd ../

echo -e "DONE\n"
