# Polymorphism statistics

This section describes how to use Quetzal.Accumulators to perform
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

- statistics the user is not interested in should be *free* (no extra space or runtime cost)
- the statistical framework should be extensible to new/exotic markers and genetic systems
- it should be easy for a user with a new statistics to incorporate it in the framework
- statistical analysis should *digest* the data, not hold onto them.

## Implementation

The Quetzal Accumulators extends the same Boost Accumulators framework to
perform incremental calculations. Usage of the framework follows the following pattern:

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


Using accumulator_set<>
Our tour of the accumulator_set<> class template begins with the forward declaration:

template< typename Sample, typename Features, typename Weight = void >
struct accumulator_set;
The template parameters have the following meaning:


Sample
The type of the data that will be accumulated.

Features
An MPL sequence of features to be calculated.

Weight
The type of the (optional) weight paramter.

For example, the following line declares an accumulator_set<> that will accept a sequence of doubles one at a time and calculate the min and mean:

accumulator_set< double, features< tag::min, tag::mean > > acc;
Notice that we use the features<> template to specify a list of features to be calculated. features<> is an MPL sequence of features.

[Note]	Note
features<> is a synonym of mpl::vector<>. In fact, we could use mpl::vector<> or any MPL sequence if we prefer, and the meaning would be the same.

Once we have defined an accumulator_set<>, we can then push data into it, and it will calculate the quantities you requested, as shown below.

// push some data into the accumulator_set ...
acc(1.2);
acc(2.3);
acc(3.4);
Since accumulator_set<> defines its accumulate function to be the function call operator, we might be tempted to use an accumulator_set<> as a UnaryFunction to a standard algorithm such as std::for_each. That's fine as long as we keep in mind that the standard algorithms take UnaryFunction objects by value, which involves making a copy of the accumulator_set<> object. Consider the following:

// The data for which we wish to calculate statistical properties:
std::vector< double > data( /* stuff */ );

// The accumulator set which will calculate the properties for us:
accumulator_set< double, features< tag::min, tag::mean > > acc;

// Use std::for_each to accumulate the statistical properties:
acc = std::for_each( data.begin(), data.end(), acc );
Notice how we must assign the return value of std::for_each back to the accumulator_set<>. This works, but some accumulators are not cheap to copy. For example, the tail and tail_variate<> accumulators must store a std::vector<>, so copying these accumulators involves a dynamic allocation. We might be better off in this case passing the accumulator by reference, with the help of boost::bind() and boost::ref(). See below:

// The data for which we wish to calculate statistical properties:
std::vector< double > data( /* stuff */ );

// The accumulator set which will calculate the properties for us:
accumulator_set< double, features< tag::tail<left> > > acc(
    tag::tail<left>::cache_size = 4 );

// Use std::for_each to accumulate the statistical properties:
std::for_each( data.begin(), data.end(), bind<void>( ref(acc), _1 ) );
Notice now that we don't care about the return value of std::for_each() anymore because std::for_each() is modifying acc directly.

[Note]	Note
To use boost::bind() and boost::ref(), you must #include <boost/bind.hpp> and <boost/ref.hpp>

Extracting Results
Once we have declared an accumulator_set<> and pushed data into it, we need to be able to extract results from it. For each feature we can add to an accumulator_set<>, there is a corresponding extractor for fetching its result. Usually, the extractor has the same name as the feature, but in a different namespace. For example, if we accumulate the tag::min and tag::max features, we can extract the results with the min and max extractors, as follows:

// Calculate the minimum and maximum for a sequence of integers.
accumulator_set< int, features< tag::min, tag::max > > acc;
acc( 2 );
acc( -1 );
acc( 1 );

// This displays "(-1, 2)"
std::cout << '(' << min( acc ) << ", " << max( acc ) << ")\n";
The extractors are all declared in the boost::accumulators::extract namespace, but they are brought into the boost::accumulators namespace with a using declaration.

[Tip]	Tip
On the Windows platform, min and max are preprocessor macros defined in WinDef.h. To use the min and max extractors, you should either compile with NOMINMAX defined, or you should invoke the extractors like: (min)( acc ) and (max)( acc ). The parentheses keep the macro from being invoked.

Another way to extract a result from an accumulator_set<> is with the extract_result() function. This can be more convenient if there isn't an extractor object handy for a certain feature. The line above which displays results could equally be written as:

// This displays "(-1, 2)"
std::cout << '('  << extract_result< tag::min >( acc )
          << ", " << extract_result< tag::max >( acc ) << ")\n";
