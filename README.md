# quetzal
C++ template library for coalescence

# Environmental demogenetic model

- AUTHOR : Arnaud Becheler, Florence Jornod
- DATE   : 2016
- LICENCE : This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.    
- CONTACT : Arnaud.Becheler@egce.cnrs-gif.fr
- WEBSITE : https://becheler.github.io/
- DOCUMENTATION : https://becheler.github.io/quetzalAPI/html/index.html

## Description
Environmental demogenetic model for study of invasion processes. Since biological invasions are demographic process well-delimited in space and time, with reasonably short range, we can hope to learn about biological processes from genetic data, using coalescence-based approach and Approximate Bayesian Computation (ABC). ABC aims at simulating a dataset from a randomly drawn parametrized model, then computing a distance between the observed dataset and the simulation in order to reject the parameters if the distance is above a given threshold. We account here for migration pattern and heterogeneous environment.

This github repository is basically the implementation of the probabilistic model of biological invasion developed during my PhD thesis under the direction of Stephane Dupas (laboratoire d'Evolution, Génomes, Comportements et Ecologie de Gif-sur-Yvette) and Camille Coron (Laboratoire de Mathématiques d'Orsay). It is mostly written in C++ but makes use of shell script for running unit tests and very few lines of C for GDAL API encapsulation.

## Informations

The present project was tested with:

###### OS
- Distributor ID: Ubuntu
- Description: Ubuntu 16.04.1 LTS
- Release: 16.04
- Codename: xenial

###### g++
gcc version 5.4.0 20160609 (Ubuntu 5.4.0-6ubuntu1~16.04.2)

## Dependencies

- Boost: ```sudo apt-get install libboost-all-dev```

## Test Driven Development
For Test Driven Development, bash scripts allow to run all the tests of the module at once, ensuring they are no regression. The sub-modules tests can be run independently.
Run the executable ```UTest<...>``` : this will recursively execute the test of the sub-modules.

## Acknowledgements

- Florence Jornod who was a tremendous intern and add much to the project.
- Ambre Marques for his help, tips and the implementation of the expressive library.
- Arnaud Le Rouzic for help and tips.
- The developpez.com community, for his large help. This project would have failed without the constant support of Bousk, ternel, Koala, dalfab, Medinoc, jo_link_noir and many others...
