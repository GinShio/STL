/*
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Hewlett-Packard Company makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 *
 * Copyright (c) 1996
 * Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this  software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 */

#ifndef GINSHIO_STL__CONTAINER_STL_TREE_BASE_HH_
#define GINSHIO_STL__CONTAINER_STL_TREE_BASE_HH_ 1

#include "base/stl_init.hh"

#include <cstddef>
#include <limits>
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



namespace __container_base {
///////////////////////// tree iterator /////////////////////////
template <typename _T, typename _Ptr, typename _Ref>
struct _TreeIterator {
  /////////////// traits type ///////////////
 public:
  using difference_type = ::std::ptrdiff_t;
  using value_type = _T;
  using pointer = _Ptr;
  using reference = _Ref;
  using iterator_category = ::std::bidirectional_iterator_tag;

  /////////////// iterator type ///////////////
 public:
  using _Iterator = _TreeIterator<_T, _T*, _T&>;
  using _ConstIterator = _TreeIterator<_T, const _T*, const _T&>;
  using _SelfIterator = _TreeIterator<_T, _Ptr, _Ref>;
  using _OtherIterator = typename
      ::std::conditional<::std::is_same<_SelfIterator, _Iterator>::value,
                          _ConstIterator, _Iterator>::type;

  /////////////// node type ///////////////
 public:
  using _NodeBase = ::ginshio::stl::tree::_TreeNodeBase;
  using _NodeType = ::ginshio::stl::tree::_TreeNode<_T>;

  /////////////// data member ///////////////
 public:
  _NodeBase* _node;

  /////////////// constructor ///////////////
 public:
  _TreeIterator() : _node(nullptr) {}
  explicit _TreeIterator(_NodeBase* _n) : _node(_n) {}
  _TreeIterator(const _SelfIterator&) = default;
  template <typename _Iter,
            typename = typename ::std::enable_if<
              ::std::is_same<_SelfIterator, _ConstIterator>::value &&
              ::std::is_same<_OtherIterator, _Iter>::value>::type>
  _TreeIterator(const _Iter& _it) : _node(_it._node) {}

  /////////////// assign ///////////////
 public:
  _SelfIterator& operator=(const _SelfIterator&) = default;
  template <typename _Iter,
            typename = typename ::std::enable_if<
              ::std::is_same<_SelfIterator, _ConstIterator>::value &&
              ::std::is_same<_OtherIterator, _Iter>::value>::type>
  _SelfIterator& operator=(const _Iter& _it) {
    _node = _it._node;
    return *this;
  }
  _SelfIterator& operator=(_NodeBase* _n) {
    _node = _n;
    return *this;
  }

  /////////////// member function ///////////////
 public:
  constexpr _SelfIterator& base() const {
    return const_cast<_SelfIterator&>(*this);
  }

  /////////////// member access operators ///////////////
 public:
  constexpr reference operator*() const noexcept {
    return static_cast<_NodeType*>(_node)->_data;
  }
  constexpr pointer operator->() const noexcept {
    return static_cast<_NodeType*>(_node)->__addr();
  }