Finally, we can define our own extractor using the extractor<> class template. For instance, another way to avoid the min / max macro business would be to define extractors with names that don't conflict with the macros, like this:

extractor< tag::min > min_;
extractor< tag::min > max_;

// This displays "(-1, 2)"
std::cout << '(' << min_( acc ) << ", " << max_( acc ) << ")\n";
Passing Optional Parameters
Some accumulators need initialization parameters. In addition, perhaps some auxiliary information needs to be passed into the accumulator_set<> along with each sample. Boost.Accumulators handles these cases with named parameters from the Boost.Parameter library.

For example, consider the tail and tail_variate<> features. tail keeps an ordered list of the largest N samples, where N can be specified at construction time. Also, the tail_variate<> feature, which depends on tail, keeps track of some data that is covariate with the N samples tracked by tail. The code below shows how this all works, and is described in more detail below.

// Define a feature for tracking covariate data
typedef tag::tail_variate< int, tag::covariate1, left > my_tail_variate_tag;

// This will calculate the left tail and my_tail_variate_tag for N == 2
// using the tag::tail<left>::cache_size named parameter
accumulator_set< double, features< my_tail_variate_tag > > acc(
    tag::tail<left>::cache_size = 2 );

// push in some samples and some covariates by using
// the covariate1 named parameter
acc( 1.2, covariate1 =  12 );
acc( 2.3, covariate1 = -23 );
acc( 3.4, covariate1 =  34 );
acc( 4.5, covariate1 = -45 );

// Define an extractor for the my_tail_variate_tag feature
extractor< my_tail_variate_tag > my_tail_variate;

// Write the tail statistic to std::cout. This will print "4.5, 3.4, "
std::ostream_iterator< double > dout( std::cout, ", " );
std::copy( tail( acc ).begin(), tail( acc ).end(), dout );

// Write the tail_variate statistic to std::cout. This will print "-45, 34, "
std::ostream_iterator< int > iout( std::cout, ", " );
std::copy( my_tail_variate( acc ).begin(), my_tail_variate( acc ).end(), iout );
There are several things to note about the code above. First, notice that we didn't have to request that the tail feature be calculated. That is implicit because the tail_variate<> feature depends on the tail feature. Next, notice how the acc object is initialized: acc( tag::tail<left>::cache_size = 2 ). Here, cache_size is a named parameter. It is used to tell the tail and tail_variate<> accumulators how many samples and covariates to store. Conceptually, every construction parameter is made available to every accumulator in an accumulator set.

We also use a named parameter to pass covariate data into the accumulator set along with the samples. As with the constructor parameters, all parameters to the accumulate function are made available to all the accumulators in the set. In this case, only the accumulator for the my_tail_variate feature would be interested in the value of the covariate1 named parameter.

We can make one final observation about the example above. Since tail and tail_variate<> are multi-valued features, the result we extract for them is represented as an iterator range. That is why we can say tail( acc ).begin() and tail( acc ).end().

Even the extractors can accept named parameters. In a bit, we'll see a situation where that is useful.

Weighted Samples
Some accumulators, statistical accumulators in particular, deal with data that are weighted. Each sample pushed into the accumulator has an associated weight, by which the sample is conceptually multiplied. The Statistical Accumulators Library provides an assortment of these weighted statistical accumulators. And many unweighted statistical accumulators have weighted variants. For instance, the weighted variant of the sum accumulator is called weighted_sum, and is calculated by accumulating all the samples multiplied by their weights.

To declare an accumulator_set<> that accepts weighted samples, you must specify the type of the weight parameter as the 3rd template parameter, as follows:

// 3rd template parameter 'int' means this is a weighted
// accumulator set where the weights have type 'int'
accumulator_set< int, features< tag::sum >, int > acc;
When you specify a weight, all the accumulators in the set are replaced with their weighted equivalents. For example, the above accumulator_set<> declaration is equivalent to the following:

// Since we specified a weight, tag::sum becomes tag::weighted_sum
accumulator_set< int, features< tag::weighted_sum >, int > acc;
When passing samples to the accumulator set, you must also specify the weight of each sample. You can do that with the weight named parameter, as follows:

acc(1, weight = 2); //   1 * 2
acc(2, weight = 4); //   2 * 4
acc(3, weight = 6); // + 3 * 6
                    // -------
                    // =    28
You can then extract the result with the sum() extractor, as follows:

// This prints "28"
std::cout << sum(acc) << std::endl;
[Note]	Note
When working with weighted statistical accumulators from the Statistical Accumulators Library, be sure to include the appropriate header. For instance, weighted_sum is defined in <boost/accumulators/statistics/weighted_sum.hpp>.
