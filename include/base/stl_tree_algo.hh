#ifndef GINSHIO_STL__STL_TREE_ALGO_HH_
#define GINSHIO_STL__STL_TREE_ALGO_HH_ 1

#include <memory>

#include <cstddef>

namespace ginshio {
namespace stl {
namespace tree {
///////////////////////// data /////////////////////////
template <typename T, typename U>
struct associative_pair {
  T first;
  U second;
  explicit operator T() const { return first; }
};
template <typename T, typename U>
bool operator<(const associative_pair<T, U>& l,
               const associative_pair<T, U>& r) {
  return l.first < r.first;
}
template <typename T, typename U>
bool operator<(const associative_pair<T, U>& l, const T& r) {
  return l.first < r;
}
template <typename T, typename U>
bool operator<(const T& l, const associative_pair<T, U>& r) {
  return l < r.first;
}


///////////////////////// tag /////////////////////////
enum class _NodeTag : char {HEADER = 0, NODE = 1};
#define _RBTreeColor ginshio::stl::tree::_NodeTag
#define _RBTreeColor_RED ginshio::stl::tree::_NodeTag::HEADER
#define _RBTreeColor_BLACK ginshio::stl::tree::_NodeTag::NODE



///////////////////////// node && iterator /////////////////////////
struct _TreeNodeBase {
  _NodeTag _tag;
  _TreeNodeBase* _left;
  _TreeNodeBase* _right;
  _TreeNodeBase* _parent;
  _TreeNodeBase() : _tag(_NodeTag::HEADER),
                    _left(nullptr), _right(nullptr), _parent(nullptr) {}
  /////////////// insert ///////////////
  void __set_left(_TreeNodeBase* _node) {
    _node->_parent = this;
    this->_left = _node;
  }
  void __set_right(_TreeNodeBase* _node) {
    _node->_parent = this;
    this->_left = _node;
  }
  /////////////// tag ///////////////
  constexpr const bool __is_header() const {
    return this->_tag == _NodeTag::HEADER;
  }
  void __set_tag(const _NodeTag tag) {
    this->_tag = tag;
  }
};

struct _TreeNodeHeader : public _TreeNodeBase {
  std::size_t _size;
  _TreeNodeHeader() { this->__init_default(); }
  _TreeNodeHeader(_TreeNodeHeader&& _header) noexcept {
    if (_header._size == 0) {
      this->__init_default();
    } else {
      this->__init_other(_header);
      _header.__init_default();
    }
  }
  _TreeNodeHeader& operator=(_TreeNodeHeader&& _header) {
    if (_header._size == 0) {
      this->__init_default();
    } else {
      this->__init_other(_header);
      _header.__init_default();
    }
    return *this;
  }
  void __init_default() {
    this->_parent = nullptr;
    this->_left = this->_right = this;
    this->_size = 0;
  }
  void __init_other(_TreeNodeHeader& _other) {
    this->_left = _other._left;
    this->_right = _other._right;
    this->_parent = _other._parent;
    this->_size = _other._size;
  }
};

template <typename _T>
struct _TreeNode : public _TreeNodeBase {
  _T _data;
  _T* __addr() noexcept { return std::addressof(this->_data); }
  constexpr const _T* __addr() const noexcept {
    return std::addressof(this->_data);
  }
};



///////////////////////// get node /////////////////////////
inline _TreeNodeBase* __get_grandparent(const _TreeNodeBase*& _node) {
  return _node ? nullptr : _node->_parent ? _node->_parent->_parent : nullptr;
}
inline _TreeNodeBase* __get_uncle(const _TreeNodeBase*& _node) {
  _TreeNodeBase* _gp = __get_grandparent(_node);
  if (!_gp) {
    return nullptr;
  }
  return _node->_parent == _gp->_left ? _gp->_right : _gp->_left;
}
inline _TreeNodeBase* __get_sibling(const _TreeNodeBase*& _node) {
  if (!_node || !_node->_parent) {
    return nullptr;
  }
  return _node == _node->_parent->_left ?
      _node->_parent->_right : _node->_parent->_left;
}
_TreeNodeBase* __get_rightmost(const _TreeNodeBase* _root) {
  if (_root == nullptr) {
    return nullptr;
  }
  while (_root->_right) {
    _root = _root->_right;
  }
  return const_cast<_TreeNodeBase*>(_root);
}
_TreeNodeBase* __get_leftmost(const _TreeNodeBase* _root) {
  if (_root == nullptr) {
    return nullptr;
  }
  while (_root->_left) {
    _root = _root->_left;
  }
  return const_cast<_TreeNodeBase*>(_root);
}



///////////////////////// size /////////////////////////
std::size_t __get_size(const _TreeNodeBase* _node) {
  if (_node == nullptr) {
    return 0;
  }
  return __get_size(_node->_left) + __get_size(_node->_right) + 1;
}



///////////////////////// set leftmost && rightmost /////////////////////////
void __set_leftmost(_TreeNodeHeader*& _header, _TreeNodeBase*& _node) {
  _header->_left->_left = _node;
  _node->_parent = _header->_left;
  _header->_left = _node;
}
void __set_rightmost(_TreeNodeHeader*& _header, _TreeNodeBase*& _node) {
  _header->_right->_right = _node;
  _node->_parent = _header->_right;
  _header->_right = _node;
}



///////////////////////// rotate /////////////////////////
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
void __rotate_left(_TreeNodeBase* _p, _TreeNodeBase*& _root) {
  _TreeNodeBase* _n = _p->_right;
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
void __rotate_right(_TreeNodeBase* _p, _TreeNodeBase*& _root) {
  _TreeNodeBase* _n = _p->_left;
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



///////////////////////// incr && decr /////////////////////////
_TreeNodeBase* __node_increment(_TreeNodeBase* _node) {
  if (_node->_right) {
    _node = _node->_right;
    while (_node->_left) {
      _node = _node->_left;
    }
  } else {
    _TreeNodeBase* _tmp = _node->_parent;
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
_TreeNodeBase* __node_decrement(_TreeNodeBase* _node) {
  if (_node->_tag == _NodeTag::HEADER && _node->_parent->_parent == _node) {
    return _node->_right;
  }
  if (_node->_left) {
    _node = _node->_left;
    while (_node->_right) {
      _node = _node->_right;
    }
  } else {
    _TreeNodeBase* _pa = _node->_parent;
    while (_node == _pa->_left) {
      _node = _pa;
      _pa = _pa->_parent;
    }
    _node = _pa;
  }
  return _node;
}



///////////////////////// get bound /////////////////////////
template <typename _T, typename _Key>
_TreeNodeBase* __lower_bound(const _TreeNode<_T>* _cur,
                             const _TreeNodeBase* _parent, const _Key& _key) {
  while (_cur != nullptr) {
    if (!(_cur->_data < _key)) {
      _parent = _cur;
      _cur = _cur->_left;
    } else {
      _cur = _cur->_right;
    }
  }
  return const_cast<_TreeNodeBase*>(_parent);
}
template <typename _T, typename _Key>
_TreeNodeBase* __upper_bound(const _TreeNode<_T>* _cur,
                             const _TreeNodeBase* _parent, const _Key& _key) {
  while (_cur != nullptr) {
    if (_key < _cur->_data) {
      _parent = _cur;
      _cur = _cur->_left;
    } else {
      _cur = _cur->_right;
    }
  }
  return const_cast<_TreeNodeBase*>(_parent);
}



///////////////////////// insert node to tree /////////////////////////
template <typename _T>
void __insert_equal(_TreeNode<_T>*& _node, _TreeNodeHeader*& _header) {
  _TreeNode<_T>* _parent = _header;
  _TreeNodeBase** _tmp = &_parent->_parent;
  _T& _val = _node->_data;
  while (*_tmp && (_parent = *_tmp)) {
    _tmp = _val < _parent->_data ? &_parent->_left : &_parent->_right;
  }
  _node->_parent = _parent;
  *_tmp = _node;
  if (_parent == _header) {
    _header->_left = _header->_right = _node;
    return;
  }
  if (_parent == _header->_left && _parent->_left == _node) {
    _header->_left = _node;
  } else if (_parent == _header->_right && _parent->_right == _node) {
    _header->_right = _node;
  }
}
template <typename _T>
void __insert_hint_equal(_TreeNode<_T>*& _pos,
                         _TreeNode<_T>*& _node, _TreeNodeHeader*& _header) {
  _T& _val = _node->_data;
  if (_pos == _header) {
    if (_header->_parent &&
        !(_val < static_cast<_TreeNode<_T>*>(_header->_right)->_data)) {
      return __set_leftmost(_header, _node);
    } else {
      return __insert_equal(_node, _header);
    }
  } else if (!(_pos->_data < _val)) {
    if (_pos == _header->_left) {
      return __set_leftmost(_header, _node);
    }
    _TreeNode<_T>* _prev = static_cast<_TreeNode<_T>*>(__node_decrement(_pos));
    if (!(_val < _prev->_data)) {
      return _prev->_right == nullptr ?
          _prev->__set_right(_node) : _pos->__set_left(_node);
    } else {
      return __insert_equal(_node, _header);
    }
  } else {
    if (_pos == _header->_right) {
      return __set_rightmost(_header, _node);
    }
    _TreeNode<_T>* _next = static_cast<_TreeNode<_T>*>(__node_increment(_pos));
    if (!(_next->_data < _val)) {
      return _next->_left == nullptr ?
          _next->__set_left(_node) : _pos->__set_right(_node);
    } else {
      return __insert_equal(_node, _header);
    }
  }
}
template <typename _T>
bool __insert_unique(_TreeNode<_T>*& _node, _TreeNodeHeader*& _header) {
  _TreeNode<_T>* _parent = _header;
  _TreeNodeBase** _tmp = &_parent->_parent;
  _T& _val = _node->_data;
  while (*_tmp && (_parent = *_tmp)) {
    _tmp = _val < _parent->_data ? &_parent->_left : &_parent->_right;
  }
  if (_tmp == &_parent->_left && _parent != _header->_left) {
    _TreeNode<_T>* _prev = static_cast<_TreeNode<_T>*>(__node_decrement(_parent));
    if (!(_prev->_data < _val)) {
      _node = _prev;
      return false;
    }
  }
  _node->_parent = _parent;
  *_tmp = _node;
  if (_parent == _header) {
    _header->_left = _header->_right = _node;
    return true;
  }
  if (_parent == _header->_left && _parent->_left == _node) {
    _header->_left = _node;
  } else if (_parent == _header->_left && _parent->_right == _node) {
    _header->_right = _node;
  }
  return true;
}
template <typename _T>
bool __insert_hint_unique(_TreeNode<_T>*& _pos,
                          _TreeNode<_T>*& _node, _TreeNodeHeader*& _header) {
  _T& _val = _node->_data;
  if (_pos == _header) {
    if (_header->_parent &&
        static_cast<_TreeNode<_T>*>(_header->_right)->_data < _val) {
      __set_rightmost(_header, _node);
      return true;
    } else {
      return __insert_unique(_node, _header);
    }
  } else if (_val < _pos->_data) {
    if (_pos == _header->_left) {
      __set_leftmost(_header, _node);
      return true;
    }
    _TreeNode<_T>* _prev = static_cast<_TreeNode<_T>*>(__node_decrement(_pos));
    if (_prev->_data < _val) {
      _prev->_right == nullptr ?
          _prev->__set_right(_node) : _pos->__set_left(_node);
      return true;
    } else {
      return __insert_unique(_node, _header);
    }
  } else if (_pos->_data < _val) {
    if (_pos == _header->_right) {
      __set_rightmost(_header, _node);
      return true;
    }
    _TreeNode<_T>* _next = static_cast<_TreeNode<_T>*>(__node_increment(_pos));
    if (_val < _next->_data) {
      _next->_left == nullptr ?
          _next->__set_left(_node) : _pos->__set_right(_node);
      return true;
    } else {
      return __insert_unique(_node, _header);
    }
  } else {
    _node = _pos;
    return false;
  }
}

} // namespace tree
} // namespace stl
} // namespace ginshio

#endif // GINSHIO_STL__STL_TREE_ALGO_HH_
