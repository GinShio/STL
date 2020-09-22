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

#ifndef GINSHIO_STL__CONTAINER_STL_RB_TREE_HH_
#define GINSHIO_STL__CONTAINER_STL_RB_TREE_HH_ 1

#include "base/stl_init.hh"
#include "base/stl_tree_algo.hh"
#include "stl_tree_base.hh"

#include <cstddef>
#include <cstdlib>
#include <initializer_list>
#include <limits>
#include <memory>
#include <utility>

namespace ginshio {
namespace stl {

///////////////////////// red black tree /////////////////////////
template <typename T, typename Allocator = std::allocator<T>>
class rb_tree : protected __container_base::_TreeBase<T, Allocator> {
  /////////////// private type ///////////////
 private:
  using _Base = __container_base::_TreeBase<T, Allocator>;
  using _BaseImpl = typename _Base::_RBTreeImpl;
  using _DataAllocTraits = typename _Base::_DataAllocTraits;
  using _NodeBase = typename _Base::_NodeBase;
  using _NodeHeader = typename _Base::_NodeHeader;
  using _NodeType = typename _Base::_NodeType;
  using _NodeAllocType = typename _Base::_NodeAllocType;
  using _NodeAllocTraits = typename _Base::_NodeAllocTraits;
  /////////////// member type ///////////////
 public:
  using value_type = typename _Base::_ValueType;
  using allocator_type = typename _DataAllocTraits::allocator_type;
  using size_type = typename _Base::_SizeType;
  using difference_type = typename _Base::_DifferenceType;
  using reference = typename _Base::_Reference;
  using const_reference = typename _Base::_ConstReference;
  using pointer = typename _DataAllocTraits::pointer;
  using const_pointer = typename _DataAllocTraits::const_pointer;
  using iterator = typename _Base::_Iterator;
  using const_iterator = typename _Base::_ConstIterator;
  using reverse_iterator = typename _Base::_ReverseIterator;
  using const_reverse_iterator = typename _Base::_ConstReverseIterator;
  using node_type = typename _Base::_NodeType*;

  /////////////// data member ///////////////
 private:
  using _Base::_impl;

  /////////////// constructor ///////////////
 public:
  rb_tree() = default;
  explicit rb_tree(const allocator_type& alloc)
      : _Base(_NodeAllocType(alloc)) {}
  template <typename InputIt,
            typename = typename std::enable_if<
                std::is_base_of<std::input_iterator_tag,
                                typename std::iterator_traits<
                                    InputIt>::iterator_category>::value>::type*>
  rb_tree(InputIt first, InputIt last,
          const allocator_type& alloc = allocator_type())
      : _Base(_NodeAllocType(alloc)) {
    using _Category = typename std::iterator_traits<InputIt>::iterator_category;
    rb_tree::__copy_equal(_impl, this->begin(), first, last, _Category());
  }
  rb_tree(const rb_tree& other)
      : _Base(_NodeAllocType(
            _DataAllocTraits::select_on_container_copy_construction(
                other.get_allocator()))) {
    if (other._impl._header._size == 0) {
      return;
    }
    rb_tree::__copy_equal(_impl, other.begin(), other.end(),
                          std::bidirectional_iterator_tag());
  }
  rb_tree(const rb_tree& other, const allocator_type& alloc)
      : _Base(_NodeAllocType(alloc)) {
    if (other._impl._header._size == 0) {
      return;
    }
    rb_tree::__copy_equal(_impl, other.begin(), other.end(),
                          std::bidirectional_iterator_tag());
  }
  rb_tree(rb_tree&& other) noexcept = default;
  rb_tree(rb_tree&& other, const allocator_type& alloc)
      : _Base(_NodeAllocType(alloc)) {
    if (alloc == other.get_allocator()) {
      _impl.__swap(other._impl);
      return;
    }
    rb_tree::__copy_equal(_impl, other.begin(), other.end(),
                          std::bidirectional_iterator_tag());
  }
  rb_tree(std::initializer_list<value_type> ilist,
          const allocator_type& alloc = allocator_type())
      : _Base(_NodeAllocType(alloc)) {
    rb_tree::__copy_equal_n(_impl, ilist.begin(), ilist.size());
  }

  /////////////// destructor ///////////////
 public:
  ~rb_tree() noexcept = default;

