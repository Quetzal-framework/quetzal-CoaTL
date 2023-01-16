// Copyright 2020 Arnaud Becheler    <abechele@umich.edu>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __NEWICK_GENERATOR_H_INCLUDED__
#define __NEWICK_GENERATOR_H_INCLUDED__

#include<concepts>
#include<regex>
#include<string>
#include<stdexcept>
#include<algorithm>
#include<string_view>

#include <boost/graph/depth_first_search.hpp>
#include "quetzal/coalescence/graph/k_ary_tree.hpp"

namespace quetzal::format::newick
{

  namespace detail
  {
    // Replacement for `std::function<T(U)>::argument_type`
    template<typename T>
    struct single_function_argument;

    template<typename Ret, typename Arg>
    struct single_function_argument<std::function<Ret(Arg)>> { using type = Arg; };

    template<typename P1>
    struct single_function_argument_impl{
      using type = typename single_function_argument<decltype(std::function{ std::declval<P1>() }) >::type;
    };

    template<typename P1>
    using single_function_argument_t = typename single_function_argument_impl<P1>::type;

    /// @brief Tag
    struct parenthesis {};

    /// @brief Tag
    struct square_bracket {};

    ///
    /// @brief Check if the string is balanced for open/close symbols (parenthesis,brackets)
    ///
    ///
    /// @note Since parenthesis checking is a context-free grammar, it requires a stack.
    ///       Regex can not accomplish that since they do not have memory.
    ///
    bool check_if_balanced(std::string_view input, const char & open='(', const char & close=')')
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
    /// @brief Default comment removal policy: do not change anything
    ///
    struct identity
    {
      static std::string edit(const std::string  s)
      {
        return s;
      }
    };

    ///
    /// @brief Class template, base for further specialization
    ///
    template<class tag>
    struct is_balanced
    {};

    ///
    /// @brief Specialization for parenthesis
    ///
    template<> struct is_balanced<detail::parenthesis>
    {
      static bool check(std::string_view s)
      {
        return check_if_balanced(s, '(', ')' );
      }
    };

    ///
    /// @brief Specialization for square bracket
    ///
    template<> struct is_balanced<detail::square_bracket>
    {
      static bool check(std::string_view s)
      {
        return check_if_balanced(s, '[', ']');
      }
    };
  }

  using namespace std::string_literals;

  ///
  /// @brief Node names can be any character except blanks, colons, semicolons, parentheses, and square brackets.
  ///
  static inline std::vector<std::string> forbidden_labels = {" "s, ","s, ";"s, "("s, ")"s, "["s, "]"s};

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
  template<> struct remove_comments_of_depth<0> : detail::identity
  {};

