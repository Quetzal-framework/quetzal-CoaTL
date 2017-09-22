#!/bin/bash

echo -e "UTest_abc\n|"

echo -e "|--\t abc_test..................................."
g++ -o abc_test -Wall -std=c++14 abc_test.cpp
./abc_test
rm abc_test
echo -e "|--\t DONE \n|"

echo -e "DONE\n"