  /////////////// member function ///////////////
 public:
  template <typename InputIt,
            typename = typename std::enable_if<
                std::is_base_of<std::input_iterator_tag,
                                typename std::iterator_traits<
                                    InputIt>::iterator_category>::value>::type*>
  void assign_equal(InputIt first, InputIt last) {
    using _Category = typename std::iterator_traits<InputIt>::iterator_category;
    this->clear();
    rb_tree::__copy_equal(_impl, first, last, _Category());
  }
  template <typename InputIt,
            typename = typename std::enable_if<
                std::is_base_of<std::input_iterator_tag,
                                typename std::iterator_traits<
                                    InputIt>::iterator_category>::value>::type*>
  void assign_unique(InputIt first, InputIt last) {
    using _Category = typename std::iterator_traits<InputIt>::iterator_category;
    this->clear();
    rb_tree::__copy_unique(_impl, first, last, _Category());
  }
  template <typename Tree, typename Alloc,
            typename = typename std::enable_if<
                std::is_base_of<
                    __container_base::_TreeBase<value_type, Alloc>,
                    Tree>::value>::type*>
  void assign_equal(const Tree& other) {
    if (&other == this) {
      return;
    }
    this->clear();
    rb_tree::__copy_equal_n(_impl, other.begin(), other.size());
  }
  template <typename Tree,
            typename = typename std::enable_if<
                std::is_base_of<
                    __container_base::_TreeBase<value_type, allocator_type>,
                    Tree>::value>::type*>
  void assign_equal_copy_allocator(const Tree& other) {
    if (&other == this) {
      return *this;
    }
    this->clear();
    if (_DataAllocTraits::propagate_on_container_copy_assignment::value &&
        other.get_allocator() != this->get_allocator()) {
      static_cast<allocator_type&>(_impl) =
          static_cast<typename Tree::allocator_type&>(other._impl);
    }
    rb_tree::__copy_equal_n(_impl, other.begin(), other.size());
  }
  void assign_equal_move_allocator(rb_tree&& other) {
    if (&other == this) {
      return *this;
    }
    this->clear();
    if (_DataAllocTraits::propagate_on_container_move_assignment::value) {
      _impl.__swap(other._impl);
      static_cast<allocator_type&>(_impl) = std::move(
          static_cast<allocator_type&>(other._impl));
    } else {
      rb_tree::__copy_equal_n(_impl, other.begin(), other.size());
    }
  }
  template <typename Tree, typename Alloc,
            typename = typename std::enable_if<
                std::is_base_of<
                    __container_base::_TreeBase<value_type, Alloc>,
                    Tree>::value>::type*>
  void assign_unique(const Tree& other) {
    if (&other == this) {
      return;
    }
    this->clear();
    rb_tree::__copy_unique_n(_impl, other.begin(), other.size());
  }
  template <typename Tree,
            typename = typename std::enable_if<
                std::is_base_of<
                    __container_base::_TreeBase<value_type, allocator_type>,
                    Tree>::value>::type*>
  void assign_unique_copy_allocator(const Tree& other) {
    if (&other == this) {
      return *this;
    }
    this->clear();
    if (_DataAllocTraits::propagate_on_container_copy_assignment::value &&
        other.get_allocator() != this->get_allocator()) {
      static_cast<allocator_type&>(_impl) =
          static_cast<typename Tree::allocator_type&>(other._impl);
    }
    rb_tree::__copy_unique_n(_impl, other.begin(), other.size());
  }
  void assign_unique_move_allocator(rb_tree&& other) {
    if (&other == this) {
      return *this;
    }
    this->clear();
    if (_DataAllocTraits::propagate_on_container_move_assignment::value) {
      _impl.__swap(other._impl);
      static_cast<allocator_type&>(_impl) = std::move(
          static_cast<allocator_type&>(other._impl));
    } else {
      rb_tree::__copy_unique_n(_impl, other.begin(), other.size());
    }
  }
  void assign_equal(std::initializer_list<value_type> ilist) {
    this->clear();
    rb_tree::__copy_equal_n(_impl, ilist.begin(), ilist.end());
  }
  void assign_unique(std::initializer_list<value_type> ilist) {
    this->clear();
    rb_tree::__copy_unique_n(_impl, ilist.begin(), ilist.end());
  }
  template <typename Tree,
            typename = typename std::enable_if<
                std::is_base_of<
                    __container_base::_TreeBase<value_type, allocator_type>,
                    Tree>::value>::type*>
  rb_tree& operator=(const Tree& other) {
    if (&other == this) {
      return *this;
    }
    this->clear();
    if (_DataAllocTraits::propagate_on_container_copy_assignment::value &&
        other.get_allocator() != this->get_allocator()) {
      static_cast<allocator_type&>(_impl) =
          static_cast<typename Tree::allocator_type&>(other._impl);
    }
    rb_tree::__copy_equal_n(_impl, other.begin(), other.size());
    return *this;
  }
  rb_tree& operator=(rb_tree&& other) {
    if (this->get_allocator() == other.get_allocator()) {
      _impl.__swap(other._impl);
      return *this;
    }
    this->clear();
    if (_DataAllocTraits::propagate_on_container_move_assignment::value) {
      _impl.__swap(other._impl);
      static_cast<allocator_type&>(_impl) = std::move(
          static_cast<allocator_type&>(other._impl));
    } else {
      rb_tree::__copy_equal_n(_impl, other.begin(), other.size());
    }
    return *this;
  }
  rb_tree& operator=(std::initializer_list<value_type> ilist) {
    this->clear();
    rb_tree::__copy_equal_n(_impl, ilist.begin(), ilist.size());
    return *this;
  }

