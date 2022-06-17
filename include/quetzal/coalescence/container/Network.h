// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

///////////////////////////////////////////////////////////////////////////                                                                       *
/// This program is free software; you can redistribute it and/or modify ///
/// it under the terms of the GNU General Public License as published by ///
/// the Free Software Foundation; either version 2 of the License, or    ///
/// (at your option) any later version.                                  ///
///                                                                     ///
///////////////////////////////////////////////////////////////////////////

#ifndef NETWORK_H_INCLUDED
#define NETWORK_H_INCLUDED

#include <vector>

namespace quetzal
{
	namespace coalescence
	{
		namespace container
		{
			///
			/// @brief Data structure for representing phylogenetic network topologies where each node contains a data field.
			///
			/// @tparam CellT Cell type, ie the value of the node. The requirements that are imposed on CellT
			///               depend on the actual operations performed on Network<CellT>.
			///
			/// @remark The network is still oriented, with a sense of root and leaves.
			///
			/// @remark Extends the concept of Tree to the case of internal nodes having possibly several parents.
			///
			/// @see Standardization of the Phylogenetic Network concept: https://doi.org/10.1186/1471-2105-9-532
			///
			template<class CellT>
			class Network
			{
			private:
				CellT  m_cell;
				std::vector<Network<CellT>> m_parents;
				std::vector<Network<CellT>> m_children;

			public:
				///
				/// @brief Default constructor
				///
				/// @remark CellT must be default constructible
				///
				Network(): m_parents(), m_cell(), m_children()
				{
				}

				///
				/// @brief Constructor with data copy
				///
				/// @param cell the value of the network cell
				///
				/// @remark CellT must be copy constructible.
				///
				Network(const CellT& cell): m_parents(), m_cell(cell), m_children()
				{
				}

				///
				/// @brief Constructor with data move
				///
				/// @param cell the value of the network cell
				///
				/// @remark CellT must be move constructible.
				///
				Network(CellT&& cell) noexcept:
				m_parents(),
				m_cell(std::move(cell)),
				m_children()
				{
				}

				///
				/// @brief Returns the number of children
				///
				unsigned int nb_children() const
				{
					return m_children.size();
				}

				///
				/// @brief Returns the number of parents
				///
				unsigned int nb_parents() const
				{
					return m_parents.size();
				}

				///
				/// @brief Cell accessor
				///
				/// @return a const-reference to the network cell
				///
				/// @remark Read-only data access
				///
				const CellT& cell() const
				{
					return m_cell;
				}
				///
				/// @brief  Cell accessor
				///
				/// @return a reference to the network cell
				///
				/// @remark Read-and-write data access
				///
				CellT& cell()
				{
					return m_cell;
				}


				///
				/// @brief Add a subnetwork to the children list of the network, initialize the network
				///
				/// @param cell the cell value of the subnetwork to be added
				///
				/// @return a reference on the new subnetwork
				///
				Network<CellT>& add_child(CellT&& cell) noexcept
				{
					Network<CellT> subnetwork(std::move(cell));
					subnetwork.add_parent(*this);
					m_children.push_back(std::move(subnetwork));
					return m_children.back();
				}
				///
				/// @brief Checks whether the network has a parent
				///
				/// @return Returns true if the network has a parent, else returns false.
				///
				bool has_parent() const
				{
					return !m_parents.empty();
				}
				///
				/// @brief Checks whether the network has one or more child
				///
				/// @return Returns false if the network has no child, else returns true.
				///
				bool has_children() const
				{
					return !m_children.empty();
				}
				///
				/// @brief Applies a function object to each node encountered in a depth first search algorithm
				///
				/// @param op unary operation function object that will be applied to the data member///
				/// of the network. The unary operator takes the cell as argument. The signature
				/// of the function should be equivalent to the following:
				/// `void op(const CellT & cell);`.
				///
				template<typename UnaryOperation>
				void visit_cells_by_pre_order_DFS(UnaryOperation op) const
				{
					op(this->cell());
					for(auto const& child : this->m_children){
						child.visit_cells_by_pre_order_DFS(op);
					}
				}
				///
				/// @brief Applies a function object to each node encountered in a depth first search algorithm
				///
				/// @param op unary operation function object that will be applied to the data member///
				/// of the network. The unary operator takes the cell as argument. The signature
				/// of the function should be equivalent to the following:
				/// `void op(CellT & cell);`.
				///
				template<class UnaryOperation>
				void visit_subnetworks_by_pre_order_DFS(UnaryOperation op) {
					op(*this);
					for(auto & child : this->m_children){
						child.visit_subnetworks_by_pre_order_DFS(op);
					}
				}

				template<typename Op1, typename Op2, typename Op3>
				void visit_subnetworks_by_generic_DFS(Op1 preorder, Op2 inorder, Op3 postorder){
					preorder(*this);
					for(auto& child : this->m_children)
					{
						child.visit_subnetworks_by_generic_DFS(preorder, inorder, postorder);
						inorder(*this);
					}
					postorder(*this);
				}


				///
				/// @brief Applies a function object to each leave encountered in a depth first search algorithm
				/// @param op unary operation function object that will be applied to the data member
				/// of the network. The unary operator takes the cell as argument. The signature
				/// of the function should be equivalent to the following:
				/// `void op(const CellT & cell);`.
				///
				template<typename UnaryOperation>
				void visit_leaves_cells_by_DFS(UnaryOperation op) const
				{
					if(has_children()){
						for(auto const& child : this->m_children){
							child.visit_leaves_cells_by_DFS(op);
						}
					}else{
						op(this->cell());
					}
				}

			}; // end class Network
		} // end namespace container
	} // end namespace coalescence
} // end namespace quetzal

#endif
