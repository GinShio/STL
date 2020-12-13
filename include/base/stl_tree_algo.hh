#ifndef GINSHIO_STL__BASE_STL_TREE_ALGO_HH_
#define GINSHIO_STL__BASE_STL_TREE_ALGO_HH_ 1

#include <cstddef>
#include <memory>
#include <utility>

namespace ginshio {
namespace stl {
namespace tree {

///////////////////////// binary tree key_of_value /////////////////////////
template <typename T>
struct _KeyOfValue {
  constexpr T& operator()(T& _) { return _; }
  constexpr const T& operator()(const T& _) const { return _; }
};
template <typename K, typename V>
struct _KeyOfValue<::std::pair<const K, V>> {
  constexpr const K& operator()(::std::pair<const K, V>& _) { return _.first; }
  constexpr const K& operator()(const ::std::pair<const K, V>& _) const {
    return _.first;
  }
};



///////////////////////// tag /////////////////////////
enum class _NodeTag : char { HEADER = 0, NODE = 1 };
#define _RBTreeColor ::ginshio::stl::tree::_NodeTag
#define _RBTreeColor_RED ::ginshio::stl::tree::_NodeTag::HEADER
#define _RBTreeColor_BLACK ::ginshio::stl::tree::_NodeTag::NODE



///////////////////////// binary tree node /////////////////////////
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
  void __set_tag(const _NodeTag tag) { this->_tag = tag; }
};

_TreeNodeBase* __get_leftmost(const _TreeNodeBase*);
_TreeNodeBase* __get_rightmost(const _TreeNodeBase*);

struct _TreeNodeHeader : public _TreeNodeBase {
  ::std::size_t _size;
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
    this->_parent = _other._parent;
    this->_size = _other._size;
    this->_left = ::ginshio::stl::tree::__get_leftmost(_other._parent);
    this->_right = ::ginshio::stl::tree::__get_rightmost(_other._parent);
    this->_parent->_parent = this;
  }
};

template <typename _T>
struct _TreeNode : public _TreeNodeBase {
  _T _data;
  _T* __addr() noexcept { return ::std::addressof(this->_data); }
  constexpr const _T* __addr() const noexcept {
    return ::std::addressof(this->_data);
  }
};



///////////////////////// get node /////////////////////////
inline _TreeNodeBase* __get_grandparent(const _TreeNodeBase* _node) {
  return !_node ? nullptr : _node->_parent ? _node->_parent->_parent : nullptr;
}
inline _TreeNodeBase* __get_uncle(const _TreeNodeBase* _node) {
  _TreeNodeBase* _gp = __get_grandparent(_node);
  if (!_gp) {
    return nullptr;
  }
  return _node->_parent == _gp->_left ? _gp->_right : _gp->_left;
}
inline _TreeNodeBase* __get_sibling(const _TreeNodeBase* _node) {
  if (!_node || !_node->_parent) {
    return nullptr;
  }
  return _node == _node->_parent->_left ? _node->_parent->_right
                                        : _node->_parent->_left;
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
void __set_leftmost(_TreeNodeHeader*& _header, _TreeNodeBase* _node) {
  _header->_left->_left = _node;
  _node->_parent = _header->_left;
  _header->_left = _node;
}
void __set_rightmost(_TreeNodeHeader*& _header, _TreeNodeBase* _node) {
  _header->_right->_right = _node;
  _node->_parent = _header->_right;
  _header->_right = _node;
}



///////////////////////// rotate /////////////////////////
/*
 * node p to rotate left
      R                 R
     / \               / \
    S   N             S   C
         \    ===>       / \
          C             N   Y
         / \             \
        X   Y             X
 */
void __rotate_left(_TreeNodeBase* _node, _TreeNodeHeader*& _header) {
  _TreeNodeBase* _child = _node->_right;
  _node->_right = _child->_left;
  if (_child->_left != nullptr) {
    _child->_left->_parent = _node;
  }
  _child->_parent = _node->_parent;
  if (_node == _header->_parent) {
    _header->_parent = _child;
  } else if (_node == _node->_parent->_left) {
    _node->_parent->_left = _child;
  } else {
    _node->_parent->_right = _child;
  }
  _child->_left = _node;
  _node->_parent = _child;
}
/*
 * node p to rotate right
         R                  R
        / \                / \
       N   S              C   S
      /         ===>     / \
     C                  X   N
    / \                      \
   X   Y                      Y
*/
void __rotate_right(_TreeNodeBase* _node, _TreeNodeHeader*& _header) {
  _TreeNodeBase* _child = _node->_left;
  _node->_left = _child->_right;
  if (_child->_right != nullptr) {
    _child->_right->_parent = _node;
  }
  _child->_parent = _node->_parent;
  if (_node == _header->_parent) {
    _header->_parent = _child;
  } else if (_node == _node->_parent->_left) {
    _node->_parent->_left = _child;
  } else {
    _node->_parent->_right = _child;
  }
  _child->_right = _node;
  _node->_parent = _child;
}



///////////////////////// incr && decr /////////////////////////
_TreeNodeBase* __node_increment(_TreeNodeBase* _node) {
  if (_node->_right != nullptr) {
    _node = _node->_right;
    while (_node->_left != nullptr) {
      _node = _node->_left;
    }
  } else {
    _TreeNodeBase* _pa = _node->_parent;
    while (_pa->_right == _node) {
      _node = _pa;
      _pa = _pa->_parent;
    }
    if (_node->_right != _pa) {
      _node = _pa;
    }
  }
  return _node;
}
_TreeNodeBase* __node_decrement(_TreeNodeBase* _node) {
  if (_node->_tag == _NodeTag::HEADER && _node->_parent->_parent == _node) {
    return _node->_right;
  }
  if (_node->_left != nullptr) {
    _node = _node->_left;
    while (_node->_right != nullptr) {
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
template <typename _T, typename _Key, typename _Comp, typename _KOfV>
_TreeNodeBase* __lower_bound(const _TreeNode<_T>* _cur,
                             const _TreeNodeBase* _parent, const _Key& _key,
                             const _Comp& _comp, const _KOfV& _kofv) {
  while (_cur != nullptr) {
    if (!_comp(_kofv(_cur->_data), _key)) {
      _parent = static_cast<const _TreeNodeBase*>(_cur);
      _cur = static_cast<_TreeNode<_T>*>(_cur->_left);
    } else {
      _cur = static_cast<_TreeNode<_T>*>(_cur->_right);
    }
  }
  return const_cast<_TreeNodeBase*>(_parent);
}
template <typename _T, typename _Key, typename _Comp, typename _KOfV>
_TreeNodeBase* __upper_bound(const _TreeNode<_T>* _cur,
                             const _TreeNodeBase* _parent, const _Key& _key,
                             const _Comp& _comp, const _KOfV& _kofv) {
  while (_cur != nullptr) {
    if (_comp(_key, _kofv(_cur->_data))) {
      _parent = static_cast<const _TreeNodeBase*>(_cur);
      _cur = static_cast<_TreeNode<_T>*>(_cur->_left);
    } else {
      _cur = static_cast<_TreeNode<_T>*>(_cur->_right);
    }
  }
  return const_cast<_TreeNodeBase*>(_parent);
}



///////////////////////// insert node to tree /////////////////////////
void __insert_node(_TreeNodeBase* _node, _TreeNodeBase* _parent,
                   _TreeNodeBase** _child, _TreeNodeHeader* _header) {
  _node->_parent = _parent;
  *_child = _node;
  if ((_parent == _header->_left && _parent->_left == _node) ||
      _parent == static_cast<_TreeNodeBase*>(_header)) {
    _header->_left = _node;
  }
  if ((_parent == _header->_right && _parent->_right == _node) ||
      _parent == static_cast<_TreeNodeBase*>(_header)) {
    _header->_right = _node;
  }
}
template <typename _T, typename _Comp, typename _KOfV>
void __insert_equal(_TreeNode<_T>*& _node, _TreeNodeHeader*& _header,
                    const _Comp& _comp, const _KOfV& _kofv) {
  _TreeNode<_T>* _parent = static_cast<_TreeNode<_T>*>(
      static_cast<_TreeNodeBase*>(_header));
  _TreeNodeBase** _tmp = &_parent->_parent;
  auto& _val = _kofv(_node->_data);
  while (*_tmp) {
    _parent = static_cast<_TreeNode<_T>*>(*_tmp);
    _tmp = _comp(_val, _kofv(_parent->_data)) ?
        &_parent->_left : &_parent->_right;
  }
  __insert_node(_node, _parent, _tmp, _header);
}
template <typename _T, typename _Comp, typename _KOfV>
void __insert_hint_equal(_TreeNode<_T>* _pos, _TreeNode<_T>*& _node,
                         _TreeNodeHeader* _header,
                         const _Comp& _comp, const _KOfV& _kofv) {
  using _NodeBase = _TreeNodeBase;
  using _NodeType = _TreeNode<_T>;
  auto& _val = _kofv(_node->_data);
  if (static_cast<_NodeBase*>(_pos) == static_cast<_NodeBase*>(_header)) {
    if (_header->_parent != nullptr &&
        !_comp(_val, _kofv(static_cast<_NodeType*>(_header->_right)->_data))) {
      return __set_leftmost(_header, _node);
    } else {
      return __insert_equal(_node, _header, _comp, _kofv);
    }
  } else if (!_comp(_kofv(_pos->_data), _val)) {
    if (_pos == _header->_left) {
      return __set_leftmost(_header, _node);
    }
    _NodeType* _prev = static_cast<_NodeType*>(__node_decrement(_pos));
    return !_comp(_val, _kofv(_prev->_data)) ?
        (_prev->_right == nullptr ?
         _prev->__set_right(_node) : _pos->__set_left(_node)) :
        __insert_equal(_node, _header, _comp, _kofv);
  } else {
    if (_pos == _header->_right) {
      return __set_rightmost(_header, _node);
    }
    _NodeType* _next = static_cast<_NodeType*>(__node_increment(_pos));
    return !_comp(_kofv(_next->_data), _val) ?
        (_next->_left == nullptr ?
         _next->__set_left(_node) : _pos->__set_right(_node)) :
        __insert_equal(_node, _header, _comp, _kofv);
  }
}
template <typename _T, typename _Comp, typename _KOfV>
bool __insert_unique(_TreeNode<_T>*& _node, _TreeNodeHeader*& _header,
                     const _Comp& _comp, const _KOfV& _kofv) {
  using _NodeType = _TreeNode<_T>;
  using _NodeBase = _TreeNodeBase;
  _NodeType* _parent =
      static_cast<_NodeType*>(static_cast<_NodeBase*>(_header));
  _NodeBase** _tmp = &_parent->_parent;
  auto& _val = _kofv(_node->_data);
  while (*_tmp) {
    _parent = static_cast<_NodeType*>(*_tmp);
    _tmp = _comp(_val, _kofv(_parent->_data)) ?
        &_parent->_left : &_parent->_right;
  }
  if (static_cast<_NodeBase*>(_parent) == static_cast<_NodeBase*>(_header) ||
      (static_cast<_NodeBase*>(_parent) == _header->_left &&
       _tmp == &_parent->_left)) {
    __insert_node(_node, _parent, _tmp, _header);
    return true;
  }
  _NodeType* _prev = _tmp == &_parent->_left ?
      static_cast<_NodeType*>(__node_decrement(_parent)) : _parent;
  if (_comp(_kofv(_prev->_data), _val)) {
    __insert_node(_node, _parent, _tmp, _header);
    return true;
  }
  _node = _prev;
  return false;
  // if (_tmp == &_parent->_right &&
  //     _comp(_kofv(static_cast<_NodeType*>(__node_decrement(_parent))->_data), _val)) {
  //   _NodeType* _prev = static_cast<_NodeType*>(__node_decrement(_parent));
  //   if (!_comp(_kofv(_prev->_data), _val)) {
  //     _node = _prev;
  //     return false;
  //   }
  // }
  // _node->_parent = _parent;
  // *_tmp = _node;
  // if (static_cast<_NodeBase*>(_parent) == static_cast<_NodeBase*>(_header)) {
  //   _header->_left = _header->_right = _node;
  //   return true;
  // }
  // if (_parent == _header->_left && _parent->_left == _node) {
  //   _header->_left = _node;
  // } else if (_parent == _header->_right && _parent->_right == _node) {
  //   _header->_right = _node;
  // }
  // return true;
}
template <typename _T, typename _Comp, typename _KOfV>
bool __insert_hint_unique(_TreeNode<_T>* _pos, _TreeNode<_T>*& _node,
                          _TreeNodeHeader* _header,
                          const _Comp& _comp, const _KOfV& _kofv) {
  using _NodeP = _TreeNode<_T>*;
  auto& _val = _kofv(_node->_data);
  if (static_cast<_TreeNodeBase*>(_pos) ==
      static_cast<_TreeNodeBase*>(_header)) {
    if (_header->_parent != nullptr &&
        _comp(_kofv(static_cast<_NodeP>(_header->_right)->_data), _val)) {
      __set_rightmost(_header, _node);
      return true;
    } else {
      return __insert_unique(_node, _header, _comp, _kofv);
    }
  } else if (_comp(_val, _kofv(_pos->_data))) {
    if (_pos == _header->_left) {
      __set_leftmost(_header, _node);
      return true;
    }
    _NodeP _prev = static_cast<_NodeP>(__node_decrement(_pos));
    return _comp(_kofv(_prev->_data), _val) ?
        (_prev->_right == nullptr ?
         _prev->__set_right(_node) : _pos->__set_left(_node), true) :
        __insert_unique(_node, _header, _comp, _kofv);
  } else if (_pos->_data < _val) {
    if (_pos == _header->_right) {
      __set_rightmost(_header, _node);
      return true;
    }
    _NodeP _next = static_cast<_NodeP>(__node_increment(_pos));
    return _comp(_val, _kofv(_next->_data)) ?
        (_next->_left == nullptr ?
         _next->__set_left(_node) : _pos->__set_right(_node), true) :
        __insert_unique(_node, _header, _comp, _kofv);
  } else {
    _node = _pos;
    return false;
  }
}

}  // namespace tree
}  // namespace stl
}  // namespace ginshio

#endif  // GINSHIO_STL__BASE_STL_TREE_ALGO_HH_
