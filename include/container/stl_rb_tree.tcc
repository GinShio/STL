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

template <typename _T, typename _Alloc>
template <typename Tree,
          typename = typename std::enable_if<
              std::is_base_of<
                  __container_base::_TreeBase<
                      typename rb_tree<_T, _Alloc>::value_type,
                      typename rb_tree<_T, _Alloc>::allocator_type>,
                  Tree>::value &&
              !std::is_same<Tree, rb_tree<_T, _Alloc>>::value>::type*>
void rb_tree<_T, _Alloc>::merge_unique(const Tree& other) {
  typename Tree::iterator _it = other.begin();
  _NodeBase* _node;
  for (typename Tree::size_type _i = 0, _n = other.size(); _i < _n; ++_i) {
    _node = _Base::__get(_impl, *_it);
    if (tree::__insert_unique(static_cast<_NodeType*>(_node), &_impl._header)) {
      rb_tree::__insert_rebalance(_node,
                                  static_cast<_NodeBase*>(&_impl._header));
      ++_impl._header._size;
    } else {
      _Base::__put(_impl, static_cast<_NodeType*>(_node));
    }
    ++_it;
  }
}

template <typename _T, typename _Alloc>
void rb_tree<_T, _Alloc>::merge_unique(rb_tree<_T, _Alloc>&& other) {
  _NodeType* _cur = static_cast<_NodeType*>(other._impl._header._left);
  _NodeBase* _next = tree::__node_increment(_cur);
  _NodeBase* _end = static_cast<_NodeBase*>(&other._impl._header);
  _NodeType* _tmp = _cur;
  for (_NodeHeader* _header = &_impl._header; _cur != _end;
       _cur = _next, _next = tree::__node_increment(_next), _tmp = _cur) {
    if (tree::__insert_unique(_cur, _header)) {
      _cur->_tag = _RBTreeColor_RED;
      rb_tree::__insert_rebalance(_cur, _header);
      ++_impl._header._size;
    } else {
      _Base::__put(_impl, _tmp);
    }
  }
}

template <typename _T, typename _Alloc>
static void rb_tree<_T, _Alloc>::__insert_rebalance(
    typename rb_tree<_T, _Alloc>::_NodeBase* _node,
    typename rb_tree<_T, _Alloc>::_NodeBase*& _header) {
  typename rb_tree<_T, _Alloc>::_NodeBase* _uncle,* _gp;
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
    if (_node == _node->_parent->_right) {
      if (_node == _node->_parent->_left) {
        _node = _node->_parent;
        tree::__rotate_right(_node, _header->_parent);
      }
      _node->_parent->_tag = _RBTreeColor_BLACK;
      _gp->_parent->_tag = _RBTreeColor_RED;
      tree::__rotate_left(_gp, _header->_parent);
    } else {
      if (_node == _node->_parent->_right) {
        _node = _node->_parent;
        tree::__rotate_left(_node, _header->_parent);
      }
      _node->_parent->_tag = _RBTreeColor_BLACK;
      _gp->_parent->_tag = _RBTreeColor_RED;
      tree::__rotate_right(_gp, _header->_parent);
    }
  }
  _header->_parent->_tag = _RBTreeColor_BLACK;
}

template <typename _T, typename _Alloc>
static void rb_tree<_T, _Alloc>::__erase_convert(
    rb_tree<_T, _Alloc>::_NodeBase* _node,
    rb_tree<_T, _Alloc>::_NodeBase*& _header,
    rb_tree<_T, _Alloc>::_NodeBase** _child,
    rb_tree<_T, _Alloc>::_NodeBase** _parent) {
  rb_tree<_T, _Alloc>::_NodeBase* _tmp = _node;
  if (_tmp->_left == nullptr) {
    *_child = _tmp->_right;  // __x might be nullptr
  } else {
    if (_tmp->_right == nullptr) {
      *_child = _tmp->_left;  // __x != nullptr
    } else {
      _tmp = tree::__get_leftmost(_tmp->_right);
      *_child = _tmp->_right;  // __x might be nullptr
    }
  }
  if (_tmp != _node) {  // __y is leftmost in _node->_right's tree
    _node->_left->_parent = _tmp;
    _tmp->_left = _node->_left;
    if (_tmp != _node->_right) {
      *_parent = _tmp->_parent;
      *_child != nullptr
          ? static_cast<void>((*_child)->_parent = _tmp->_parent)
          : void();
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
    std::swap(_tmp->_tag, _node->_tag);
  } else {  // __y == _node
    *_parent = _tmp->_parent;
    *_child != nullptr ? static_cast<void>((*_child)->_parent = _tmp->_parent)
                       : void();
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
      _header->_left = _node->_right == nullptr
                           ? _node->_parent
                           : tree::__get_leftmost(*_child);
    }
    if (_node == _header->_right) {
      _header->_right = _node->_left == nullptr
                            ? _node->_parent
                            : tree::__get_rightmost(*_child);
    }
  }
}

template <typename _T, typename _Alloc>
static void rb_tree<_T, _Alloc>::__delete_rebalance(
    rb_tree<_T, _Alloc>::_NodeBase* _node,
    rb_tree<_T, _Alloc>::_NodeBase* _parent,
    rb_tree<_T, _Alloc>::_NodeBase*& _header) {
  rb_tree<_T, _Alloc>::_NodeBase* _sibling = nullptr;
  while (_node != _header->_parent &&
         (_node == nullptr || _node->_tag == _RBTreeColor_BLACK)) {
    _sibling = _node == _parent->_left ? _parent->_right : _parent->_left;
    if (_sibling->_tag == _RBTreeColor_RED) {
      _parent->_tag = _RBTreeColor_RED;
      _sibling->_tag = _RBTreeColor_BLACK;
      _node == _parent->_left
          ? tree::__rotate_left(_parent, _header->_parent)
          : tree::__rotate_right(_parent, _header->_parent);
      _sibling = tree::__get_sibling(_node);
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
    break;
  }
  if (_node == _parent->_left) {
    if (_sibling->_right == nullptr ||
        _sibling->_right->_tag == _RBTreeColor_BLACK) {
      if (_sibling->_left != nullptr) {
        _sibling->_left->_tag = _RBTreeColor_BLACK;
      }
      _sibling->_tag = _RBTreeColor_RED;
      tree::__rotate_right(_sibling, _header->_parent);
      _sibling = _parent->_right;
    }
    _sibling->_tag = _parent->_tag;
    _parent->_tag = _RBTreeColor_BLACK;
    if (_sibling->_right != nullptr) {
      _sibling->_right->_tag = _RBTreeColor_BLACK;
    }
    tree::__rotate_left(_parent, _header->_parent);
  } else {
    if (_sibling->_left == nullptr ||
        _sibling->_left->_tag == _RBTreeColor_BLACK) {
      if (_sibling->_right != nullptr) {
        _sibling->_right->_tag = _RBTreeColor_BLACK;
      }
      _sibling->_tag = _RBTreeColor_RED;
      tree::__rotate_left(_sibling, _header->_parent);
      _sibling = _parent->_left;
    }
    _sibling->_tag = _parent->_tag;
    _parent->_tag = _RBTreeColor_BLACK;
    if (_sibling->_left != nullptr) {
      _sibling->_left->_tag = _RBTreeColor_BLACK;
    }
    tree::__rotate_right(_parent, _header->_parent);
  }
  if (_node != nullptr) {
    _node->_tag = _RBTreeColor_BLACK;
  }
}

}  // namespace stl
}  // namespace ginshio

#endif  // GINSHIO_STL__CONTAINER_STL_RB_TREE_TCC_
