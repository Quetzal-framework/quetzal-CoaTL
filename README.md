# Quetzal-CoalTL <img align="right" width="200" src="https://github.com/Becheler/quetzal-CoalTL/blob/media/quetzal.png">

![GitHub release (latest by date)](https://img.shields.io/github/v/release/Becheler/quetzal-CoalTL)
[![Becheler](https://circleci.com/gh/Becheler/quetzal-CoaTL.svg?style=shield)](https://app.circleci.com/pipelines/github/Becheler/quetzal-CoaTL)
[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Website becheler.github.io](https://img.shields.io/website-up-down-green-red/https/becheler.github.io.svg)](https://becheler.github.io/softwares/quetzal-CoalTL/home/)
![Lines of code](https://img.shields.io/tokei/lines/github/Becheler/quetzal-CoalTL)
![GitHub last commit](https://img.shields.io/github/last-commit/Becheler/quetzal-CoalTL)
# A Coalescence Template Libary

Quetzal-CoalTL is a C++ template scientific library for simulating gene genealogies in explicit landscapes.

:egg::egg::egg:  If you're a not a programmer, you may be interested by [Quetzal-EGGS simulators](https://github.com/Becheler/quetzal-EGGS)!

:seedling: Infering populations ecological features (such as migration or growth patterns) from genetic datasets can be a complex task. In some settings it is actually a mathematically intractable problem, so simulations methods are needed ! Approximate Bayesian Computation (ABC) can be used to update your knowledge about the processes shaping your genetic dataset by simulating the underlying gene trees (coalescents) in an explicit geographic space.

Existing softwares like [SPLATCHE](http://splatche.com/), [simcoal2](http://cmpg.unibe.ch/software/simcoal2/), [egglib](http://mycor.nancy.inra.fr/egglib/index.html), or [msprime](http://msprime.readthedocs.io/en/stable/index.html) are very useful and user-friendly resources that should be used whenever possible to achieve this task.

However if you are working on developing some border case simulation model, or if you are not comfortable with hypothesis of the existing software solutions, you will surely consider to build your own program :grimacing:

:v: Quetzal can help you doing so by offering atomic components (structures, algorithms, concepts) that can be easily reused to build a new program. The template mechanism allow to adapt them efficiently to each particular situation you may encounter.

- :crystal_ball: looking forward, we expect to develop the library related to genetic simulation and [C++20 concepts!](https://en.cppreference.com/w/cpp/language/constraints)
- :email: You are interested? Want to contribute? Want to give some feedback? Don't be shy, [contact me!](https://lsa.umich.edu/eeb/people/postdoctoral-fellows/arnaud-becheler.html)
- :star: You think this is a cool project? Drop a star on GitHub :point_up:
- :bug: A bug? Oopsie daisy! I'll fix it asap if you [email me](https://lsa.umich.edu/eeb/people/postdoctoral-fellows/arnaud-becheler.html) or open an issue :point_up:

![Quetzal-CoalTL concepts](https://github.com/Becheler/quetzal-CoalTL/blob/media/quetzal_scheme.png)

# Website

Please visit [Quetzal website](https://becheler.github.io/softwares/quetzal-CoalTL/home/) for more details and:
 - [user tutorials](https://becheler.github.io/softwares/quetzal-CoalTL/tutorials/)
 - [developer documentation](https://becheler.github.io/softwares/quetzal-CoalTL/API/html/index.html)
 - [FAQ](https://becheler.github.io/softwares/quetzal-CoalTL/FAQ/)

# Author

This github repository is basically the implementation of the probabilistic model of biological invasion developed during my PhD thesis under the direction of Stephane Dupas (laboratoire d'Evolution, Génomes, Comportements et Ecologie de Gif-sur-Yvette) and Camille Coron (Laboratoire de Mathématiques d'Orsay).

- AUTHOR : Arnaud Becheler
- DATE   : 2016
- LICENCE : This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.    
- CONTACT : abechele@umich.edu
- WEBSITE : https://becheler.github.io/

# How to cite:

 Becheler, A, Coron, C, Dupas, S. The Quetzal Coalescence template library: A C++ programmers resource for integrating distributional, demographic and coalescent models. Mol Ecol Resour. 2019; 19: 788– 793. https://doi.org/10.1111/1755-0998.12992

Becheler, A., & Knowles, L. L. (2020). Occupancy spectrum distribution: application for coalescence simulation with generic mergers. Bioinformatics, 36(10), 3279-3280.

# Acknowledgements

- Florence Jornod who was a tremendous intern and add much to the project.
- Ambre Marques for his help, tips and the implementation of the expressive library.
- Arnaud Le Rouzic for help and tips.
- The developpez.com community, for their large help. This project would have failed without the constant support of Bousk, ternel, Koala, bacelar, dalfab, Medinoc, jo_link_noir and many others...
