// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/
#include "../../expressive.h"

#include <vector>
#include <algorithm>

//! \remark Represents a set of couples {param, summary statistics}
template<template <class,class> class Cont, class ParamType, class DataType>
class ReferenceTable{

private:
  std::vector<Cont<ParamType,DataType>> m_table;
public:

  auto begin() {return m_table.begin(); }

  auto end() {return m_table.end(); }

  auto cbegin() const {return m_table.cbegin(); }

  auto cend() const {return m_table.cend(); }

  auto size() const {return m_table.size(); }

  void reserve(size_t n){ m_table.reserve(n); }

  void push_back(Cont<ParamType,DataType> const& sample ){
    m_table.push_back(sample);
  }

  void push_back( Cont<ParamType,DataType>&& sample ){
    m_table.push_back(std::move(sample));
  }

  template<class... Args>
  void emplace_back(Args&&... args) {
    m_table.emplace_back(std::forward<Args>(args)...);
  }

  template<typename UnaryOperation>
  auto compute_summary_statistics(UnaryOperation const& eta) const {
    using Ret = typename quetzal::expressive::return_type<UnaryOperation>;
    ReferenceTable<Cont, ParamType, Ret> s;
    for(auto const& it : m_table){
      s.emplace_back(it.param(), eta(it.data()));
    }
    return s;
  }

  template<typename BinaryOperation>
  auto compute_distance_to(DataType const& obs, BinaryOperation const& rho) const {
    using Ret = typename quetzal::expressive::return_type<BinaryOperation>;
    ReferenceTable<Cont, ParamType, Ret> s;
    for(auto const& it : m_table){
      s.emplace_back(it.param(), rho(obs, it.data()));
    }
    return s;
  }

}; // end class ReferenceTable
