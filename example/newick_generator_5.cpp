#include "quetzal/quetzal.hpp"
#include <random>
#include <cassert>

// Let's say you have this legacy class ...
struct Node
{
  Node *parent = nullptr;
  Node *left = nullptr;
  Node *right = nullptr;

  // You will need a DFS method that takes arbitrary (templated) pre/in/post order operations
  template<class Op1, class Op2, class Op3>
  void depth_first_search(Op1 pre_order, Op2 in_order, Op3 post_order)
  {
    pre_order(*this);
    if(this->left != nullptr && this->right != nullptr)
    {
      this->left->depth_first_search(pre_order, in_order, post_order);
      in_order();
      this->right->depth_first_search(pre_order, in_order, post_order);
    }
    post_order(*this);
  }
};

int main()
{
  /* Let's assume you built this topology using your legacy Node class :
  *             a
  *           /   \
  *          /     c
  *         /     / \
  *        b     d   e
  */

  Node a, b, c, d, e;
  a.left = &b ; 
  b.parent = &a;
  a.right = &c; 
  c.parent = &a;
  c.left = &d ; 
  d.parent = &c;
  c.right = &e; 
  e.parent = &c;

  // Now you want to generate its Newick formula ...
  namespace newick = quetzal::format::newick;

  // You need to define 4 lambdas to interface the Quetzal generator with a non-quetzal tree type:
  std::predicate<Node> auto has_parent = [](const Node& v){return v.parent != nullptr;};
  std::predicate<Node> auto has_children = [](const Node& v){return v.left != nullptr && v.right != nullptr;};

  // If you are not interested into formatting label or branch_length information, just return an empty string:
  newick::Formattable<Node> auto no_label = [](const Node& v ){return "";};
  newick::Formattable<Node> auto no_branch_length = [](const Node& v){return "";};

  // We declare a quetzal newick generator and we pass to it the 4 lambdas:
  auto generator = newick::generator(has_parent, has_children, no_label, no_branch_length);

  // We connect the generator's interface to the legacy class DFS
  // the generator will handle the formatting events during the traversal
  a.depth_first_search(generator.pre_order(), generator.in_order(), generator.post_order());

  // We retrieve the formatted string
  std::cout << generator.take_result() << std::endl;
  return 0;
}
