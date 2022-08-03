# Polymorphism statistics

This section describes how to use the Quetzal.Accumulators to perform
incremental statistical computations on genetic data.

## Hello world!

Below is a complete example of how to use the Accumulators Framework and the
Statistical Accumulators to perform an incremental statistical calculation.
It calculates the Tajima's D and its dependent statistics, add internal variables
to the cache and also compute FST.

include example/stats.cpp

Expected output:

include example/results/stats.txt

## Motivations

To our knowledge, there is no existing resource that can compute summary statistics
on genetic data in a satisfying way.

By satisfying, we mean that:

- statistics the user is not interested in should be *free* (no space or runtime cost)
- the statistical framework should be extensible to new/exotic markers and genetic systems
- it should be easy for a user with a new statistics to incorporate it in the framework
- statistical analysis should *digest* the data, not hold onto them.
implementation
The Quetzal Accumulators Framework extends the same Boost framework for
performing incremental calculations. Usage of the framework follows the following pattern:

1. Users build a computational object, called an `accumulator_set<>`,
   by selecting the computations in which they are interested, or authoring
   their own computational primitives which fit within the framework.
2. Users push data into the `accumulator_set<>` object one sample at a time.
3. The `accumulator_set<>` computes the requested quantities in the most efficient
   method possible, resolving dependencies between requested calculations, possibly
   caching intermediate results.
4. The Accumulators Framework defines the utilities needed for defining primitive
   computational elements, called accumulators. It also provides the `accumulator_set<>` type, described above.

## Terminology

For sake of consistency, we use the same terms as Boost Accumulators in the rest of the documentation.

- **Sample**: A datum that is pushed into an `accumulator_set<>`. The type of the sample is the sample type.

- **Weight**: An optional scalar value passed along with the sample specifying
the weight of the sample. Conceptually, each sample is multiplied with its weight.
The type of the weight is the weight type.

- **Feature**: An abstract primitive computational entity. When defining an `accumulator_set<>`, users specify the features in which they are interested, and the `accumulator_set<>` figures out which accumulators would best provide those features. Features may depend on other features. If they do, the accumulator set figures out which accumulators to add to satisfy the dependencies.

- **Accumulator**: A concrete primitive computational entity. An accumulator is a concrete implementation of a feature. It satisfies exactly one abstract feature. Several different accumulators may provide the same feature, but may represent different implementation strategies.

- **Accumulator Set**: A collection of accumulators. An accumulator set is specified with a sample type and a list of features. The accumulator set uses this information to generate an ordered set of accumulators depending on the feature dependency graph. An accumulator set accepts samples one datum at a time, propagating them to each accumulator in order. At any point, results can be extracted from the accumulator set.

- **Extractor**: A function or function object that can be used to extract a result from an `accumulator_set<>`.

## Overview

Here is a list of the important types and functions in the Accumulator Framework and a
brief description of each.

| Tool                	| Description                                                                                                                                                                                                                                                                                                                                                	|
|---------------------	|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------	|
| `accumulator_set<>` 	| This is the most important type in the Accumulators Framework. <br>It is a collection of accumulators. A datum pushed into an<br>`accumulator_set<>` is forwarded to each accumulator, in an<br>order determined by the dependency relationships between the <br>accumulators. Computational results can be extracted from an <br>accumulator at any time. 	|
| `depends_on<>`      	| Used to specify which other features a feature depends on.                                                                                                                                                                                                                                                                                                 	|
| `feature_of<>`      	| Trait used to tell the Accumulators Framework that, for the <br>purpose of feature-based dependency resolution, one feature <br>should be treated the same as another.                                                                                                                                                                                     	|
| `as_feature<>`      	| Used to create an alias for a feature. For example, if there<br>are two features, `fast_X` and `accurate_X`, they can be mapped<br>to `X(fast)` and `X(accurate)` with `as_feature<>`. This is just<br>syntactic sugar.                                                                                                                                    	|
| `features<>`        	| An MPL sequence. We can use `features<>` as the second template<br>parameter when declaring an accumulator_set<>.                                                                                                                                                                                                                                          	|
| `external<>`        	| Used when declaring an `accumulator_set<>`. If the weight type <br>is specified with `external<>`, then the weight accumulators <br>are assumed to reside in a separate accumulator set which will<br>be passed in with a named parameter.                                                                                                                 	|
| `extractor<>`       	| A class template useful for creating an extractor function <br>object. It is parameterized on a feature, and it has member <br>functions for extracting from an `accumulator_set<>` the <br>result corresponding to that feature.                                                                                                                          	|