  /////////////// arithmetic operators ///////////////
 public:
  _SelfIterator& operator++() {
    this->_node = __node_increment(this->_node);
    return *this;
  }
  _SelfIterator operator++(int) {
    _SelfIterator _tmp = *this;
    this->_node = __node_increment(this->_node);
    return _tmp;
  }
  _SelfIterator& operator--() {
    this->_node = __node_decrement(this->_node);
    return *this;
  }
  _SelfIterator& operator--(int) {
    _SelfIterator _tmp = *this;
    this->_node = __node_decrement(this->_node);
    return _tmp;
  }
};
///////////////////////// iterator comparison operator /////////////////////////
template <typename _T, typename _Ptr, typename _Ref>
constexpr bool operator==(const _TreeIterator<_T, _Ptr, _Ref>& _lit,
                          const _TreeIterator<_T, _Ptr, _Ref>& _rit) noexcept {
  return _lit._node == _rit._node;
}
template <typename _T, typename _PtrL, typename _RefL,
          typename _PtrR, typename _RefR>
constexpr bool operator==(
    const _TreeIterator<_T, _PtrL, _RefL>& _lit,
    const _TreeIterator<_T, _PtrR, _RefR>& _rit) noexcept {
  return _lit._node == _rit._node;
}
template <typename _T, typename _Ptr, typename _Ref>
constexpr bool operator!=(const _TreeIterator<_T, _Ptr, _Ref>& _lit,
                          const _TreeIterator<_T, _Ptr, _Ref>& _rit) noexcept {
  return _lit._node != _rit._node;
}
template <typename _T, typename _PtrL, typename _RefL,
          typename _PtrR, typename _RefR>
constexpr bool operator!=(
    const _TreeIterator<_T, _PtrL, _RefL>& _lit,
    const _TreeIterator<_T, _PtrR, _RefR>& _rit) noexcept {
  return _lit._node != _rit._node;
}



///////////////////////// tree base /////////////////////////
template <typename _T, typename _Compare, typename _KeyOfValue,
          typename _Allocator>
struct _TreeBase {
  /////////////// member type ///////////////
 protected:
  using _ValueType = _T;
  using _CompType = _Compare;
  using _KOfVType = _KeyOfValue;
  using _DataAllocType = _Allocator;
  using _DataAllocTraits = std::allocator_traits<_DataAllocType>;
  using _NodeBase = ::ginshio::stl::tree::_TreeNodeBase;
  using _NodeHeader = ::ginshio::stl::tree::_TreeNodeHeader;
  using _NodeType = ::ginshio::stl::tree::_TreeNode<_ValueType>;
  using _NodeAllocType =
      typename _DataAllocTraits::template rebind_alloc<_NodeType>;
  using _NodeAllocTraits =
      typename _DataAllocTraits::template rebind_traits<_NodeType>;
  using _SizeType = ::std::size_t;
  using _DifferenceType = ::std::ptrdiff_t;
  using _Reference = _ValueType&;
  using _ConstReference = const _ValueType&;
  using _Iterator = _TreeIterator<_ValueType, _ValueType*, _ValueType&>;
  using _ConstIterator =
      _TreeIterator<_ValueType, const _ValueType*, const _ValueType&>;
  using _ReverseIterator = ::std::reverse_iterator<_Iterator>;
  using _ConstReverseIterator = ::std::reverse_iterator<_ConstIterator>;

  /////////////// implement ///////////////
 protected:
  struct _TreeImpl : public _NodeAllocType, public _CompType, public _KOfVType {
    ::ginshio::stl::tree::_TreeNodeHeader _header;
    _TreeImpl() = default;
    _TreeImpl(const _NodeAllocType& _alloc)
        : _NodeAllocType(_alloc) {}
    void __swap(_TreeImpl& _impl) { ::std::swap(_impl._header, this->_header); }
  };

  /////////////// data member ///////////////
 protected:
  _TreeImpl _impl;

  /////////////// constructor ///////////////
 public:
  _TreeBase() = default;
  _TreeBase(const _NodeAllocType& _alloc) : _impl(_alloc) {}
  _TreeBase(_TreeBase&& _other) noexcept : _impl(::std::move(_other._impl)) {}
  ~_TreeBase() noexcept { this->clear(); }

  /////////////// utils function ///////////////
 protected:
  static void __clear_aux(_TreeImpl& _impl, _NodeBase* _node) {
    if (_node == nullptr) {
      return;
    }
    _NodeBase* _cur = _node,* _pre = nullptr;
    _NodeBase* _leftmost = _impl._header._left;
    while (_cur != nullptr) {
      _pre = _cur;
      if (_cur->_right != nullptr) {
        _leftmost->_left = _cur->_right;
        _leftmost = ::ginshio::stl::tree::__get_leftmost(_leftmost);
      }
      _cur = _cur->_left;
      _TreeBase::__put(_impl, static_cast<_NodeType*>(_pre));
    }
    _impl._header.__init_default();
  }
  static void __swap_allocator(_NodeAllocType& _a, _NodeAllocType& _b) {
    ::std::swap(_a, _b);
  }

  /////////////// get and put node ///////////////
 protected:
  template <typename... Args>
  static _NodeBase* __get(_TreeImpl& _impl, Args&&... args) {
    _NodeType* _node = _NodeAllocTraits::allocate(_impl, 1);
    try {
      ::ginshio::stl::construct(_node->__addr(), ::std::forward<Args>(args)...);
    } catch (...) {
      _NodeAllocTraits::deallocate(_impl, _node, 1);
      throw;
    }
    _node->_NodeBase::_parent =
        _node->_NodeBase::_left = _node->_NodeBase::_right = nullptr;
    return static_cast<_NodeBase*>(_node);
  }
  static void __put(_TreeImpl& _impl, _NodeType* _node) noexcept {
    ::ginshio::stl::destroy(_node->__addr());
    _NodeAllocTraits::deallocate(_impl, _node, 1);
  }