  /////////////// modifier ///////////////
 public:
  iterator insert_equal(const value_type& value) {
    return this->emplace_equal(value);
  }
  iterator insert_equal(value_type&& value) {
    return this->emplace_equal(std::forward<value_type>(value));
  }
  template <typename InputIt,
            typename = typename std::enable_if<
                std::is_base_of<std::input_iterator_tag,
                                typename std::iterator_traits<
                                    InputIt>::iterator_category>::value>::type*>
  iterator insert_equal(InputIt first, InputIt last) {
    while (first != last) {
      this->emplace_equal(*first);
      ++first;
    }
  }
  template <typename InputIt,
            typename = typename std::enable_if<
                std::is_base_of<std::input_iterator_tag,
                                typename std::iterator_traits<
                                    InputIt>::iterator_category>::value>::type*>
  iterator insert_unique(InputIt first, InputIt last) {
    while (first != last) {
      this->emplace_unique(*first);
      ++first;
    }
  }
  std::pair<iterator, bool> insert_unique(const value_type& value) {
    return this->emplace_unique(value);
  }
  std::pair<iterator, bool> insert_unique(value_type&& value) {
    return this->emplace_unique(std::forward<value_type>(value));
  }
  template <typename... Args>
  iterator emplace_equal(Args&&... args) {
    _NodeBase* _node = _Base::__get(_impl, std::forward<Args>(args)...);
    tree::__insert_equal(_node, &_impl._header);
    rb_tree::__insert_rebalance(_node, static_cast<_NodeBase*>(&_impl._header));
    ++_impl._header._size;
    return iterator(_node);
  }
  template <typename... Args>
  std::pair<iterator, bool> emplace_unique(Args&&... args) {
    _NodeBase* _node = _Base::__get(_impl, std::forward<Args>(args)...);
    _NodeBase* _tmp = _node;
    if (tree::__insert_unique(_node, &_impl._header)) {
      rb_tree::__insert_rebalance(_node,
                                  static_cast<_NodeBase*>(&_impl._header));
      ++_impl._header._size;
      return {iterator(_node), true};
    }
    _Base::__put(_impl, static_cast<_NodeType*>(_tmp));
    return {iterator(_node), false};
  }
  iterator insert_hint_equal(const_iterator hint, const value_type& value) {
    return this->emplace_hint_equal(hint, value);
  }
  iterator insert_hint_equal(const_iterator hint, value_type&& value) {
    return this->emplace_hint_equal(hint, std::forward(value));
  }
  std::pair<iterator, bool> insert_hint_unique(const_iterator hint,
                                               const value_type& value) {
    return this->emplace_hint_unique(hint, value);
  }
  std::pair<iterator, bool> insert_hint_unique(const_iterator hint,
                                               value_type&& value) {
    return this->emplace_hint_unique(hint, std::forward(value));
  }
  template <typename... Args>
  iterator emplace_hint_equal(const_iterator hint, Args&&... args) {
    _NodeBase* _node = _Base::__get(_impl, std::forward<Args>(args)...);
    tree::__insert_hint_equal(hint._node, _node, &_impl._header);
    rb_tree::__insert_rebalance(_node, static_cast<_NodeBase*>(&_impl._header));
    ++_impl._header._size;
    return iterator(_node);
  }
  template <typename... Args>
  std::pair<iterator, bool> emplace_hint_unique(const_iterator hint,
                                                Args&&... args) {
    _NodeBase* _node = _Base::__get(_impl, std::forward<Args>(args)...);
    _NodeBase* _tmp = _node;
    if (tree::__insert_hint_unique(hint._node, _node, &_impl._header)) {
      rb_tree::__insert_rebalance(_node,
                                  static_cast<_NodeBase*>(&_impl._header));
      ++_impl._header._size;
      return {iterator(_node), true};
    }
    _Base::__put(_impl, static_cast<_NodeType*>(_tmp));
    return {iterator(_node), false};
  }
  iterator erase(const_iterator pos) {
    iterator _ret = iterator(pos._node);
    ++_ret;
    rb_tree::__erase_aux(pos._node, static_cast<_NodeBase*>(&_impl._header));
    return _ret;
  }
  iterator erase(const_iterator first, const_iterator last) {
    _NodeBase* _header = static_cast<_NodeBase*>(&_impl._header);
    if (first._node == _impl._header._left && last._node == _header) {
      _Base::__clear(_impl);
    } else {
      while (first != last) {
        rb_tree::__erase_aux(first._node, _header);
        ++first;
      }
    }
    return iterator(last._node);
  }
  template <typename Key,
            typename = typename std::enable_if<
                std::is_convertible<value_type, Key>::value>::type*>
  size_type erase(const Key& key) {
    std::pair<iterator, iterator> _pair = this->equal_range(key);
    const size_type _old = _impl._header._size;
    this->erase(_pair.first, _pair._second);
    return _old - _impl._header._size;
  }
  node_type extract(const_iterator pos) {
    return rb_tree::__extract_aux(pos._node,
                                  static_cast<_NodeBase*>(&_impl._header));
  }
  template <typename Key,
            typename = typename std::enable_if<
                std::is_convertible<value_type, Key>::value>::type*>
  node_type extract(const Key& key) {
    iterator _pos = this->find(key);
    if (_pos._node == static_cast<_NodeBase*>(&_impl._header)) {
      return nullptr;
    }
    return rb_tree::__extract_aux(_pos._node,
                                  static_cast<_NodeBase*>(&_impl._header));
  }
  template <typename Tree,
            typename = typename std::enable_if<
                std::is_base_of<
                    __container_base::_TreeBase<value_type, allocator_type>,
                    Tree>::value &&
                !std::is_same<Tree, rb_tree>::value>::type*>
  void merge_equal(const Tree& other) {
    typename Tree::iterator _it = other.begin();
    _NodeBase* _node;
    for (typename Tree::size_type _i = 0, _n = other.size(); _i < _n; ++_i) {
      _node = _Base::__get(_impl, *_it);
      tree::__insert_equal(static_cast<_NodeType*>(_node), &_impl._header);
      rb_tree::__insert_rebalance(_node,
                                  static_cast<_NodeBase*>(&_impl._header));
      ++_impl._header._size;
      ++_it;
    }
  }
  void merge_equal(rb_tree& other) {
    if (this == &other || this->get_allocator() != other.get_allocator()) {
      return;
    }
    this->merge_equal(std::move(other));
  }
  void merge_equal(rb_tree&& other) {
    _NodeType* _cur = static_cast<_NodeType*>(other._impl._header._left);
    _NodeBase* _next = tree::__node_increment(_cur);
    _NodeBase* _end = static_cast<_NodeBase*>(&other._impl._header);
    for (_NodeHeader* _header = &_impl._header; _cur != _end;
         _cur = _next, _next = tree::__node_increment(_next)) {
      tree::__insert_equal(_cur, _header);
      _cur->_tag = _RBTreeColor_RED;
      rb_tree::__insert_rebalance(_cur, _header);
      ++_impl._header._size;
    }
  }
  template <typename Tree,
            typename = typename std::enable_if<
                std::is_base_of<
                    __container_base::_TreeBase<value_type, allocator_type>,
                    Tree>::value &&
                !std::is_same<Tree, rb_tree>::value>::type*>
  void merge_unique(const Tree& other);
  void merge_unique(rb_tree& other) {
    if (this == &other || this->get_allocator() != other.get_allocator()) {
      return;
    }
    this->merge_unique(std::move(other));
  }
  void merge_unique(rb_tree&& other);
  void swap(rb_tree& other) noexcept(
      _NodeAllocTraits::propagate_on_container_swap::value ||
      _NodeAllocTraits::is_always_equal::value) {
    if (this == &other) {
      return;
    }
    _Base::__swap_allocator(_impl, other._impl);
    _impl.__swap(other._impl);
  }

