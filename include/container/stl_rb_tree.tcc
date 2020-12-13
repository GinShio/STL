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

#ifndef GINSHIO_STL__CONTAINER_STL_RB_TREE_TCC_
#define GINSHIO_STL__CONTAINER_STL_RB_TREE_TCC_ 1

namespace ginshio {
namespace stl {

template <typename T, typename Compare, typename KeyOfValue, typename Allocator>
void rb_tree<T, Compare, KeyOfValue, Allocator>::__insert_rebalance(
    typename rb_tree<T, Compare, KeyOfValue, Allocator>::_NodeBase* _node,
    typename rb_tree<T, Compare, KeyOfValue, Allocator>::_NodeHeader*& _header) {
  typename rb_tree<T, Compare, KeyOfValue, Allocator>::_NodeBase* _uncle,* _gp;
  while (_node != _header->_parent &&
         _node->_parent->_tag == _RBTreeColor_RED) {
    _uncle = tree::__get_uncle(_node);
    _gp = tree::__get_grandparent(_node);
    if (_uncle && _uncle->_tag == _RBTreeColor_RED) {
      _node->_parent->_tag = _RBTreeColor_BLACK;
      _uncle->_tag = _RBTreeColor_BLACK;
      _gp->_tag = _RBTreeColor_BLACK;
      _node = _gp;
      continue;
    }
    if (_node->_parent == _gp->_left) {
      if (_node == _node->_parent->_right) {
        _node = _node->_parent;
        tree::__rotate_left(_node, _header);
      }
      _node->_parent->_tag = _RBTreeColor_BLACK;
      _gp->_tag = _RBTreeColor_RED;
      tree::__rotate_right(_gp, _header);
    } else {
      if (_node == _node->_parent->_left) {
        _node = _node->_parent;
        tree::__rotate_right(_node, _header);
      }
      _node->_parent->_tag = _RBTreeColor_BLACK;
      _gp->_tag = _RBTreeColor_RED;
      tree::__rotate_left(_gp, _header);
    }
  }
  _header->_parent->_tag = _RBTreeColor_BLACK;
}

template <typename T, typename Compare, typename KeyOfValue, typename Allocator>
auto rb_tree<T, Compare, KeyOfValue, Allocator>::__erase_convert(
    _NodeBase* _node, _NodeHeader*& _header,
    _NodeBase** _child, _NodeBase** _parent) -> _NodeBase* {
  _NodeBase* _tmp = _node;
  if (_tmp->_left == nullptr) {
    *_child = _tmp->_right;  // _child might be nullptr
  } else {
    if (_tmp->_right == nullptr) {
      *_child = _tmp->_left;  // _child != nullptr
    } else {
      _tmp = tree::__get_leftmost(_tmp->_right);
      *_child = _tmp->_right;  // _child might be nullptr
    }
  }
  if (_tmp != _node) {
    // _tmp is leftmost in _node->_right's tree, swap _tmp & _node
    _node->_left->_parent = _tmp;
    _tmp->_left = _node->_left;
    if (_tmp != _node->_right) {
      *_parent = _tmp->_parent;
      *_child != nullptr ?
          static_cast<void>((*_child)->_parent = _tmp->_parent) : void();
      _tmp->_parent->_left = *_child;
      _tmp->_right = _node->_right;
      _node->_right->_parent = _tmp;
    } else {
      *_parent = _tmp;
    }
    if (_header->_parent == _node) {
      _header->_parent = _tmp;
    } else if (_node->_parent->_left == _node) {
      _node->_parent->_left = _tmp;
    } else {
      _node->_parent->_right = _tmp;
    }
    _tmp->_parent = _node->_parent;
    ::std::swap(_tmp->_tag, _node->_tag);
    _tmp = _node;
  } else {  // _tmp == _node
    *_parent = _tmp->_parent;
    *_child != nullptr ?
        static_cast<void>((*_child)->_parent = _tmp->_parent) : void();
    if (_header->_parent == _node) {
      _header->_parent = *_child;
    } else {
      if (_node->_parent->_left == _node) {
        _node->_parent->_left = *_child;
      } else {
        _node->_parent->_right = *_child;
      }
    }
    if (_node == _header->_left) {
      _header->_left = _node->_right == nullptr ?
          _node->_parent : tree::__get_leftmost(*_child);
    }
    if (_node == _header->_right) {
      _header->_right = _node->_left == nullptr ?
          _node->_parent : tree::__get_rightmost(*_child);
    }
  }
  return _tmp;
}

template <typename T, typename Compare, typename KeyOfValue, typename Allocator>
void rb_tree<T, Compare, KeyOfValue, Allocator>::__delete_rebalance(
    _NodeBase* _node, _NodeBase* _parent, _NodeHeader*& _header) {
  while (_node != _header->_parent &&
         (_node == nullptr || _node->_tag == _RBTreeColor_BLACK)) {
    bool _is_left = _node == _parent->_left;
    _NodeBase* _sibling = _is_left ? _parent->_right : _parent->_left;
    if (_sibling->_tag == _RBTreeColor_RED) {
      _parent->_tag = _RBTreeColor_RED;
      _sibling->_tag = _RBTreeColor_BLACK;
      if (_is_left) {
        tree::__rotate_left(_parent, _header);
        _sibling = _parent->_right;
      } else {
        tree::__rotate_right(_parent, _header);
        _sibling = _parent->_left;
      }
    }
    if ((_sibling->_left == nullptr ||
         _sibling->_left->_tag == _RBTreeColor_BLACK) &&
        (_sibling->_right == nullptr ||
         _sibling->_right->_tag == _RBTreeColor_BLACK)) {
      _sibling->_tag = _RBTreeColor_RED;
      _node = _parent;
      _parent = _parent->_parent;
      continue;
    }
    if (_is_left) {
      if (_sibling->_right == nullptr ||
          _sibling->_right->_tag == _RBTreeColor_BLACK) {
        if (_sibling->_left != nullptr) {
          _sibling->_left->_tag = _RBTreeColor_BLACK;
        }
        _sibling->_tag = _RBTreeColor_RED;
        tree::__rotate_right(_sibling, _header);
        _sibling = _parent->_right;
      }
      _sibling->_tag = _parent->_tag;
      _parent->_tag = _RBTreeColor_BLACK;
      if (_sibling->_right != nullptr) {
        _sibling->_right->_tag = _RBTreeColor_BLACK;
      }
      tree::__rotate_left(_parent, _header);
    } else {
      if (_sibling->_left == nullptr ||
          _sibling->_left->_tag == _RBTreeColor_BLACK) {
        if (_sibling->_right != nullptr) {
          _sibling->_right->_tag = _RBTreeColor_BLACK;
        }
        _sibling->_tag = _RBTreeColor_RED;
        tree::__rotate_left(_sibling, _header);
        _sibling = _parent->_left;
      }
      _sibling->_tag = _parent->_tag;
      _parent->_tag = _RBTreeColor_BLACK;
      if (_sibling->_left != nullptr) {
        _sibling->_left->_tag = _RBTreeColor_BLACK;
      }
      tree::__rotate_right(_parent, _header);
    }
    break;
  }
  if (_node != nullptr) {
    _node->_tag = _RBTreeColor_BLACK;
  }
}

template <typename T, typename Compare, typename KeyOfValue, typename Allocator>
template <typename _InputIt>
void rb_tree<T, Compare, KeyOfValue, Allocator>::__copy_equal(
    typename rb_tree<T, Compare, KeyOfValue, Allocator>::_BaseImpl& _impl,
    _InputIt _first, _InputIt _last, ::std::input_iterator_tag) {
  _NodeHeader* _header = &_impl._header;
  try {
    _NodeType* _node;
    for (; _first != _last; ++_first) {
      _node = static_cast<_NodeType*>(_Base::__get(_impl, *_first));
      tree::__insert_equal(_node, _header,
                           static_cast<typename rb_tree::key_compare>(_impl),
                           static_cast<typename rb_tree::key_of_value>(_impl));
      rb_tree::__insert_rebalance(_node, _header);
      ++_impl._header._size;
    }
  } catch (...) {
    _Base::__clear_aux(_impl, _impl._header._parent);
    throw;
  }
}

template <typename T, typename Compare, typename KeyOfValue, typename Allocator>
template <typename _InputIt>
void rb_tree<T, Compare, KeyOfValue, Allocator>::__copy_equal_n(
    typename rb_tree<T, Compare, KeyOfValue, Allocator>::_BaseImpl& _impl,
    _InputIt _first,
    typename rb_tree<T, Compare, KeyOfValue, Allocator>::size_type _cnt) {
  _NodeHeader* _header = &_impl._header;
  try {
    _NodeType* _node;
    for (; _cnt > 0; --_cnt) {
      _node = static_cast<_NodeType*>(_Base::__get(_impl, *_first));
      tree::__insert_equal(_node, _header,
                           static_cast<typename rb_tree::key_compare>(_impl),
                           static_cast<typename rb_tree::key_of_value>(_impl));
      rb_tree::__insert_rebalance(static_cast<_NodeBase*>(_node), _header);
      ++_impl._header._size;
      ++_first;
    }
  } catch (...) {
    _Base::__clear_aux(_impl, _impl._header._parent);
    throw;
  }
}

template <typename T, typename Compare, typename KeyOfValue, typename Allocator>
template <typename _InputIt>
void rb_tree<T, Compare, KeyOfValue, Allocator>::__copy_unique(
    typename rb_tree<T, Compare, KeyOfValue, Allocator>::_BaseImpl& _impl,
    _InputIt _first, _InputIt _last, ::std::input_iterator_tag) {
  _NodeHeader* _header = &_impl._header;
  try {
    _NodeType* _node;
    for (; _first != _last; ++_first) {
      _node = static_cast<_NodeType*>(_Base::__get(_impl, *_first));
      tree::__insert_unique(_node, _header,
                            static_cast<typename rb_tree::key_compare>(_impl),
                            static_cast<typename rb_tree::key_of_value>(_impl));
      rb_tree::__insert_rebalance(
          _node, static_cast<_NodeBase*>(&_impl._header));
      ++_impl._header._size;
    }
  } catch (...) {
    _Base::__clear_aux(_impl, _impl._header._parent);
    throw;
  }
}

template <typename T, typename Compare, typename KeyOfValue, typename Allocator>
template <typename _InputIt>
void rb_tree<T, Compare, KeyOfValue, Allocator>::__copy_unique_n(
    typename rb_tree<T, Compare, KeyOfValue, Allocator>::_BaseImpl& _impl,
    _InputIt _first,
    typename rb_tree<T, Compare, KeyOfValue, Allocator>::size_type _cnt) {
  _NodeHeader* _header = &_impl._header;
  try {
    _NodeType* _node;
    for (size_type _n = _cnt; _n > 0; --_n) {
      _node = static_cast<_NodeType*>(_Base::__get(_impl, *_first));
      tree::__insert_unique(_node, _header,
                            static_cast<typename rb_tree::key_compare>(_impl),
                            static_cast<typename rb_tree::key_of_value>(_impl));
      rb_tree::__insert_rebalance(
          _node, static_cast<_NodeBase*>(&_impl._header));
      ++_impl._header._size;
      ++_first;
    }
  } catch (...) {
    _Base::__clear_aux(_impl, _impl._header._parent);
    throw;
  }
}

}  // namespace stl
}  // namespace ginshio

#endif  // GINSHIO_STL__CONTAINER_STL_RB_TREE_TCC_
