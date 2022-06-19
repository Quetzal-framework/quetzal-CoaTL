// Copyright 2020 Arnaud Becheler    <abechele@umich.edu>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __NEWICK_FORMATTER_H_INCLUDED__
#define __NEWICK_FORMATTER_H_INCLUDED__

#include<concepts>
#include<regex>

namespace quetzal
{
  namespace format
  {
    ///
    /// @brief Tag
    ///
    struct parenthesis {};
    ///
    /// @brief Tag
    ///
    struct square_bracket {};
    ///
    /// @brief Check if the string is balanced for open/close symbols (parenthesis,brackets)
    ///
    ///
    /// @note Since parenthesis checking is a context-free grammar, it requires a stack.
    ///       Regex can not accomplish that since they do not have memory.
    ///
    bool is_balanced(string input, const std::string & open="(", const std::string & close=")")
    {
      int count = 0;
      for(const auto & ch : input)
      {
          if (ch == open ) count++;
          if (ch == close ) count--;
          // if a parenthesis is closed without being opened return false
          if(count < 0)
              return false;
      }
      // in the end the test is passed only if count is zero
      return count == 0;
    }
    ///
    /// @brief Do not change anything
    ///
    struct identity
    {
      static formula_type edit(formula_type&& s)
      {
        return std::move(s);
      }
    };
    ///
    /// @brief Class template
    ///
    template<class tag>
    struct check_if_balanced
    {};
    ///
    /// @brief Specialization for parenthesis
    ///
    template<> struct check_if_balanced<parenthesis>
    {
      static bool check(const std::string& s)
      {
        return is_balanced(s, open="(", close=")" );
      }
    };
    ///
    /// @brief Specialization for square bracket
    ///
    template<> struct check_if_balanced<square_bracket>
    {
      static bool check(const std::string& s)
      {
        return is_balanced(s, open="[", close="]");
      }
    };

    namespace newick
    {
      ///
      /// @brief Template class.
      ///
      template<unsigned int N>
      struct remove_comments_depth
      {};
      ///
      /// @brief Do not remove anything
      ///
      template<> struct remove_comments_depth<0>
      {
        static formula_type edit(std::string&& s)
        {
          return identity::edit(s);
        }
      };
      ///
      /// @brief Remove all comments substrings contained between square brackets
      ///
      /// @note Assumes that text is well formatted so there are no such things like [[]] or unclosed bracket
      ///
      template<> struct remove_comments__of_depth<1>
      {
        static formula_type edit(const std::string& s)
        {
          return std::regex_replace(s, std::regex(R"(\[[^()]*\])"), "");
        }
      };
      ///
      /// @brief Remove all comments substrings contained between square brackets of depth 2
      ///
      /// @note Assumes that text is well formatted so there are no such things like [[]] or unclosed bracket
      ///
      struct remove_comments_of_depth<2>
      {
        static formula_type edit(const std::string& s)
        {
          std::string buffer;
          int counter = 0;
          for (const auto & ch : s)
          {
            if (ch == '[' ) counter++;
            if (ch == ']' ) counter--;
            if (!(ch == '[' || ch == ']') && counter == 0) buffer.append(ch);
          }
          return buffer;
        }
      };
      ///
      /// @brief Allow nested comments.
      ///
      struct PAUP
      {
        static constexpr inline formula_type root_branch_length(){ return ""; }
      };
      ///
      /// @brief Set a root node branch length to zero. Remove all nested comments.
      ///
      struct TreeAlign
      {
        static constexpr inline formula_type root_branch_length(){ return "0.0"; }
      };
      ///
      /// @brief equires that an unrooted tree begin with a trifurcation; it will not "uproot" a rooted tree.
      ///
      struct PHYLIP
      {
        static constexpr inline formula_type root_branch_length(){ return ""; }
      };
      ///
      /// @brief Concept for label name
      ///
      template<class F, class... Args>
      concept Formattable = std::invocable<F, Args...> &&
                            std::convertible_to<std::invoke_result_t<F, Args...>, std::string>;
    }