  /////////////// insert ///////////////
 private:
  static void __insert_rebalance(_NodeBase* _node, _NodeBase*& _header);

  /////////////// erase ///////////////
 private:
  static void __erase_aux(_NodeBase* _node, _NodeBase* _header) {
    _NodeBase* _child = nullptr,* _parent = nullptr;
    rb_tree::__erase_convert(_node, _header, &_child, &_parent);
    if (_node->_tag != _RBTreeColor_RED) {
      rb_tree::__delete_rebalance(_child, _parent, _header);
    }
    _Base::__put(_impl, _node);
    --_impl._header._size;
  }
  static node_type __extract_aux(_NodeBase* _node, _NodeBase* _header) {
    _NodeBase* _child = nullptr,* _parent = nullptr;
    rb_tree::__erase_convert(_node, _header, &_child, &_parent);
    if (_node->_tag != _RBTreeColor_RED) {
      rb_tree::__delete_rebalance(_child, _parent, _header);
    }
    --_impl._header._size;
    _node->_parent = _node->_left = _node->_right = nullptr;
    return static_cast<node_type>(_node);
  }
  static void __erase_convert(_NodeBase* _node, _NodeBase*& _header,
                              _NodeBase** _child, _NodeBase** _parent);
  static void __delete_rebalance(_NodeBase* _node, _NodeBase* _parent,
                                 _NodeBase*& _header);

