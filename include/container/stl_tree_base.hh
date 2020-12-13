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
#include "base/stl_tree_algo.hh"

#include <cstddef>
#include <limits>
#include <memory>

namespace ginshio {
namespace stl {
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
              decltype(_KOfVType()(_ValueType())), Key>::value>::type>
  _SizeType count(const Key& key) const {
    auto _pair = this->equal_range(key);
    return static_cast<_SizeType>(::std::distance(_pair.first, _pair.second));
  }
  template <typename Key,
            typename = typename ::std::enable_if<::std::is_constructible<
              decltype(_KOfVType()(_ValueType())), Key>::value>::type>
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
              decltype(_KOfVType()(_ValueType())), Key>::value>::type>
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
              decltype(_KOfVType()(_ValueType())), Key>::value>::type>
  ::std::pair<_Iterator, _Iterator> equal_range(const Key& key) {
    return {this->lower_bound(key), this->upper_bound(key)};
  }
  template <typename Key,
            typename = typename ::std::enable_if<::std::is_constructible<
              decltype(_KOfVType()(_ValueType())), Key>::value>::type>
  constexpr auto equal_range(const Key& key) const ->
      ::std::pair<_ConstIterator, _ConstIterator> {
    return {this->lower_bound(key), this->upper_bound(key)};
  }
  template <typename Key,
            typename = typename ::std::enable_if<::std::is_constructible<
              decltype(_KOfVType()(_ValueType())), Key>::value>::type>
  constexpr _Iterator lower_bound(const Key& key) {
    return _Iterator(
        ::ginshio::stl::tree::__lower_bound(
             static_cast<_NodeType*>(_impl._header._parent),
             static_cast<_NodeBase*>(&_impl._header), key,
             static_cast<_CompType>(_impl), static_cast<_KOfVType>(_impl)));
  }
  template <typename Key,
            typename = typename ::std::enable_if<::std::is_constructible<
              decltype(_KOfVType()(_ValueType())), Key>::value>::type>
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
              decltype(_KOfVType()(_ValueType())), Key>::value>::type>
  constexpr _Iterator upper_bound(const Key& key) {
    return _Iterator(
        ::ginshio::stl::tree::__upper_bound(
             static_cast<_NodeType*>(_impl._header._parent),
             static_cast<_NodeBase*>(&_impl._header), key,
             static_cast<_CompType>(_impl), static_cast<_KOfVType>(_impl)));
  }
  template <typename Key,
            typename = typename ::std::enable_if<::std::is_constructible<
              decltype(_KOfVType()(_ValueType())), Key>::value>::type>
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