    ///
    /// @brief Generic algorithm to generate the Newick formula of a tree.
    ///
    /// @see https://evolution.genetics.washington.edu/phylip/newicktree.html
    /// @note Unquoted labels may not contain blanks, parentheses, square brackets, single_quotes, colons, semicolons, or commas.
    /// @note Underscore characters in unquoted labels are converted to blanks.
    /// @note Single quote characters in a quoted label are represented by two single quotes.
    /// @note Blanks or tabs may appear anywhere except within unquoted labels or branch_lengths.
    /// @note Newlines may appear anywhere except within labels or branch_lengths.
    /// @note Comments are enclosed in square brackets and may appear anywhere newlines are permitted.
    /// @note PAUP (David Swofford) allows nesting of comments.
    /// @note TreeAlign (Jotun Hein) writes a root node branch length (with a value of 0.0).
    /// @note PHYLIP (Joseph Felsenstein) requires that an unrooted tree begin with a trifurcation; it will not "uproot" a rooted tree.
    class NewickFormatter
    {
      ///
      /// @brief Type of node being formatted
      ///
      using node_type = T;
      ///
      /// @brief Type of formula being generated
      ///
      using formula_type = std::string;
      ///
      /// @brief Clear the formula buffer.
      ///
      constexpr void clear() noexcept :
      {
        formula.clear();
      }
      ///
      /// @brief Node names can be any character except blanks, colons, semicolons, parentheses, and square brackets.
      ///
      static inline constexpr std::vector<std::string> forbidden_labels = {" ",",",";","(",")","()",")(","[","]","[]","]["};
      ///
      /// @brief Underscore characters in unquoted labels are converted to blanks.
      ///
      /// @detail Because you may want to include a blank in a name, it is assumed
      ///         that an underscore character ("_") stands for a blank; any of
      ///         these in a name will be converted to a blank when it is read in.
      ///
      static inline constexpr std::string blank = "_";
      ///
      /// @brief End character.
      ///
      static inline constexpr std::string end = ";";
      /// @brief Functor inspecting if the node bein visited has children.
      ///
      /// @note if T is quetzal::Tree or quetzal::Network, then F1 is
      ///        bool has_children(node_type node){ return node.has_children() }
      ///
      template<class T, class P1 , class P2, class F1, class F2>
        requires std::predicate<P1> && std::predicate<P2> && Formattable<F1> && Formattable<F2>
      constexpr formula_type format(node_type & root) const
      {
        tree.visit_by_generic_DFS(preorder, inorder, postorder);
        formula.push_back(this->end);
        return formula;
      }

    private:
      ///
      /// @brief The string formula to be updated.
      ///
      mutable formula_type formula;

      P1 has_children;

      ///
      /// @brief Functor inspecting if the node being visited has a parent.
      ///
      P2 has_parent;

      ///
      /// @brief Retrieve the name of the node being visited.
      ///
      /// @detail A name can be any string of printable characters except blanks,
      ///         colons, semicolons, parentheses, and square brackets.
      ///
      /// @remark Return type must be convertible to std::string
      ///
      F3 name;

      ///
      /// @brief Retrieve the branch length immediately above the node being visited.
      ///
      /// @details Branch lengths can be incorporated into a tree by putting a
      ///          real number, with or without decimal point, after a node and
      ///          preceded by a colon. This represents the length of the branch
      ///          immediately above that node (that is, distance to parent node)
      /// @remark Return type must be convertible to std::string
      ///
      F4 branch_length;

      ///
      /// @brief Operation called in the general DFS algorithm to open a parenthesis if node has children to be visited.
      ///
      /// @param node the node currently visited
      ///
      void preorder(const node_type & node) const
      {
        if(std::invoke(has_children, node))
        {
          formula += "(";
        }
      }
      ///
      /// @brief Operation called in the general DFS algorithm to add a comma between visited nodes.
      ///
      void inorder() const
      {
        formula += ",";
      }
      ///
      /// @brief Operation called in the general DFS algorithm to open a parenthesis if node has children to be visited.
      ///
      /// @param node the node currently visited
      ///
      void postorder(const node_type & node) const
      {
        if(std::invoke(has_children, node))
        {
          // Remove comma
          formula.pop_back();
          newick += ")";
        }

        if(this->has_parent(node))
        {
          // TODO: How to require that return type is convertible to string?
          formula += std::to_string(std::invoke(label, node));
          formula += ":";
          formula += std::to_string(std::invoke(branch_length, node));
        }
      }
    }; // end stucture Newick
  } // end namespace coalescence
} // end namespace quetzal

#endif