  /////////////// member function ///////////////
 public:
  constexpr _DataAllocType get_allocator() const {
    return _DataAllocType(static_cast<_NodeAllocType>(_impl));
  }

  /////////////// element access ///////////////
 public:
  _Reference front() { return *_Iterator(_impl._header._left); }
  constexpr _ConstReference front() const {
    return *_ConstIterator(_impl._header._left);
  }
  _Reference back() { return *_Iterator(_impl._header._right); }
  constexpr _ConstReference back() const {
    return *_ConstIterator(_impl._header._right);
  }

  /////////////// iterator ///////////////
 public:
  _Iterator begin() noexcept { return _Iterator(_impl._header._left); }
  constexpr _ConstIterator begin() const noexcept {
    return _ConstIterator(_impl._header._left);
  }
  constexpr _ConstIterator cbegin() const noexcept {
    return _ConstIterator(_impl._header._left);
  }
  _Iterator end() noexcept { return _Iterator(&_impl._header); }
  constexpr _ConstIterator end() const noexcept {
    return _ConstIterator(const_cast<_NodeHeader*>(&_impl._header));
  }
  constexpr _ConstIterator cend() const noexcept {
    return _ConstIterator(const_cast<_NodeHeader*>(&_impl._header));
  }
  _ReverseIterator rbegin() noexcept {
    return _ReverseIterator(_Iterator(static_cast<_NodeBase*>(&_impl._header)));
  }
  constexpr _ConstReverseIterator rbegin() const noexcept {
    return _ConstReverseIterator(
        _ConstIterator(const_cast<_NodeHeader*>(&_impl._header)));
  }
  constexpr _ConstReverseIterator crbegin() const noexcept {
    return _ConstReverseIterator(
        _ConstIterator(const_cast<_NodeHeader*>(&_impl._header)));
  }
  _ReverseIterator rend() noexcept {
    return _ReverseIterator(_Iterator(_impl._header._left));
  }
  constexpr _ConstReverseIterator rend() const noexcept {
    return _ConstReverseIterator(_ConstIterator(_impl._header._left));
  }
  constexpr _ConstReverseIterator crend() const noexcept {
    return _ConstReverseIterator(_ConstIterator(_impl._header._left));
  }

  /////////////// capacity ///////////////
 public:
  constexpr bool empty() const noexcept { return _impl._header._size == 0; }
  constexpr _SizeType size() const noexcept { return _impl._header._size; }
  constexpr _SizeType max_size() const noexcept {
    return ::std::numeric_limits<std::ptrdiff_t>::max() / sizeof(_NodeType);
  }
  void shrink_to_fit() {}
  constexpr _SizeType capacity() const noexcept { return _impl._header._size; }

  /////////////// modifier ///////////////
 public:
  void clear() noexcept {
    if (_impl._header._size == 0) {
      return;
    }
    _TreeBase::__clear_aux(_impl, _impl._header._parent);
  }

