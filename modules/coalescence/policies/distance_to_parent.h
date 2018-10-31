// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __DISTANCE_TO_PARENT_H_INCLUDED__
#define __DISTANCE_TO_PARENT_H_INCLUDED__

#include "../containers/Tree.h"
#include "../containers/Forest.h"
#include "../../simulator/simulators.h"

namespace quetzal {

namespace coalescence {

namespace policies {

template<typename Space, typename Time>
struct distance_to_parent {

  using coord_type = Space;
  using time_type = Time;

  //! Defines what data type the tree should store
  class cell_type {
    private:
    	time_type m_t;
      unsigned int m_distance_to_parent = 0;
    public:
    	cell_type(time_type t): m_t(t) {}
    	cell_type() = default;

      time_type time() const {return m_t;}
      void time(time_type const& t) { m_t = t;}

      unsigned int distance_to_parent() const {return m_distance_to_parent;}
      void distance_to_parent(unsigned int l) {m_distance_to_parent = l;}
  };

  using tree_type = quetzal::coalescence::Tree<cell_type>;
  using forest_type = quetzal::coalescence::Forest<coord_type, tree_type>;

  //! Treatment to operate on a DFS on the tree to compute branches length
  struct treatment {
    auto operator()(tree_type & node) const {
      if(node.has_parent()){
        int d = node.cell().time() - node.parent().cell().time();
        int l = std::abs(d);
        node.cell().distance_to_parent(l);
      }else{
        node.cell().distance_to_parent(0);
      }
    }
  };

  static auto make_forest(std::map<coord_type, unsigned int> const& sample_counts, time_type const& sampling_time){
    forest_type forest;
    for(auto const& it: sample_counts){
      forest.insert(it.first, std::vector<tree_type>(it.second, tree_type(sampling_time)));
    }
    return forest;
  }

  static auto branch(){
    return []( auto& parent , auto const& child ){ return parent.add_child(child);};
  }

  static auto init(){
    return [](coord_type, time_type const& t){return tree_type(t);};
  }

  template<typename Generator>
  static tree_type end_in_wright_fisher(forest_type const& forest, time_type const& first_time, unsigned int N, Generator& gen){
    time_type t_curr = first_time;
    auto WF_init = [&t_curr](time_type const& t){
      t_curr -= t;
      return tree_type(t_curr);
    };
    using WF_model = quetzal::simulators::DiscreteTimeWrightFisher;
    auto tree = WF_model::coalesce(forest, N, gen, branch(), WF_init);
    treatment computer;
    tree.visit_subtrees_by_pre_order_DFS(computer);
    return tree;
  }

  static std::string make_newick(tree_type & tree){
    std::string newick;

  	auto preorder = [&newick](auto const& node){
  			if(node.has_children()){
  				newick += "(";
  			}
  	};

  	auto inorder = [&newick](auto const&){
  			newick += ",";
  	};

  	auto postorder = [&newick](auto const& node){
  		if(node.has_children()){
  			newick.pop_back();
  			newick += ")";
  		}
  		if(node.has_parent()){
  			newick += ":";
  			newick += std::to_string(node.cell().distance_to_parent());
  		}
  	};
  	tree.visit_subtrees_by_generic_DFS(preorder, inorder, postorder);
  	newick.push_back(';');
    return newick;
  }

};

template<typename Space, typename Time>
struct distance_to_parent_leaf_name {

  using coord_type = Space;
  using time_type = Time;

  //! Defines what data type the tree should store
  class cell_type {
    private:
      std::string m_name;
    	time_type m_t;
      unsigned int m_distance_to_parent = 0;

    public:
      cell_type(std::string const& name, time_type const& t): m_name(name), m_t(t) {}
      cell_type(time_type const& t): m_name(""), m_t(t) {}
    	cell_type() = default;

      time_type time() const {return m_t;}
      void time(time_type const& t) { m_t = t;}

      std::string name() const {return m_name;}
      void name(std::string const& name) { m_name = name;}

      unsigned int distance_to_parent() const {return m_distance_to_parent;}
      void distance_to_parent(unsigned int l) {m_distance_to_parent = l;}
  };

  using tree_type = quetzal::coalescence::Tree<cell_type>;
  using forest_type = quetzal::coalescence::Forest<coord_type, tree_type>;

  //! Treatment to operate on a DFS on the tree to compute branches length
  struct treatment {
    auto operator()(tree_type & node) const {
      if(node.has_parent()){
        int d = node.cell().time() - node.parent().cell().time();
        int l = std::abs(d);
        node.cell().distance_to_parent(l);
      }else{
        node.cell().distance_to_parent(0);
      }
    }
  };

  template<typename F>
  static auto make_forest(std::map<coord_type, unsigned int> const& sample_counts, time_type const& sampling_time, F get_name){
    forest_type forest;
    for(auto const& it: sample_counts){
      cell_type c(get_name(it.first, sampling_time), sampling_time);
      forest.insert(it.first, std::vector<tree_type>(it.second, tree_type(c)));
    }
    return forest;
  }

  static auto branch(){
    return []( auto& parent , auto const& child ){ return parent.add_child(child);};
  }

  static auto init(){
    return [](coord_type, time_type const& t){return tree_type(t);};
  }

  template<typename Generator>
  static tree_type end_in_wright_fisher(forest_type const& forest, time_type const& first_time, unsigned int N, Generator& gen){
    time_type t_curr = first_time;
    auto WF_init = [&t_curr](time_type const& t){
      t_curr -= t;
      return tree_type(t_curr);
    };
    using WF_model = quetzal::simulators::DiscreteTimeWrightFisher;
    auto tree = WF_model::coalesce(forest, N, gen, branch(), WF_init);
    treatment computer;
    tree.visit_subtrees_by_pre_order_DFS(computer);
    return tree;
  }

  static std::string make_newick(tree_type & tree){
    std::string newick;

  	auto preorder = [&newick](auto const& node){
  			if(node.has_children()){
  				newick += "(";
  			}
  	};

  	auto inorder = [&newick](auto const&){
  			newick += ",";
  	};

  	auto postorder = [&newick](auto const& node){
  		if(node.has_children()){
  			newick.pop_back();
  			newick += ")";
  		}
  		if(node.has_parent()){
        newick += node.cell().name();
  			newick += ":";
  			newick += std::to_string(node.cell().distance_to_parent());
  		}
  	};
  	tree.visit_subtrees_by_generic_DFS(preorder, inorder, postorder);
  	newick.push_back(';');
    return newick;
  }

};

} // end namespace policies

} // end namespace coalescence
} // end namespace quetzal

#endif
