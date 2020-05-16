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


#ifndef GINSHIO_STL__STL_LIST_TCC_
#define GINSHIO_STL__STL_LIST_TCC_ 1

namespace ginshio {
namespace stl {

template <typename T, typename Allocator>
auto list<T, Allocator>::erase(const_iterator first, const_iterator last)
    -> typename list<T, Allocator>::iterator {
  _NodeBase* _cur = last._node->_prev;
  for (_NodeBase* _prev = _cur->_prev,* _end = first._node->_prev;
       _cur != _end; _cur = _prev, _prev = _prev->_prev) {
    _Base::__put(_impl, static_cast<_NodeType*>(_cur));
    --_impl._header._size;
  }
  _cur->_next = last._node;
  last._node->_prev = _cur;
  return iterator(last._node);
}

template <typename T, typename Allocator>
void list<T, Allocator>::merge(list<T, Allocator>&& other) {
  if (this == &other || this->get_allocator() != other.get_allocator()) {
    return;
  }
  size_type _t_n = 0, _o_n = 0;
  _NodeBase* _t_cur = _impl._header._next;
  _NodeBase* _o_cur = other._impl._header._next;
  try {
    while (_t_n < _impl._header._size && _o_n < other._impl._header._size) {
      if (static_cast<_NodeType*>(_o_cur)->_data <
          static_cast<_NodeType*>(_t_cur)->_data) {
        _NodeBase* _next = _o_cur->_next;
        _o_cur->__hook(_t_cur);
        _o_cur = _next;
        ++_o_n;
      } else {
        _t_cur = _t_cur->_next;
        ++_t_n;
      }
    }
  } catch (...) {
    _impl._header._size += _o_n;
    _o_cur->_prev = static_cast<_NodeBase*>(&other._impl._header);
    other._impl._header._next = _o_cur;
    other._impl._header._size -= _o_n;
    throw;
  }
  if (_o_n < other._impl._header._size) {
    _t_cur->_prev->_next = _o_cur;
    _o_cur->_prev = _t_cur->_prev;
    _t_cur->_prev = other._impl._header._prev;
    _t_cur->_prev->_next = _t_cur;
  }
  _impl._header._size += other._impl._header._size;
  other._impl._header.__init_default();
}

template <typename T, typename Allocator> template <typename Compare>
void list<T, Allocator>::merge(list<T, Allocator>&& other, Compare comp) {
  if (this == &other || this->get_allocator() != other.get_allocator()) {
    return;
  }
  size_type _t_n = 0, _o_n = 0;
  _NodeBase* _t_cur = _impl._header._next;
  _NodeBase* _o_cur = other._impl._header._next;
  try {
    while (_t_n < _impl._header._size && _o_n < other._impl._header._size) {
      if (comp(static_cast<_NodeType*>(_o_cur)->_data,
               static_cast<_NodeType*>(_t_cur)->_data)) {
        _NodeBase* _next = _o_cur->_next;
        _o_cur->__hook(_t_cur);
        _o_cur = _next;
        ++_o_n;
      } else {
        _t_cur = _t_cur->_next;
        ++_t_n;
      }
    }
  } catch (...) {
    _impl._header._size += _o_n;
    _o_cur->_prev = static_cast<_NodeBase*>(&other._impl._header);
    other._impl._header._next = _o_cur;
    other._impl._header._size -= _o_n;
    throw;
  }
  if (_o_n < other._impl._header._size) {
    _t_cur->_prev->_next = _o_cur;
    _o_cur->_prev = _t_cur->_prev;
    _t_cur->_prev = other._impl._header._prev;
    _t_cur->_prev->_next = _t_cur;
  }
  _impl._header._size += other._impl._header._size;
  other._impl._header.__init_default();
}

template <typename T, typename Allocator>
auto list<T, Allocator>::remove(const value_type& value)
    -> typename list<T, Allocator>::size_type {
  size_type _removed = 0;
  iterator _cur = iterator(_impl._header._next);
  iterator _next = iterator(_cur._node->_next);
  for (size_type _n = 0; _n < _impl._header._size; ++_n) {
    if (*_cur == value) {
      ++_removed;
      _Base::__put(_impl, static_cast<_NodeType*>(_cur._node->__unhook()));
    }
    _cur = _next;
    ++_next;
  }
  _impl._header._size -= _removed;
  return _removed;
}

template <typename T, typename Allocator> template <typename UnaryPredicate>
auto list<T, Allocator>::remove_if(UnaryPredicate p)
    -> typename list<T, Allocator>::size_type {
  size_type _removed = 0;
  iterator _cur = iterator(_impl._header._next);
  iterator _next = iterator(_cur._node->_next);
  for (size_type _n = 0; _n < _impl._header._size; ++_n) {
    if (p(*_cur)) {
      ++_removed;
      _Base::__put(_impl, static_cast<_NodeType*>(_cur._node->__unhook()));
    }
    _cur = _next;
    ++_next;
  }
  _impl._header._size -= _removed;
  return _removed;
}

template <typename T, typename Allocator>
void list<T, Allocator>::reverse() noexcept {
  _NodeBase* _cur = _impl._header._next,* _tmp;
  for (size_type _n = 0, _cnt = _impl._header._size + 1; _n < _cnt; ++_n) {
    _tmp = _cur->_next;
    _cur->_next = _cur->_prev;
    _cur = _cur->_prev = _tmp;
  }
}

template <typename T, typename Allocator>
auto list<T, Allocator>::unique() -> typename list<T, Allocator>::size_type {
  size_type _removed = 0;
  iterator _cur = iterator(_impl._header._next);
  iterator _next = iterator(_cur._node->_next);
  for (size_type _n = 1; _n < _impl._header._size; ++_n) {
    if (*_cur == *_next) {
      ++_removed;
      _Base::__put(_impl, static_cast<_NodeType*>(_cur._node->__unhook()));
    }
    _cur = _next;
    ++_next;
  }
  _impl._header._size -= _removed;
  return _removed;
}

template <typename T, typename Allocator> template <typename BinaryPredicate>
auto list<T, Allocator>::unique(BinaryPredicate p)
    -> typename list<T, Allocator>::size_type {
  size_type _removed = 0;
  iterator _cur = iterator(_impl._header._next);
  iterator _next = iterator(_cur._node->_next);
  for (size_type _n = 1; _n < _impl._header._size; ++_n) {
    if (p(*_cur, *_next)) {
      ++_removed;
      _Base::__put(_impl, static_cast<_NodeType*>(_cur._node->__unhook()));
    }
    _cur = _next;
    ++_next;
  }
  _impl._header._size -= _removed;
  return _removed;
}

template <typename T, typename Allocator>
void list<T, Allocator>::sort() {
  if (_impl._header._size < 2) {
    return;
  }
  list _transfer;
  list _pool[48];
  int _end = 0;
  try {
    for (int _cur = 0; _impl._header._size != 0; _cur = 0) {
      _transfer.splice(_transfer.begin(), std::move(*this), this->begin());
      for (; _cur < _end && !_pool[_cur].empty(); ++_cur) {
        _pool[_cur].merge(std::move(_transfer));
        _transfer._impl.__swap(_pool[_cur]._impl);
      }
      _transfer._impl.__swap(_pool[_cur]._impl);
      if (_cur == _end) {
        ++_end;
      }
    }
    for (int _cur = 1; _cur < _end; ++_cur) {
      _pool[_cur].merge(_pool[_cur - 1]);
    }
    _impl.__swap(_pool[_end - 1]._impl);
  } catch (...) {
    this->splice(this->begin(), std::move(_transfer));
    for (int _cur = 0; _cur < _end; ++_cur) {
      this->splice(this->begin(), std::move(_pool[_cur]));
    }
    throw;
  }
}

template <typename T, typename Allocator> template <typename Compare>
void list<T, Allocator>::sort(Compare comp) {
  if (_impl._header._size < 2) {
    return;
  }
  list _transfer;
  list _pool[48];
  int _end = 0;
  try {
    for (int _cur = 0; _impl._header._size != 0; _cur = 0) {
      _transfer.splice(_transfer.begin(), std::move(*this), this->begin());
      for (; _cur < _end && !_pool[_cur].empty(); ++_cur) {
        _pool[_cur].merge(std::move(_transfer), comp);
        _transfer._impl.__swap(_pool[_cur]._impl);
      }
      _transfer._impl.__swap(_pool[_cur]._impl);
      if (_cur == _end) {
        ++_end;
      }
    }
    for (int _cur = 1; _cur < _end; ++_cur) {
      _pool[_cur].merge(std::move(_pool[_cur - 1]), comp);
    }
    _impl.__swap(_pool[_end - 1]._impl);
  } catch (...) {
    this->splice(this->begin(), std::move(_transfer));
    for (int _cur = 0; _cur < _end; ++_cur) {
      this->splice(this->begin(), std::move(_pool[_cur]));
    }
    throw;
  }
}

template <typename T, typename Allocator> template <typename _InputIt>
void list<T, Allocator>::
__assign_range_dispatch(_InputIt _first, _InputIt _last,
                        std::input_iterator_tag) {
  iterator _it = iterator(_impl._header._next);
  for (auto _end = const_iterator(static_cast<_NodeBase*>(&_impl._header));
       _first != _last && _it != _end; ++_first, ++_it) {
    *_it = *_first;
  }
  if (_first == _last) {
    this->erase(_it, const_iterator(static_cast<_NodeBase*>(&_impl._header)));
  } else {
    this->insert(_it, _first, _last);
  }
}

template <typename T, typename Allocator> template <typename _RandomIt>
void list<T, Allocator>::
__assign_range_dispatch(_RandomIt _first, _RandomIt _last,
                        std::random_access_iterator_tag) {
  iterator _it = iterator(_impl._header._next);
  size_type _n = 0, _size = std::distance(_first, _last);
  for (; _n < _impl._header._size && _n < _size; ++_n) {
    *_it = *_first;
    ++_first, ++_it;
  }
  if (_n < _size) {
    this->insert(_it, _first, _last);
  } else {
    this->erase(_it, const_iterator(static_cast<_NodeBase*>(&_impl._header)));
  }
}

template <typename T, typename Allocator> template <typename... Args>
void list<T, Allocator>::__fill_n(_BaseImpl& _impl, const_iterator _next,
                                  const size_type& _count, Args&&... args) {
  for (size_type _n = 0; _n < _count; ++_n) {
    _Base::__get(_impl, std::forward<Args>(args)...)->__hook(_next._node);
    ++_impl._header._size;
  }
}

template <typename T, typename Allocator> template <typename _InputIt>
void list<T, Allocator>::__copy(_BaseImpl& _impl, const_iterator _next,
                                _InputIt _first, _InputIt _last,
                                std::input_iterator_tag) {
  for (; _first != _last; ++_first) {
    _Base::__get(_impl, *_first)->__hook(_next._node);
    ++_impl._header._size;
  }
}

template <typename T, typename Allocator> template <typename _InputIt>
void list<T, Allocator>::__copy_n(_BaseImpl& _impl, const_iterator _next,
                                  _InputIt _first, const size_type& _cnt) {
  for (size_type _n = 0; _n < _cnt; ++_n) {
    _Base::__get(_impl, *_first)->__hook(_next._node);
    ++_impl._header._size;
    ++_first;
  }
}

} // namespace stl
} // namespace ginshio

#endif // GINSHIO_STL__STL_LIST_TC_C
