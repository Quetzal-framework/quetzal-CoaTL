

#include "../coalescence.h"

#include <random>    // std::mt19937
#include <iostream>  // std::cout
#include <iterator>  // std::ostream_iterator
#include <algorithm> // std::copy
#include <string>

int main(){

  using node_type = std::string;
  std::vector<node_type> nodes = {"a","b","c","d"};
  unsigned int N = 100;

  node_type init;

  auto branch = [](node_type parent, node_type child){
    if(parent.size() == 0) return "(" + child;
    else return parent + "," + child + ")";
  };

  std::mt19937 rng;
  auto last  = nodes.end();
  while(std::distance(nodes.begin(), last) > 1){
    std::cout << std::distance(nodes.begin(), last) << std::endl;
    last = coalescence::BinaryMerger::merge(nodes.begin(), last, N, init, branch, rng);
  }

  std::copy(nodes.begin(), last, std::ostream_iterator<node_type>(std::cout, " "));

  return 0;
}
