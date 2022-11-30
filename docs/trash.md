
:seedling: Infering populations ecological features (such as migration or growth
patterns) from genetic datasets can be a complex task. In some settings it is
actually a mathematically intractable problem, so simulations methods are needed!

For example, Approximate Bayesian Computation (ABC) can be used to update current
knowledge on the processes shaping a genetic dataset by simulating the underlying
gene trees (coalescents) in an explicit geographic space.

Existing softwares like
[SPLATCHE](http://splatche.com/),
[simcoal2](http://cmpg.unibe.ch/software/simcoal2/),
[egglib](http://mycor.nancy.inra.fr/egglib/index.html), or
[msprime](http://msprime.readthedocs.io/en/stable/index.html) are very useful and
user-friendly resources that should be used whenever possible to achieve this task.

However if you are working on developing some border case simulation model, or
if you are not comfortable with hypothesis of the existing software solutions,
you will surely consider to build your own program :grimacing:

And that's why we need something that looks like a standard.

:v: Quetzal can help you doing so by offering atomic components (structures,
algorithms, concepts) that can be easily reused to build a new program.
The template mechanism allow to adapt them efficiently to each particular situation
you may encounter.

Although the initial purpose of the library was mostly directed towards landscape
simulations, my intend is to propose general-purpose components for coalescence-simulations:
- generic containers for representing coalescent trees, forests (sets of trees), phylogenetic networks...
- generic algorithms (like DFS, BFS) to apply on containers
- policies to "furnish" the containers and specialize the containers (e.g. the many Newick format variants)
- polymoprhism statics running on abstract genealogies
