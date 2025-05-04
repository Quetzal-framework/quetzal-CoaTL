#include <gtest/gtest.h>

#include "quetzal/io/newick/parser.hpp"

#include <iomanip>
#include <string>
#include <vector>

namespace x3 = boost::spirit::x3;

class NewickParserTest : public ::testing::Test
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

        tree_exotic_cases = {"(Space In,Leaf)Name", "(Special,Symbol)%;"};

        tree_must_fail_cases = {};
    }

    void TearDown() override {}

    std::vector<std::string> name_cases;
    std::vector<std::string> length_cases;
    std::vector<std::string> leaf_cases;
    std::vector<std::string> branch_cases;
    std::vector<std::string> internal_cases;
    std::vector<std::string> subtree_cases;
    std::vector<std::string> tree_standard_cases;
    std::vector<std::string> tree_exotic_cases;
    std::vector<std::string> tree_must_fail_cases;
};

TEST_F(NewickParserTest, name_grammar)
{
    for (const auto &input : name_cases)
    {
        auto iter = input.begin();
        auto iter_end = input.end();
        bool r = x3::parse(iter, iter_end, quetzal::format::newick::parser::name);
        EXPECT_TRUE(r && iter == iter_end);
    }
}

TEST_F(NewickParserTest, length_grammar)
{
    for (const auto &input : length_cases)
    {
        auto iter = input.begin();
        auto iter_end = input.end();
        bool r = x3::parse(iter, iter_end, quetzal::format::newick::parser::length);
        EXPECT_TRUE(r && iter == iter_end);
    }
}

TEST_F(NewickParserTest, leaf_grammar)
{
    for (const auto &input : leaf_cases)
    {
        auto iter = input.begin();
        auto iter_end = input.end();
        bool r = x3::parse(iter, iter_end, quetzal::format::newick::parser::leaf);
        EXPECT_TRUE(r && iter == iter_end);
    }
}

TEST_F(NewickParserTest, tree_standard_grammar)
{
    for (const auto &input : tree_standard_cases)
    {
        auto iter = input.begin();
        auto iter_end = input.end();
        bool r = x3::parse(iter, iter_end, quetzal::format::newick::parser::tree);
        EXPECT_TRUE(r && iter == iter_end);
    }
}

// TEST_F(NewickParserTest, tree_exotic_grammar)
// {
//   for (const auto& input : tree_exotic_cases)
//   {
//     auto iter = input.begin();
//     auto iter_end = input.end();
//     bool r = x3::parse(iter, iter_end, quetzal::format::newick::parser::tree);
//     EXPECT_TRUE(r && iter == iter_end);
//   }
// }

