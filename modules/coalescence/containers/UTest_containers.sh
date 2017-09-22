#!/bin/bash

echo -e "UTest_containers\n|"

cd test/Tree

echo -e "|--\t Tree_test..................................."
./UTest.sh
echo -e "|--\t DONE \n|"

cd ../Forest

echo -e "|--\t Forest_test..................................."
./UTest.sh
echo -e "|--\t DONE \n|"

echo -e "DONE\n"
