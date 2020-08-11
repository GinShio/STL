/*
 * include/base/stl_tree_algo.hh
 *
 * author:  GinShio <ginshio78@gmail.com>
 *
 * date:    2020-08-08 22:17:45
 *
 * details: tree base algorithms
 */

#ifndef STL_INCLUDE_BASE_STL_TREE_ALGO_HH_
#define STL_INCLUDE_BASE_STL_TREE_ALGO_HH_

namespace ginshio {
namespace stl {
namespace tree {
/////////////// tag ///////////////
enum class _NodeTag : char {HEADER = 0, NODE};
#define _RBTreeColor ginshio::stl::tree::_NodeTag
#define _RBTreeColor_RED ginshio::stl::tree::_NodeTag::HEADER
#define _RBTreeColor_BLACK ginshio::stl::tree::_NodeTag::NODE



/////////////// node ///////////////
template <typename TreeNode>
inline TreeNode* __get_grandparent(const TreeNode*& _node) {
  return _node->_parent ? _node->_parent->_parent : nullptr;
}
template <typename TreeNode>
inline TreeNode* __get_uncle(const TreeNode*& _node) {
  TreeNode* _gp = __get_grandparent(_node);
  if (!_gp) {
    return nullptr;
  }
  return _node->_parent == _gp->_left ? _gp->_right : _gp->_left;
}
template <typename TreeNode>
inline TreeNode* __get_sibling(const TreeNode*& _node) {
  if (!_node->_parent) {
    return nullptr;
  }
  return _node == _node->_parent->_left ?
      _node->_parent->_right : _node->_parent->_left;
}



/////////////// check ///////////////
template <typename TreeNode>
constexpr bool __is_left(TreeNode*& _node) {
  return _node == _node->_parent->_left;
}
template <typename TreeNode>
constexpr bool __is_right(TreeNode*& _node) {
  return _node == _node->_parent->_right;
}



/////////////// rotate ///////////////
/*
 * node p to rotate left
      R                 R
     / \               / \
    U   P             U   N
         \    ===>       / \
          N             P  SR
         / \             \
        SL  SR           SL
 */
template <typename TreeNode>
void __rotate_left(TreeNode* _p, TreeNode*& _root) {
  TreeNode* _n = _p->_right;
  _p->_right = _n->_left;
  if (_n->_left) {
    _n->_left->_parent = _p;
  }
  _n->_parent = _p->_parent;
  if (_p == _root) {
    _root = _n;
  } else if (_p == _p->_parent->_left) {
    _p->_parent->_left = _n;
  } else {
    _p->_parent->_right = _n;
  }
  _n->_left = _p;
  _p->_parent = _n;
}
/*
 * node p to rotate right
         R                  R
        / \                / \
       P   U              N   U
      /         ===>     / \
     N                  SL  P
    / \                    /
   SL  SR                 SR
*/
template <typename TreeNode>
void __rotate_right(TreeNode* _p, TreeNode*& _root) {
  TreeNode* _n = _p->_left;
  _p->_left = _n->_right;
  if (_n->_right) {
    _n->_right->_parent = _p;
  }
  _n->_parent = _p->_parent;
  if (_p == _root) {
    _root = _n;
  } else if (_p == _p->_parent->_right) {
    _p->_parent->_right = _n;
  } else {
    _p->_parent->_left = _n;
  }
  _n->_right = _p;
  _p->_parent = _n;
}



/////////////// maxi && mini ///////////////
template <typename TreeNode>
TreeNode* __node_maximum(const TreeNode* _root) {
  while (_root->_right) {
    _root = _root->_right;
  }
  return _root;
}
template <typename TreeNode>
TreeNode* __node_minimum(const TreeNode* _root) {
  while (_root->_left) {
    _root = _root->_left;
  }
  return _root;
}



/////////////// incr && decr ///////////////
template <typename TreeNode>
TreeNode* __node_increment(TreeNode* _node) {
  if (_node->_right) {
    _node = _node->_right;
    while (_node->_left) {
      _node = _node->_left;
    }
  } else {
    TreeNode* _tmp = _node->_parent;
    while (_tmp->_right == _node) {
      _node = _tmp;
      _tmp = _tmp->_right;
    }
    if (_node->_right != _tmp) {
      _node = _tmp;
    }
  }
  return _node;
}
template <typename TreeNode>
TreeNode* __node_decrement(TreeNode* _node) {
  if (_node->_tag == _NodeTag::HEADER && _node->_parent->_parent == _node) {
    return _node->_right;
  } else if (_node->left) {
    _node = _node->_left;
    while (_node->_right) {
      _node = _node->_right;
    }
    return _node;
  } else {
    TreeNode* _pa = _node->_parent;
    while (_node == _pa->_left) {
      _node = _pa;
      _pa = _pa->_parent;
    }
    return _pa;
  }
}

} // namespace tree
} // namespace stl
} // namespace ginshio

#endif // STL_INCLUDE_BASE_STL_TREE_ALGO_HH_