  /////////////// copy ///////////////
 private:
  template <typename _InputIt>
  static void __copy_equal(_BaseImpl& _impl, _InputIt _first, _InputIt _last,
                           std::input_iterator_tag) {
    _NodeBase* _n;
    for (; _first != _last; ++_first) {
      _n = _Base::__get(_impl, *_first);
      tree::__insert_equal(_n, static_cast<_NodeBase*>(&_impl._header));
      rb_tree::__insert_rebalance(_n, static_cast<_NodeBase*>(&_impl._header));
      ++_impl._header._size;
    }
  }
  template <typename _RandIt>
  static void __copy_equal(_BaseImpl& _impl, _RandIt _first, _RandIt _last,
                           std::random_access_iterator_tag) {
    rb_tree::__copy_equal_n(_impl, _first, _last - _first);
  }
  template <typename _InputIt>
  static void __copy_unique(_BaseImpl& _impl, _InputIt _first, _InputIt _last,
                            std::input_iterator_tag) {
    _NodeBase* _n;
    for (; _first != _last; ++_first) {
      _n = _Base::__get(_impl, *_first);
      tree::__insert_unique(_n, static_cast<_NodeBase*>(&_impl._header));
      rb_tree::__insert_rebalance(_n, static_cast<_NodeBase*>(&_impl._header));
      ++_impl._header._size;
    }
  }
  template <typename _RandIt>
  static void __copy_unique(_BaseImpl& _impl, _RandIt _first, _RandIt _last,
                            std::random_access_iterator_tag) {
    rb_tree::__copy_unique_n(_impl, _first, _last - _first);
  }
  template <typename _InputIt>
  static void __copy_equal_n(_BaseImpl& _impl, _InputIt _first,
                             const size_type& _cnt) {
    _NodeBase* _n;
    for (size_type _n = 0; _n < _cnt; ++_n) {
      _n = _Base::__get(_impl, *_first);
      tree::__insert_equal(_n, static_cast<_NodeBase*>(&_impl._header));
      rb_tree::__insert_rebalance(_n, static_cast<_NodeBase*>(&_impl._header));
      ++_impl._header._size;
      ++_first;
    }
  }
  template <typename _InputIt>
  static void __copy_unique_n(_BaseImpl& _impl, _InputIt _first,
                              const size_type& _cnt) {
    _NodeBase* _n;
    for (size_type _n = 0; _n < _cnt; ++_n) {
      _n = _Base::__get(_impl, *_first);
      tree::__insert_unique(_n, static_cast<_NodeBase*>(&_impl._header));
      rb_tree::__insert_rebalance(_n, static_cast<_NodeBase*>(&_impl._header));
      ++_impl._header._size;
      ++_first;
    }
  }
};



///////////////////////// java style iterator /////////////////////////
template <typename T>
using RBTreeIterator = __container_base::_TreeIterator<T, T*, T&>;
template <typename T>
using RBTreeConstIterator =
    __container_base::_TreeIterator<T, const T*, const T&>;



///////////////////////// rb_tree comparison operators /////////////////////////
template <typename T, typename Allocator>
constexpr bool operator==(const rb_tree<T, Allocator>& lhs,
                          const rb_tree<T, Allocator>& rhs) {
  return &lhs == &rhs || (lhs.size() == rhs.size() &&
                          std::equal(lhs.begin(), lhs.end(), rhs.begin()));
}
template <typename T, typename Allocator>
constexpr bool operator!=(const rb_tree<T, Allocator>& lhs,
                          const rb_tree<T, Allocator>& rhs) {
  return &lhs != &rhs && (lhs.size() != rhs.size() ||
                          !std::equal(lhs.begin(), lhs.end(), rhs.begin()));
}
template <typename T, typename Allocator>
constexpr bool operator<(const rb_tree<T, Allocator>& lhs,
                         const rb_tree<T, Allocator>& rhs) {
  return std::lexicographical_compare(lhs.begin(), lhs.end(),
                                      rhs.begin(), rhs.end());
}
template <typename T, typename Allocator>
constexpr bool operator>(const rb_tree<T, Allocator>& lhs,
                         const rb_tree<T, Allocator>& rhs) {
  return std::lexicographical_compare(rhs.begin(), rhs.end(),
                                      lhs.begin(), lhs.end());
}
template <typename T, typename Allocator>
constexpr bool operator<=(const rb_tree<T, Allocator>& lhs,
                          const rb_tree<T, Allocator>& rhs) {
  return !operator<(rhs, lhs);
}
template <typename T, typename Allocator>
constexpr bool operator>=(const rb_tree<T, Allocator>& lhs,
                          const rb_tree<T, Allocator>& rhs) {
  return !operator<(lhs, rhs);
}



///////////////////////// specialization /////////////////////////
template <typename T, typename Allocator>
inline void swap(rb_tree<T, Allocator>& lhs, rb_tree<T, Allocator>& rhs) {
  lhs.swap(rhs);
}

// erase_if
template <typename T, typename Allocator, typename Pred>
auto erase_if(rb_tree<T, Allocator>& c, Pred pred) ->
    typename rb_tree<T, Allocator>::size_type {
  auto _old = c.size();
  for (auto _it = c.begin(), _last = c.end(); _it != _last;) {
    pred(*_it) ? _it = c.erase(_it) : ++_it;
  }
  return _old - c.size();
}

}  // namespace stl
}  // namespace ginshio





namespace std {
///////////////////////// specialization /////////////////////////
template <typename T, typename Allocator>
inline void swap(ginshio::stl::rb_tree<T, Allocator>& lhs,
                 ginshio::stl::rb_tree<T, Allocator>& rhs) {
  lhs.swap(rhs);
}

// erase_if
template <typename T, typename Allocator, typename Pred>
auto erase_if(ginshio::stl::rb_tree<T, Allocator>& c, Pred pred) ->
    typename ginshio::stl::rb_tree<T, Allocator>::size_type {
  auto _old = c.size();
  for (auto _it = c.begin(), _last = c.end(); _it != _last;) {
    pred(*_it) ? _it = c.erase(_it) : ++_it;
  }
  return _old - c.size();
}
}  // namespace std

#endif  // GINSHIO_STL__CONTAINER_STL_RB_TREE_HH_
