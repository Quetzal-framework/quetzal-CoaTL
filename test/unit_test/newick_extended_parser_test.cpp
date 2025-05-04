#include <gtest/gtest.h>

#include "quetzal/io/newick/extended/to_network.hpp"

#include <string_view>

namespace x3 = boost::spirit::x3;

class FixtureCases : public ::testing::Test
{
protected:
    void SetUp() override
    {
        name_cases = {"a34", "A856BC", "k56aH"};

        length_cases = {":1", ":999", ":0.001"};

        leaf_cases = {"", "A", "B897kj"};

        branch_cases = {
            "leaf",
            "(in,ternal)",
            "leaf:10.0",
            "(in,ternal):50.0",
        };

        internal_cases = {"(,)", "(A,B)F", "(A:10,B:10)F"};

        subtree_cases = {"leaf",
                         "(in,ternal)",
                         "(,,(,))",
                         "(A,B,(C,D))",
                         "(A,B,(C,D)E)F",
                         "(:0.1,:0.2,(:0.3,:0.4):0.5)",
                         "(:0.1,:0.2,(:0.3,:0.4):0.5)",
                         "(A:0.1,B:0.2,(C:0.3,D:0.4):0.5)",
                         "(A:0.1,B:0.2,(C:0.3,D:0.4)E:0.5)F",
                         "((B:0.2,(C:0.3,D:0.4)E:0.5)F:0.1)A"};

        tree_standard_cases = {";",
                               "(,);",
                               "(,,(,));",
                               "(A,B,(C,D));",
                               "(A,B,(C,D)E)F;",
                               "(:0.1,:0.2,(:0.3,:0.4):0.5);",
                               "(:0.1,:0.2,(:0.3,:0.4):0.5):0.0;",
                               "(A:0.1,B:0.2,(C:0.3,D:0.4):0.5);",
                               "(A:0.1,B:0.2,(C:0.3,D:0.4)E:0.5)F;",
                               "((B:0.2,(C:0.3,D:0.4)E:0.5)F:0.1)A;"};

        network_cases = {"((1, ((2, (3, (4)Y#H1)g)e, (((Y#H1, 5)h, 6)f)X#H2)c)a, ((X#H2, 7)d, 8)b)r;",
                         "(A,B,((C,(Y)x#H1)c,(x#H1,D)d)e)f"};
    }

    void TearDown() override {}

    std::vector<std::string_view> name_cases;
    std::vector<std::string_view> length_cases;
    std::vector<std::string_view> leaf_cases;
    std::vector<std::string_view> branch_cases;
    std::vector<std::string_view> internal_cases;
    std::vector<std::string_view> subtree_cases;
    std::vector<std::string_view> tree_standard_cases;
    std::vector<std::string_view> network_cases;
};

TEST_F(FixtureCases, name_grammar)
{
    for (const auto &input : name_cases)
    {
        auto iter = input.begin();
        auto iter_end = input.end();
        bool r = x3::parse(iter, iter_end, quetzal::format::newick::parser::name);
        EXPECT_TRUE(r && iter == iter_end);
    }
}

TEST_F(FixtureCases, length_grammar)
{
    for (const auto &input : length_cases)
    {
        auto iter = input.begin();
        auto iter_end = input.end();
        bool r = x3::parse(iter, iter_end, quetzal::format::newick::parser::length);
        EXPECT_TRUE(r && iter == iter_end);
    }
}

TEST_F(FixtureCases, leaf_grammar)
{
    for (const auto &input : leaf_cases)
    {
        auto iter = input.begin();
        auto iter_end = input.end();
        bool r = x3::parse(iter, iter_end, quetzal::format::newick::parser::leaf);
        EXPECT_TRUE(r && iter == iter_end);
    }
}

TEST_F(FixtureCases, tree_standard_grammar)
{
    for (const auto &input : tree_standard_cases)
    {
        auto iter = input.begin();
        auto iter_end = input.end();
        bool r = x3::parse(iter, iter_end, quetzal::format::newick::parser::tree);
        EXPECT_TRUE(r && iter == iter_end);
    }
}

// TEST_F(FixtureCases, tree_exotic_grammar)
// {
//   for (const auto& input : tree_exotic_cases)
//   {
//     auto iter = input.begin();
//     auto iter_end = input.end();
//     bool r = x3::parse(iter, iter_end, quetzal::format::newick::parser::tree);
//     EXPECT_TRUE(r && iter == iter_end);
//   }
// }

