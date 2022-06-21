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
    bool check_if_balanced(const std::string & input, const char & open='(', const char & close=')')
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
      static std::string edit(std::string&& s)
      {
        return std::move(s);
      }
    };
    ///
    /// @brief Class template
    ///
    template<class tag>
    struct is_balanced
    {};
    ///
    /// @brief Specialization for parenthesis
    ///
    template<> struct is_balanced<parenthesis>
    {
      static bool check(const std::string& s)
      {
        return check_if_balanced(s, '(', ')' );
      }
    };
    ///
    /// @brief Specialization for square bracket
    ///
    template<> struct is_balanced<square_bracket>
    {
      static bool check(const std::string& s)
      {
        return check_if_balanced(s, '[', ']');
      }
    };

    namespace newick
    {
      ///
      /// @brief Node names can be any character except blanks, colons, semicolons, parentheses, and square brackets.
      ///
      // static inline constexpr std::vector<std::string> forbidden_labels = {" ",",",";","(",")","()",")(","[","]","[]","]["};
      ///
      /// @brief Underscore characters in unquoted labels are converted to blanks.
      ///
      /// @detail Because you may want to include a blank in a name, it is assumed
      ///         that an underscore character ("_") stands for a blank; any of
      ///         these in a name will be converted to a blank when it is read in.
      ///
      static inline constexpr char blank = '_';
      ///
      /// @brief Template class.
      ///
      template<unsigned int N>
      struct remove_comments_of_depth
      {};
      ///
      /// @brief Do not remove anything
      ///
      template<> struct remove_comments_of_depth<0> : identity
      {};
      ///
      /// @brief Remove all comments substrings contained between square brackets
      ///
      /// @note Assumes that text is well formatted so there are no such things like [[]] or unclosed bracket
      ///
      template<> struct remove_comments_of_depth<1>
      {
        static std::string edit(const std::string& s)
        {
          return std::regex_replace(s, std::regex(R"(\[[^()]*\])"), "");
        }
      };
      ///
      /// @brief Remove all comments substrings contained between square brackets of depth 2
      ///
      /// @note Assumes that text is well formatted so there are no such things like [[]] or unclosed bracket
      ///
      template<> struct remove_comments_of_depth<2>
      {
        static std::string edit(const std::string& s)
        {
          std::string buffer;
          int counter = 0;
          for (const auto & ch : s)
          {
            if (ch == '[' ) counter++;
            if (ch == ']' ) counter--;
            if (!(ch == '[' || ch == ']') && counter == 1) buffer.append(std::string(1,ch));
          }
          return buffer;
        }
      };
      ///
      /// @brief Allow nested comments.
      ///
      struct PAUP
      {
        // return empty string
        static inline std::string root_branch_length() { return "";}
        // do nothing
        static inline std::string treat_comments(std::string & s)
        {

        }
      };
      ///
      /// @brief Set a root node branch length to zero. Remove all nested comments.
      ///
      struct TreeAlign
      {
        // Set explicit null branch length for root node
        static inline std::string root_branch_length() { return "0.0";}
        // Remove comments that are nested, keep comments of depth 1
        static inline std::string treat_comments(const std::string &s)
        {
          return remove_comments_of_depth<2>::edit(s);
        }
      };
      ///
      /// @brief Requires that an unrooted tree begin with a trifurcation; it will not "uproot" a rooted tree.
      ///
      struct PHYLIP
      {
        // Branch length for root node is not explicit.
        static inline std::string root_branch_length(){return "";}
        // Remove comments that are nested, keep comments of depth 1
        static inline std::string treat_comments(std::string & s){}
      };
      ///
      /// @brief Concept for label name
      ///
      template<class F, class... Args>
      concept Formattable = std::invocable<F, Args...> &&
      std::convertible_to<std::invoke_result_t<F, Args...>, std::string>;

      ///
      /// @brief Generic algorithm to generate the Newick formula of a tree.
      ///
      template<class T, std::predicate<T> P1 , std::predicate<T> P2, Formattable<T> F1, Formattable<T> F2 >
      class Formatter
      {
      public:
        ///
        /// @brief Type of node being formatted
        ///
        using node_type = T;
        ///
        /// @brief Type of formula being generated
        ///
        using formula_type = std::string;
      private:
        ///
        /// @brief End character.
        ///
        static inline constexpr char _end = ';';
        ///
        /// @brief The string formula to be updated.
        ///
        mutable formula_type _formula;
        ///
        /// @brief Functor inspecting if the node being visited has children.
        ///
        P1 _has_children;
        ///
        /// @brief Functor inspecting if the node being visited has a parent.
        ///
        P2 _has_parent;
        ///
        /// @brief Retrieve the name of the node being visited.
        ///
        /// @detail A name can be any string of printable characters except blanks,
        ///         colons, semicolons, parentheses, and square brackets.
        ///
        /// @remark Return type must be convertible to std::string
        ///
        F1 _label;
        ///
        /// @brief Retrieve the branch length immediately above the node being visited.
        ///
        /// @details Branch lengths can be incorporated into a tree by putting a
        ///          real number, with or without decimal point, after a node and
        ///          preceded by a colon. This represents the length of the branch
        ///          immediately above that node (that is, distance to parent node)
        /// @remark Return type must be convertible to std::string
        ///
        F2 _branch_length;
        ///
        /// @brief Operation called in the general DFS algorithm to open a parenthesis if node has children to be visited.
        ///
        /// @param node the node currently visited
        ///
        void preorder(const node_type & node) const
        {
          if(std::invoke(_has_children, node))
          {
            _formula += "(";
          }
        }
        ///
        /// @brief Operation called in the general DFS algorithm to add a comma between visited nodes.
        ///
        void inorder() const
        {
          _formula += ",";
        }
        ///
        /// @brief Operation called in the general DFS algorithm to open a parenthesis if node has children to be visited.
        ///
        /// @param node the node currently visited
        ///
        void postorder(const node_type & node) const
        {
          if(std::invoke(_has_children, node))
          {
            // Remove comma
            _formula.pop_back();
            _formula += ")";
          }

          if(has_parent(node))
          {
            _formula += std::to_string(std::invoke(_label, node));
            _formula += ":";
            _formula += std::to_string(std::invoke(_branch_length, node));
          }
        }
      public:
        ///
        /// @brief Constructor
        ///
        Formatter(P1 has_parent, P2 has_children, F1 label, F2 branch_length):
        _has_parent(has_parent),
        _has_children(has_children),
        _label(label),
        _branch_length(branch_length)
        {
        }
        ///
        /// @brief Clear the formula buffer.
        ///
        void clear()
        {
          _formula.clear();
        }
        ///
        /// @brief Format the given node in the specified format
        ///
        template<typename Policy>
        formula_type format(node_type & root) const
        {
          // that or expose orders
          root.visit_by_generic_DFS(preorder, inorder, postorder);

          _formula.push_back(this->_end);

          if(is_balanced<parenthesis>::check(_formula) == false)
          {
            throw std::runtime_error(std::string("Failed: formula parenthesis are not balanced:") + _formula);
          }

          if(is_balanced<square_bracket>::check(_formula) == false)
          {
            throw std::runtime_error(std::string("Failed: formula square brackets are not balanced:") + _formula);
          }

          return _formula;
        }
      }; // end structrure Newick

      template<class T, std::predicate<T> P1 , std::predicate<T> P2, Formattable<T> F1, Formattable<T> F2 >
      auto make_formatter(P1 has_parent, P2 has_children, F1 label, F2 branch_length)
      {
        return Formatter<T, P1, P2, F1, F2>(has_parent, has_children, label, branch_length);
      }
    } // end namespace newick
  } // end namespace format
} // end namespace quetzal

#endif
