#include "quetzal/io.hpp"
#include "quetzal/coalescence.hpp"

#include <string>

// Define a namespace alias to shorten notation
namespace newick = quetzal::format::newick;

int main()
{
  // We will build this topology:
  //             a
  //           /\ \
  //          b  d e
  //         /      \\
  //        c       f g
  //

  // Let's import the Quetzal Tree container into the current namespace
  using quetzal::coalescence::container::Tree;
  // A type alias: a cell here will be a simple label string - no branch length
  using cell_type = std::string;
  // Another type alias
  using tree_type = Tree<cell_type>;

  // Building the topology
  tree_type root("a");
  root.add_child("b").add_child("c");
  root.add_child("d");
  auto& e = root.add_child("e");
  e.add_child("f");
  e.add_child("g");

  // Tell the formatter what a label should be for a given tree t
  newick::Formattable<tree_type> auto label = [](const tree_type& t ){return t.cell();};

  // We still have to tell the formatter not to display the branch length
  newick::Formattable<tree_type> auto branch = [](const tree_type& t ){return "";};

  // Since Quetzal Tree and Quetzal make_generator know each other, this is enough information
  // auto generator = newick::make_generator(label, branch);
  //
  // // We can now generate the string
  // auto s = generator.get();
  //
  // // And print it out!
  // std::cout << s << std::endl;

  return 0;
}
