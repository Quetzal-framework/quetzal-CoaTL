#include <gtest/gtest.h>
#include <quetzal/coalescence.hpp>

TEST(coalescence_algorithms, binary_merge)
{
    using node_type = int;
    std::vector<node_type> nodes = {1, 1, 1, 1};
    std::mt19937 rng;
    using quetzal::coalescence::algorithm::binary_merge;
    // First coalescence using default parent initialization and operator
    auto last = binary_merge(nodes.begin(), nodes.end(), rng);
    std::copy(nodes.begin(), last, std::ostream_iterator<node_type>(std::cout, " "));
    std::cout << std::endl;
    // Second coalescence using custom initialization and operator
    last = binary_merge(nodes.begin(), last, 100, std::minus<node_type>(), rng);
    std::copy(nodes.begin(), last, std::ostream_iterator<node_type>(std::cout, " "));
}

TEST(coalescence_algorithms, simultaneous_multiple_merge)
{
    using node_type = int;
    std::vector<node_type> nodes = {1, 1, 1, 1, 1};
    // 99 parents with no child, 0 parent with 1 child, 1 parent with 2 children, 1 parent with 3 children
    std::vector<unsigned int> spectrum = {98, 0, 1, 1};
    std::mt19937 rng;
    using quetzal::coalescence::algorithm::simultaneous_multiple_merge;
    auto last = simultaneous_multiple_merge(nodes.begin(), nodes.end(), spectrum, rng);
    std::copy(nodes.begin(), last, std::ostream_iterator<node_type>(std::cout, " "));
}

TEST(coalescence_containers, forest)
{
    using position_type = int;
    using tree_type = std::string;
    using forest_type = quetzal::coalescence::container::Forest<position_type, tree_type>;
    forest_type forest;
    auto summarize = [](auto const &forest) {
        std::cout << "Forest has " << forest.nb_trees() << " trees at " << forest.positions().size()
                  << " geographic positions." << std::endl;
    };
    auto print = [](auto const &forest) {
        for (auto const &it : forest)
        {
            std::cout << "Position: " << it.first << "\t"
                      << "Species: " << it.second << std::endl;
        }
    };
    // Insert some trees at some positions
    std::vector<tree_type> trees = {"populus_tremula", "salix_nigra"};
    forest.insert(1, std::move(trees));
    forest.insert(2, std::string("populus_tremula"));
    forest.insert(3, std::string("quercus_robur"));
    summarize(forest);
    print(forest);
    std::cout << "\nErase coordinate 1:" << std::endl;
    forest.erase(1);
    summarize(forest);
    print(forest);
}

TEST(coalescence_containers, forest_init)
{
    using position_type = int;
    using tree_type = int;
    using forest_type = quetzal::coalescence::container::Forest<position_type, tree_type>;
    // initialization by default constructor
    forest_type a;
    // initialization by copy constructor
    forest_type b(a);
    // initialization by move constructor
    forest_type c(std::move(b)); // b should not be used again
    // Also initialization by copy constructor
    forest_type d = a;
    // assignment by copy assignment operator
    d = c;
    // assignment by move assignment operator
    d = std::move(c);
}

TEST(coalescence_containers, forest_insert)
{
    // \remark These are very basic types sufficient for testing, but have little to see with coalescence.
    using position_type = int;
    using std::string;
    using tree_type = string;
    using forest_type = quetzal::coalescence::container::Forest<position_type, tree_type>;
    forest_type forest;
    // Insert some trees at some positions
    forest.insert(1, string("salix_nigra"));
    std::vector<string> u{"populus_tremula", "salix_nigra"};
    forest.insert(2, u);
    std::vector<string> v{"quercus_robur", "salix_nigra"};
    forest.insert(3, std::move(v));
    auto it = forest.insert(4, string("salix_nigra"));
    EXPECT_EQ(it->first, 4);
    EXPECT_EQ(forest.nb_trees(), 6);
    EXPECT_EQ(forest.nb_trees(3), 2);
    for (auto const &it : forest)
    {
        std::cout << "Position " << it.first << " : " << it.second << std::endl;
    }
}

TEST(coalescence_containers, tree_init)
{
    using quetzal::coalescence::container::Tree;
    using std::string;
    // initialization by default constructor
    Tree<int> a;
    Tree<int> b;
    Tree<int> c;
    // assignment by copy assignment operator
    a = b;
    // assignment by move assignment operator
    c = std::move(b); // b should not be used anymore !
    int cell = 12;
    // initialization copying cell
    Tree<int> d(cell);
    // initialization moving cell
    Tree<int> e(std::move(cell)); // cell should not be used anymore !
}

TEST(coalescence_containers, tree_DFS)
{
    using quetzal::coalescence::container::Tree;
    using std::string;

    /* Topology :
     *             a
     *           /\ \
     *          b  d e
     *         /      \\
     *        c       f g
     */

    Tree<string> root("a");
    root.add_child("b").add_child("c");
    root.add_child("d");
    auto &e = root.add_child("e");
    e.add_child("f");
    e.add_child("g");
    std::vector<string> expected = {"c", "d", "f", "g"};
    std::vector<string> v;
    auto f = [&v](string s) { v.push_back(s); };
    root.visit_leaves_cells_by_DFS(f);
    EXPECT_EQ(v, expected);
}

TEST(coalescence_containers, tree_preorder_DFS)
{
    using quetzal::coalescence::container::Tree;
    using std::string;
    /* Topology :
     *             a
     *           /\ \
     *          b  d e
     *         /      \\
     *        c       f g
     */

    Tree<string> root("a");
    root.add_child("b").add_child("c");
    root.add_child("d");
    auto &e = root.add_child("e");
    e.add_child("f");
    e.add_child("g");
    std::vector<string> expected = {"a", "b", "c", "d", "e", "f", "g"};
    std::vector<string> v;
    auto f = [&v](string s) { v.push_back(s); };
    root.visit_cells_by_pre_order_DFS(f);
    EXPECT_EQ(v, expected);
}

