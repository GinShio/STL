#ifndef GINSHIO_STL__STL_FORWARD_LIST_TCC_
#define GINSHIO_STL__STL_FORWARD_LIST_TCC_ 1

namespace ginshio {
namespace stl {

namespace __container_base {
template <typename _T, typename _Alloc>
void _FwdListBase<_T, _Alloc>::
__clear_aux(_FwdListImpl& _impl,
            _NodeBase* _start_before, const std::size_t& _count) {
  _NodeBase* _cur = _start_before->_next;
  for (std::size_t _n = _count; _n < _impl._header._size; ++_n) {
    _NodeBase* _next = _cur->_next;
    _FwdListBase::__put(_impl, static_cast<_NodeType*>(_cur));
    _cur = _next;
  }
  _start_before->_next = _cur;
  _impl._header._size = _count;
}
} // namespace __container_base

template <typename T, typename Allocator>
void forward_list<T, Allocator>::assign(size_type count,
                                        const value_type& value) {
  auto _prev = const_iterator(static_cast<_NodeBase*>(&_impl._header));
  size_type _n = 0;
  for (auto _it = iterator(_impl._header._next);
       _n < _impl._header._size && _n < count; ++_n) {
    *_it = value;
    _prev = _it._node;
    _it._node = _it._node->_next;
  }
  _n < count ? this->insert_after(_prev, count - _n, value) :
      this->erase_after(_prev, const_iterator(nullptr));
}

template <typename T, typename Allocator>
void forward_list<T, Allocator>::merge(forward_list<T, Allocator>&& other) {
  if (this == &other || this->get_allocator() != other.get_allocator()) {
    return;
  }
  size_type _t_n = 0, _o_n = 0;
  _NodeBase* _t_prev = static_cast<_NodeBase*>(&_impl._header);
  _NodeBase* _t_cur = _impl._header._next;
  _NodeBase* _o_cur = other._impl._header._next;
  try {
    while (_t_n < _impl._header._size && _o_n < other._impl._header._size) {
      if (static_cast<_NodeType*>(_o_cur)->_data <
          static_cast<_NodeType*>(_t_cur)->_data) {
        _NodeBase* _next = _o_cur->_next;
        _o_cur->__hook(_t_prev);
        _t_prev = _o_cur;
        _o_cur = _next;
        ++_o_n;
      } else {
        _t_prev = _t_cur;
        _t_cur = _t_cur->_next;
        ++_t_n;
      }
    }
  } catch (...) {
    _impl._header._size += _o_n;
    other._impl._header._next = _o_cur;
    other._impl._header._size -= _o_n;
    throw;
  }
  if (_o_n < other._impl._header._size) {
    _t_prev->_next = _o_cur;
    _impl._finish = other._impl._finish;
  }
  _impl._header._size += other._impl._header._size;
  other._impl._header.__init_default();
  other._impl._finish = static_cast<_NodeBase*>(&other._impl._header);
}

template <typename T, typename Allocator> template <typename Compare>
void forward_list<T, Allocator>::merge(forward_list<T, Allocator>&& other,
                                       Compare comp) {
  if (this == &other || this->get_allocator() != other.get_allocator()) {
    return;
  }
  size_type _t_n = 0, _o_n = 0;
  _NodeBase* _t_prev = static_cast<_NodeBase*>(&_impl._header);
  _NodeBase* _t_cur = _impl._header._next;
  _NodeBase* _o_cur = other._impl._header._next;
  try {
    while (_t_n < _impl._header._size && _o_n < other._impl._header._size) {
      if (comp(static_cast<_NodeType*>(_o_cur)->_data,
               static_cast<_NodeType*>(_t_cur)->_data)) {
        _NodeBase* _next = _o_cur->_next;
        _o_cur->__hook(_t_prev);
        _t_prev = _o_cur;
        _o_cur = _next;
        ++_o_n;
      } else {
        _t_prev = _t_cur;
        _t_cur = _t_cur->_next;
        ++_t_n;
      }
    }
  } catch (...) {
    _impl._header._size += _o_n;
    other._impl._header._next = _o_cur;
    other._impl._header._size -= _o_n;
    throw;
  }
  if (_o_n < other._impl._header._size) {
    _t_prev->_next = _o_cur;
    _impl._finish = other._impl._finish;
  }
  _impl._header._size += other._impl._header._size;
  other._impl._header.__init_default();
  other._impl._finish = static_cast<_NodeBase*>(&other._impl._header);
}

template <typename T, typename Allocator>
void forward_list<T, Allocator>::
splice_after(const_iterator pos, forward_list&& other,
             const_iterator first, const_iterator last) {
  if (first == last || first._node->_next == last._node ||
      this->get_allocator() != other.get_allocator() ||
      other._impl._header._size == 0) {
    return;
  }
  size_type _len = 0;
  _NodeBase* _end = first._node;
  while (_end && _end->_next != last._node) {
    ++_len;
    _end = _end->_next;
  }
  if (_end == other._impl._finish) {
    other._impl._finish = first._node;
  }
  pos._node->__transfer_after(first._node, _end);
  if (pos._node == _impl._finish) {
    _impl._finish = _end;
  }
  _impl._header._size += _len;
  other._impl._header._size -= _len;
}

template <typename T, typename Allocator>
auto forward_list<T, Allocator>::remove(const value_type& value)
    -> forward_list<T, Allocator>::size_type {
  size_type _removed = 0;
  iterator _prev = iterator(static_cast<_NodeBase*>(&_impl._header));
  iterator _cur = iterator(_impl._header._next), _next;
  for (size_type _n = 0; _n < _impl._header._size; ++_n) {
    _next._node = _cur._node->_next;
    if (*_cur == value) {
      ++_removed;
      _Base::__put(
          _impl, static_cast<_NodeType*>(_cur._node->__unhook(_prev._node)));
    } else {
      _prev._node = _cur._node;
    }
    _cur._node = _next._node;
  }
  _impl._header._size -= _removed;
  _impl._finish = _prev._node;
  return _removed;
}

template <typename T, typename Allocator> template <typename UnaryPredicate>
auto forward_list<T, Allocator>::remove_if(UnaryPredicate p)
    -> forward_list<T, Allocator>::size_type {
  size_type _removed = 0;
  iterator _prev = iterator(static_cast<_NodeBase*>(&_impl._header));
  iterator _cur = iterator(_impl._header._next), _next;
  for (size_type _n = 0; _n < _impl._header._size; ++_n) {
    if (p(*_cur)) {
      ++_removed;
      _next._node = _cur._node->_next;
      _Base::__put(
          _impl, static_cast<_NodeType*>(_cur._node->__unhook(_prev._node)));
      _cur._node = _next._node;
    } else {
      _prev._node = _cur._node;
      _cur._node = _cur._node->_next;
    }
  }
  _impl._header._size -= _removed;
  _impl._finish = _prev._node;
  return _removed;
}

template <typename T, typename Allocator>
void forward_list<T, Allocator>::reverse() noexcept {
  if (_impl._header._size < 2) {
    return;
  }
  _impl._finish = _impl._header._next;
  _NodeBase* _prev = _impl._header._next;
  _NodeBase* _cur = _prev->_next,* _next;
  for (size_type _n = 1; _n < _impl._header._size; ++_n) {
    _next = _cur->_next;
    _cur->_next = _prev;
    _prev = _cur;
    _cur = _next;
  }
  _impl._header._next = _prev;
  _impl._finish->_next = nullptr;
}

template <typename T, typename Allocator>
auto forward_list<T, Allocator>::unique()
    -> forward_list<T, Allocator>::size_type {
  size_type _removed = 0;
  iterator _prev = iterator(static_cast<_NodeBase*>(&_impl._header));
  iterator _cur = iterator(_impl._header._next), _next;
  for (size_type _n = 1; _n < _impl._header._size; ++_n) {
    _next._node = _cur._node->_next;
    if (*_cur == *_next) {
      ++_removed;
      _Base::__put(
          _impl, static_cast<_NodeType*>(_cur._node->__unhook(_prev._node)));
    } else {
      _prev._node = _cur._node;
    }
    _cur._node = _next._node;
    _next._node = _cur._node->_next;
  }
  _impl._header._size -= _removed;
  _impl._finish = _prev._node;
  return _removed;
}

template <typename T, typename Allocator> template <typename BinaryPredicate>
auto forward_list<T, Allocator>::unique(BinaryPredicate p)
    -> forward_list<T, Allocator>::size_type {
  size_type _removed = 0;
  iterator _prev = iterator(static_cast<_NodeBase*>(&_impl._header));
  iterator _cur = iterator(_impl._header._next), _next;
  for (size_type _n = 1; _n < _impl._header._size; ++_n) {
    _next._node = _cur._node->_next;
    if (p(*_cur, *_next)) {
      ++_removed;
      _Base::__put(
          _impl, static_cast<_NodeType*>(_cur._node->__unhook(_prev._node)));
    } else {
      _prev._node = _cur._node;
    }
    _cur._node = _next._node;
    _next._node = _cur._node->_next;
  }
  _impl._header._size -= _removed;
  _impl._finish = _prev._node;
  return _removed;
}

template <typename T, typename Allocator>
void forward_list<T, Allocator>::sort() {
  if (_impl._header._size < 2) {
    return;
  }
  forward_list _transfer;
  forward_list _pool[48];
  int _end = 0;
  try {
    for (int _cur = 0; _impl._header._size != 0; _cur = 0) {
      _transfer.splice_after(_transfer.before_begin(), std::move(*this),
                             this->before_begin());
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
    this->splice_after(this->before_begin(), std::move(_transfer));
    for (int _cur = 0; _cur < _end; ++_cur) {
      this->splice_after(this->before_begin(), std::move(_pool[_cur]));
    }
    throw;
  }
}

template <typename T, typename Allocator> template <typename Compare>
void forward_list<T, Allocator>::sort(Compare comp) {
  if (_impl._header._size < 2) {
    return;
  }
  forward_list _transfer;
  forward_list _pool[48];
  int _end = 0;
  try {
    for (int _cur = 0; _impl._header._size != 0; _cur = 0) {
      _transfer.splice_after(_transfer.before_begin(), std::move(*this),
                             this->before_begin());
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
      _pool[_cur].merge(_pool[_cur - 1], comp);
    }
    _impl.__swap(_pool[_end - 1]._impl);
  } catch (...) {
    this->splice_after(this->before_begin(), std::move(_transfer));
    for (int _cur = 0; _cur < _end; ++_cur) {
      this->splice_after(this->before_begin(), std::move(_pool[_cur]));
    }
    throw;
  }
}

template <typename T, typename Allocator> template <typename _InputIt>
void forward_list<T, Allocator>::__assign_aux(_InputIt _first, _InputIt _last,
                                              std::input_iterator_tag) {
  auto _prev = const_iterator(static_cast<_NodeBase*>(&_impl._header));
  auto _end = const_iterator(nullptr);
  for (auto _it = iterator(_impl._header._next);
       _first != _last && _it != _end; ++_first, ++_it) {
    *_it = *_first;
    _prev._node = _it._node;
  }
  _first == _last ? this->erase_after(_prev, _end) :
      this->insert_after(_prev, _first, _last);
}

template <typename T, typename Allocator> template <typename _RandomIt>
void forward_list<T, Allocator>::__assign_aux(_RandomIt _first, _RandomIt _last,
                                              std::random_access_iterator_tag) {
  auto _prev = const_iterator(static_cast<_NodeBase*>(&_impl._header));
  size_type _n = 0, _size = std::distance(_first, _last);
  for (auto _it = iterator(_impl._header._next);
       _n < _impl._header._size && _n < _size; ++_n) {
    *_it = *_first;
    _prev._node = _it._node;
    ++_first, ++_it;
  }
  _n < _size ? this->insert_after(_prev, _first, _last) :
      this->erase_after(_prev, const_iterator(nullptr));
}

template <typename T, typename Allocator> template <typename... Args>
auto forward_list<T, Allocator>::
__fill_n(_BaseImpl& _impl, const_iterator _prev,
         const size_type& _count, Args&&... args)
    -> forward_list<T, Allocator>::_NodeBase* {
  for (size_type _n = 0; _n < _count; ++_n) {
    _Base::__get(_impl, std::forward<Args>(args)...)->__hook(_prev._node);
    ++_impl._header._size;
    ++_prev;
  }
  return _prev._node;
}

template <typename T, typename Allocator> template <typename _InputIt>
auto forward_list<T, Allocator>::__copy(_BaseImpl& _impl, const_iterator _prev,
                                        _InputIt _first, _InputIt _last,
                                        std::input_iterator_tag)
    -> forward_list<T, Allocator>::_NodeBase* {
  for (; _first != _last; ++_first) {
    _Base::__get(_impl, *_first)->__hook(_prev._node);
    ++_impl._header._size;
    ++_prev;
  }
  return _prev._node;
}

template <typename T, typename Allocator> template <typename _InputIt>
auto forward_list<T, Allocator>::
__copy_n(_BaseImpl& _impl, const_iterator _prev,
         _InputIt _first, const size_type& _cnt)
    -> forward_list<T, Allocator>::_NodeBase* {
  for (size_type _n = 0; _n < _cnt; ++_n) {
    _Base::__get(_impl, *_first)->__hook(_prev._node);
    ++_impl._header._size;
    ++_first, ++_prev;
  }
  return _prev._node;
}

} // namespace stl
} // namespace ginshio

#endif // GINSHIO_STL__STL_FORWARD_LIST_TCC_