  ///
  /// @brief Remove all comments substrings contained between square brackets
  ///
  /// @note Assumes that text is well formatted so there are no such things like [[]] or unclosed bracket
  ///
  template<> struct remove_comments_of_depth<1>
  {
    static std::string edit(const std::string  s)
    {
      if (s.empty())
      return s;
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
    static std::string edit(const std::string  s)
    {
      std::string buffer;
      int counter = 0;
      for (const auto & ch : s)
      {
        if (ch == '[' ) counter++;
        if (ch == ']' ) counter--;
        if ( ch == '[' && counter == 2) continue;  // do nothing, that was the opening
        if ( ch == ']' && counter == 1) continue; // do nothing, that was the closing
        if ( !( counter >=2 || (counter == 1 && ch == ']') ) ) buffer.append(std::string(1, ch));
      }
      return buffer;
    }
  };

  ///
  /// @brief Policy allowing to keep nested comments.
  ///
  /// @note Use this as a template parameter to specialize a Newick generator policy
  ///
  struct PAUP
  {
    // return empty string
    static inline std::string root_branch_length() { return "";}
    // do nothing
    static inline std::string treat_comments(std::string & s) { return s; }
  };

  ///
  /// @brief Set a root node branch length to zero, allow comments of depth 1, but will remove nested comments.
  ///
  /// @note Use this as a template parameter to specialize a Newick generator policy
  ///
  struct TreeAlign
  {
    // Set explicit null branch length for root node
    static inline std::string root_branch_length() { return ":0.0";}
    // Remove comments that are nested, keep comments of depth 1
    static inline std::string treat_comments(const std::string  s)
    {
      return remove_comments_of_depth<2>::edit(s);
    }
  };

  ///
  /// @brief Requires that an unrooted tree begin with a trifurcation; it will not "uproot" a rooted tree.
  ///        Allow comments of depth 1, but does not allow nested comments.
  /// @note Use this as a template parameter to specialize a Newick generator policy
  ///
  struct PHYLIP
  {
    // Branch length for root node is not explicit.
    static inline std::string root_branch_length(){return "";}
    // Remove comments that are nested, keep comments of depth 1
    static inline std::string treat_comments(std::string & s)
    {
      // Allow comments of depth 1, but does not allow nested comments.
      return remove_comments_of_depth<2>::edit(s);
    }
  };

  ///
  /// @brief Concept for label name: invocable and the return type is convertible to a string.
  ///
  template<class F, class... Args>
  concept Formattable = std::invocable<F, Args...> &&
  std::convertible_to<std::invoke_result_t<F, Args...>, std::string>;

  ///
  /// @brief Generate the Newick formula from an external (custom) tree class.
  ///
  /// @remark This is a non-intrusive interface implementation so users can reuse Newick formatting
  ///         logic and expose the formatting internals to their own Tree class's DFS.
  template<class T, std::predicate<T> P1 , std::predicate<T> P2, Formattable<T> F1, Formattable<T> F2, class Policy=PAUP>
  class generator : public Policy
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
    ///
    /// @brief Type of formula being generated
    ///
    using policy_type = Policy;

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
    /// @brief The depth at which the generator is
    ///
    mutable int _depth=0;

    ///
    /// @brief Functor inspecting if the node being visited has a parent.
    ///
    P1 _has_parent;

    ///
    /// @brief Functor inspecting if the node being visited has children.
    ///
    P2 _has_children;

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

    void _pre_order(const node_type & node) const
    {
      if(std::invoke(_has_children, node))
      {
        _formula += "(";
        _depth += 1;
      }
    }

    void _in_order() const
    {
      _formula += ",";
    }

    void _post_order(node_type const& node)
    {
      // Not leaf
      if(std::invoke(_has_children, node)) _formula += ")";

      // Not root
      if ( std::invoke(_has_parent, node) )
      { 
        auto const label = std::invoke(_label, node);
        _formula += label;
        std::string const branch( std::invoke(_branch_length, node) );
        if ( !branch.empty() ) _formula.append(":").append(branch);
      } else {
        // Is root
        _formula += std::invoke(_label, node);
        _formula += policy_type::root_branch_length();
      }

      _depth -= 1; 
    }

  public:
    ///
    /// @brief Constructor
    ///
    generator(P1 has_parent, P2 has_children, F1 label, F2 branch_length, Policy pol = {})
        : policy_type(std::move(pol))
        , _has_parent(std::move(has_parent))
        , _has_children(std::move(has_children))
        , _label(std::move(label))
        , _branch_length(std::move(branch_length)) {}

    ///
    /// @brief Operation called in the general DFS algorithm to open a parenthesis if node has children to be visited.
    ///
    /// @param node the node currently visited
    ///
    auto pre_order()
    {
      return [this](const node_type & node){this->_pre_order(node);};
    }

    ///
    /// @brief Operation called in the general DFS algorithm to add a comma between visited nodes.
    ///
    auto in_order()
    {
      return [this](){this->_in_order();};
    }

    ///
    /// @brief Operation to be passed to a generic DFS algorithm to open a parenthesis if node has children to be visited.
    ///
    /// @param node the node currently visited
    ///
    auto post_order()
    {
      return [this](const node_type & node){this->_post_order(node);};
    }

    ///
    /// @brief Check if a string contains characters forbidden by the standard
    ///
    bool has_forbidden_characters(const std::string & s) const
    {
      if (s.empty()) return false;
      std::string forbidden = " ,;()[\\]";
      bool is_forbidden = std::regex_search(s, std::regex("[" + forbidden + "]"));
      return is_forbidden;
    }

    ///
    /// @brief Clear the formula buffer to refresh the generator.
    ///
    void clear()
    {
      _formula.clear();
    }

    ///
    /// @brief Retrieve the formatted string of the given node in the specified format
    ///
    std::string&& take_result() const
    {
      _formula.push_back(this->_end);

      _formula = policy_type::treat_comments(_formula);

      if(detail::is_balanced<detail::parenthesis>::check(_formula) == false)
      {
        throw std::runtime_error(std::string("Failed: formula parenthesis are not balanced:") + _formula);
      }

      if(detail::is_balanced<detail::square_bracket>::check(_formula) == false)
      {
        throw std::runtime_error(std::string("Failed: formula square brackets are not balanced:") + _formula);
      }

      return std::move(_formula);
    }
  }; // end structure generator

  ///
  /// @brief User-defined deduction guide where the node/graph type T is deduced from P1
  /// @remark Template deduction guides are patterns associated with a template
  ///         class that tell the compiler how to translate a set of constructor
  ///          arguments (and their types) into template parameters for the class.
  template <class P1, class P2, class F1, class F2, class Policy = PAUP>
  generator(P1, P2, F1, F2, Policy pol = {})
      -> generator<detail::single_function_argument_t<P1>, P1, P2, F1, F2, Policy>;

} // end namespace quetzal::format::newick

#endif