  /////////////// find ///////////////
 public:
  template <typename Key,
            typename = typename ::std::enable_if<::std::is_constructible<
              decltype(::std::declval<_KOfVType>()(::std::declval<_ValueType>())),
              Key>::value>::type>
  _SizeType count(const Key& key) const {
    auto _pair = this->equal_range(key);
    return static_cast<_SizeType>(::std::distance(_pair.first, _pair.second));
  }
  template <typename Key,
            typename = typename ::std::enable_if<::std::is_constructible<
              decltype(::std::declval<_KOfVType>()(::std::declval<_ValueType>())),
              Key>::value>::type>
  _Iterator find(const Key& key) {
    _NodeBase* _ret = static_cast<_NodeBase*>(&_impl._header);
    const _CompType& _comp = static_cast<_CompType>(_impl);
    const _KOfVType& _kofv = static_cast<_KOfVType>(_impl);
    for (_NodeBase* _cur = _ret->_parent; _cur != nullptr;) {
      if (_comp(_kofv(static_cast<_NodeType*>(_cur)->_data), key)) {
        _cur = static_cast<_NodeType*>(_cur->_right);
      } else if (_comp(key, _kofv(static_cast<_NodeType*>(_cur)->_data))) {
        _cur = static_cast<_NodeType*>(_cur->_left);
      } else {
        _ret = static_cast<_NodeBase*>(_cur);
        break;
      }
    }
    return _Iterator(_ret);
  }
  template <typename Key,
            typename = typename ::std::enable_if<::std::is_constructible<
              decltype(::std::declval<_KOfVType>()(::std::declval<_ValueType>())),
              Key>::value>::type>
  _ConstIterator find(const Key& key) const {
    const _NodeBase* _ret = static_cast<const _NodeBase*>(&_impl._header);
    const _CompType& _comp = static_cast<_CompType>(_impl);
    const _KOfVType& _kofv = static_cast<_KOfVType>(_impl);
    for (_NodeBase* _cur = _ret->_parent; _cur != nullptr;) {
      if (_comp(_kofv(static_cast<_NodeType*>(_cur)->_data), key)) {
        _cur = static_cast<_NodeType*>(_cur->_right);
      } else if (_comp(key, _kofv(static_cast<_NodeType*>(_cur)->_data))) {
        _cur = static_cast<_NodeType*>(_cur->_left);
      } else {
        _ret = static_cast<_NodeBase*>(_cur);
        break;
      }
    }
    return _ConstIterator(const_cast<_NodeBase*>(_ret));
  }
  template <typename Key,
            typename = typename ::std::enable_if<::std::is_constructible<
              decltype(::std::declval<_KOfVType>()(::std::declval<_ValueType>())),
              Key>::value>::type>
  ::std::pair<_Iterator, _Iterator> equal_range(const Key& key) {
    return {this->lower_bound(key), this->upper_bound(key)};
  }
  template <typename Key,
            typename = typename ::std::enable_if<::std::is_constructible<
              decltype(::std::declval<_KOfVType>()(::std::declval<_ValueType>())),
              Key>::value>::type>
  constexpr auto equal_range(const Key& key) const ->
      ::std::pair<_ConstIterator, _ConstIterator> {
    return {this->lower_bound(key), this->upper_bound(key)};
  }
  template <typename Key,
            typename = typename ::std::enable_if<::std::is_constructible<
              decltype(::std::declval<_KOfVType>()(::std::declval<_ValueType>())),
              Key>::value>::type>
  _Iterator lower_bound(const Key& key) {
    return _Iterator(
        ::ginshio::stl::tree::__lower_bound(
             static_cast<_NodeType*>(_impl._header._parent),
             static_cast<_NodeBase*>(&_impl._header), key,
             static_cast<_CompType>(_impl), static_cast<_KOfVType>(_impl)));
  }
  template <typename Key,
            typename = typename ::std::enable_if<::std::is_constructible<
              decltype(::std::declval<_KOfVType>()(::std::declval<_ValueType>())),
              Key>::value>::type>
  constexpr _ConstIterator lower_bound(const Key& key) const {
    return _ConstIterator(
        ::ginshio::stl::tree::__lower_bound(
             static_cast<_NodeType*>(_impl._header._parent),
             static_cast<_NodeBase*>(const_cast<_NodeHeader*>(&_impl._header)),
             key,
             static_cast<_CompType>(_impl), static_cast<_KOfVType>(_impl)));
  }
  template <typename Key,
            typename = typename ::std::enable_if<::std::is_constructible<
              decltype(::std::declval<_KOfVType>()(::std::declval<_ValueType>())),
              Key>::value>::type>
  _Iterator upper_bound(const Key& key) {
    return _Iterator(
        ::ginshio::stl::tree::__upper_bound(
             static_cast<_NodeType*>(_impl._header._parent),
             static_cast<_NodeBase*>(&_impl._header), key,
             static_cast<_CompType>(_impl), static_cast<_KOfVType>(_impl)));
  }
  template <typename Key,
            typename = typename ::std::enable_if<::std::is_constructible<
              decltype(::std::declval<_KOfVType>()(::std::declval<_ValueType>())),
              Key>::value>::type>
  constexpr _ConstIterator upper_bound(const Key& key) const {
    return _ConstIterator(static_cast<_NodeType*>(
        ::ginshio::stl::tree::__upper_bound(
             static_cast<_NodeType*>(_impl._header._parent),
             static_cast<_NodeBase*>(const_cast<_NodeHeader*>(&_impl._header)),
             key,
             static_cast<_CompType>(_impl), static_cast<_KOfVType>(_impl))));
  }
};

}  // namespace __container_base
}  // namespace stl
}  // namespace ginshio

#endif  // GINSHIO_STL__CONTAINER_STL_TREE_BASE_HH_
