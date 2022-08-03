#include <iostream>
#include <quetzal/accumulators/accumulators.h>
#include <quetzal/accumulators/statistics/stats.h>
#include <quetzal/accumulators/statistics/tajima.h>
using namespace boost::accumulators;

int main()
{
    // Define an accumulator set for calculating the mean and the
    // 2nd moment ...
    accumulator_set<double, stats<tag::mean, tag::moment<2> > > acc;

    // push in some data ...
    acc(1.2);
    acc(2.3);
    acc(3.4);
    acc(4.5);

    // Display the results ...
    std::cout << "Mean:   " << mean(acc) << std::endl;
    std::cout << "Moment: " << accumulators::moment<2>(acc) << std::endl;

    return 0;
}
