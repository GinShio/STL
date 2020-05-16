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


#ifndef GINSHIO_STL__STL_DEQUE_TCC_
#define GINSHIO_STL__STL_DEQUE_TCC_ 1

namespace ginshio {
namespace stl {

namespace __container_base {
template <typename _T, typename _Alloc>
void _DequeBase<_T, _Alloc>::
__get_chunk(_DataAllocType& _a, _ChunkPtr _first, const std::size_t& _cnt) {
  _ChunkPtr _cur = _first;
  try {
    for (std::size_t _n = 0; _n < _cnt; ++_n) {
      *_cur = _DataAllocTraits::
          allocate(_a, __deque_chunk_capacity<_ValueType>());
      ++_cur;
    }
  } catch (...) {
    _DequeBase::__put_chunk(_a, _first, _cur - _first);
    throw;
  }
}

template <typename _T, typename _Alloc>
void _DequeBase<_T, _Alloc>::
__put_chunk(_DataAllocType& _a, _ChunkPtr _first, const std::size_t& _cnt) {
  for (std::size_t _n = 0; _n < _cnt; ++_n) {
    _DataAllocTraits::
        deallocate(_a, *_first, __deque_chunk_capacity<_ValueType>());
    ++_first;
  }
}

template <typename _T, typename _Alloc>
void _DequeBase<_T, _Alloc>::
__reallocate_map(_DequeImpl& _impl,
                 const std::size_t& _len, const bool& _is_front) {
  const std::size_t _dist = _impl._end._node - _impl._begin._node + 1;
  const std::size_t _need = 1 +
      (_len - (_impl._begin._cur - _impl._begin._first) - 1) /
      __deque_chunk_capacity<_ValueType>();
  std::size_t _new_sz = _need + _dist;
  _ChunkPtr _new_begin = nullptr;
  if (_impl._chunk_size < (_new_sz << 1)) {
    const std::size_t _new_chunk_sz = _impl._chunk_size +
        (_impl._chunk_size > _new_sz ? _impl._chunk_size : _new_sz) + 2;
    _ChunkPtr _new_chunks = _DequeBase::__get_map(_impl, _new_chunk_sz);
    _new_begin = _new_chunks +
        (((_new_chunk_sz - _new_sz) >> 1) + (_is_front ? _need : 0));
    std::memcpy(_new_begin, _impl._begin._node, sizeof(_ChunkPtr) * _dist);
    _DequeBase::__put_map(_impl, _impl._set, _impl._chunk_size);
    _impl._chunk_size = _new_chunk_sz;
    _impl._set = _new_chunks;
  } else {
    _new_begin = _impl._set + ((_impl._chunk_size - _new_sz) >> 1) +
        (_is_front ? _need : 0);
    std::memmove(_new_begin, _impl._begin._node, sizeof(_ChunkPtr) * _dist);
  }
  _impl._end._node = _new_begin + _dist - 1;
  _impl._begin._node = _new_begin;
  _DequeBase::
      __get_chunk(_impl, _is_front ?
                  _impl._begin._node - _need : _impl._end._node + 1, _need);
}

template <typename _T, typename _Alloc>
void _DequeBase<_T, _Alloc>::__deallocate_chunk_aux(_DequeImpl& _impl,
                                                    std::false_type) {
  if (_impl._begin._node == _impl._end._node) {
    ginshio::stl::destroy(_impl._begin._cur, _impl._end._cur);
    return _DataAllocTraits::deallocate(_impl, *_impl._begin._node,
                                        __deque_chunk_capacity<_ValueType>());
  } else {
    ginshio::stl::destroy(_impl._begin._cur, _impl._begin._last);
    ginshio::stl::destroy(_impl._end._first, _impl._end._cur);
    _DataAllocTraits::deallocate(_impl, *_impl._begin._node,
                                 __deque_chunk_capacity<_ValueType>());
    _DataAllocTraits::deallocate(_impl, *_impl._end._node,
                                 __deque_chunk_capacity<_ValueType>());
  }
  for (_ChunkPtr _cur = _impl._begin._node + 1;
       _cur < _impl._end._node; ++_cur) {
    ginshio::stl::destroy(*_cur, *_cur + __deque_chunk_capacity<_ValueType>());
    _DataAllocTraits::
        deallocate(_impl, *_cur, __deque_chunk_capacity<_ValueType>());
  }
}
} // namespace __container_base



template <typename T, typename Allocator>
auto deque<T, Allocator>::erase(const_iterator first, const_iterator last)
    -> typename deque<T, Allocator>::iterator {
  if (first == last) {
    return iterator(last._cur, last._node);
  }
  if (first == _impl._begin) {
    deque::__erase_front(_impl, iterator(last._cur, last._node));
    return _impl._begin;
  } else if (last == _impl._end) {
    deque::__erase_back(_impl, iterator(first._cur, first._node));
    return _impl._end;
  }
  const size_type _dist = static_cast<size_type>(first - _impl._begin);
  if (deque::__is_front(_dist, _impl._end - _impl._begin - (last - first))) {
    iterator _old_begin = _impl._begin;
    _impl._begin = ginshio::stl::
        move_backward(const_iterator(_old_begin), first,
                      iterator(last._cur, last._node));
    ginshio::stl::destroy(_old_begin, _impl._begin);
    _Base::__put_chunk(_impl, _old_begin._node,
                       _impl._begin._node - _old_begin._node);
    return _impl._begin + _dist;
  }
  iterator _old_end = _impl._end;
  _impl._end = ginshio::stl::
      move(last, const_iterator(_old_end), iterator(first._cur, first._node));
  ginshio::stl::destroy(_impl._end, _old_end);
  _Base::__put_chunk(_impl, _impl._end._node + 1,
                     _old_end._node - _impl._end._node);
  return iterator(first._cur, first._node);
}

template <typename T, typename Allocator>
void deque<T, Allocator>::resize(size_type count) {
  if (this->size() < count) {
    ginshio::stl::
        __check_length_error("deque::resize", count, _Base::__max_size());
    iterator _it = this->__insert_aux(_impl._end, count);
    for (int i = 0, cnt = count - this->size(); i < cnt; ++i) {
      ginshio::stl::construct(_it._cur);
    }
  } else if (this->size() > count) {
    deque::__erase_back(_impl, _impl._begin + count);
  }
}

template <typename T, typename Allocator> template <typename _InputIt>
void deque<T, Allocator>::
__assign_aux(_InputIt _first, _InputIt _last, std::input_iterator_tag) {
  iterator _cur = _impl._begin;
  for (size_type _n = 0, _sz = this->size(); _n < _sz && _first != _last;) {
    *_cur = *_first;
    ++_first, ++_cur;
  }
  if (_first == _last) {
    return deque::__erase_back(_impl, _cur);
  }
  for (; _first != _last; ++_first) {
    this->emplace_back(*_first);
  }
}

template <typename T, typename Allocator> template <typename _InputIt>
void deque<T, Allocator>::
__insert_aux(iterator _pos, _InputIt _first, _InputIt _last,
             std::input_iterator_tag) {
  if (_pos == _impl._end) {
    for (; _first != _last; ++_first) {
      this->emplace_back(*_first);
    }
    return;
  }
  deque _deq{_first, _last, this->get_allocator()};
  ginshio::stl::
      uninitialized_copy_n(std::move_iterator<_InputIt>(_first), _deq.size(),
                           this->__insert_aux(_pos, _deq.size()));
}

template <typename T, typename Allocator>
auto deque<T, Allocator>::__insert_aux(iterator _pos, const size_type& _len)
    -> typename deque<T, Allocator>::iterator {
  if (_pos._cur == _impl._begin._cur) {
    if (static_cast<size_type>(_impl._begin._cur -
                               _impl._begin._first) < _len) {
      _Base::__reallocate_map(_impl, _len, true);
    }
    return _impl._begin += -_len;
  } else if (_pos._cur == _impl._end._cur) {
    if (static_cast<size_type>(_impl._end._last - _impl._end._cur) < _len) {
      _Base::__reallocate_map(_impl, _len, false);
      _pos._node = _impl._end._node;
    }
    _impl._end += _len;
    return _pos;
  }
  return deque::__is_front(static_cast<size_type>(_pos - _impl._begin),
                           this->size() - _len) ?
      deque::__exten_cap_front(_impl, _pos, _len) :
      deque::__exten_cap_back(_impl, _pos, _len);
}

template <typename T, typename Allocator>
auto deque<T, Allocator>::
__exten_cap_front(_BaseImpl& _impl, iterator _pos, const size_type& _len)
    -> typename deque<T, Allocator>::iterator {
  const size_type _front_elm_num = static_cast<size_type>(_pos - _impl._begin);
  if (static_cast<size_type>(_impl._begin._cur -
                             _impl._begin._first) < _len) {
    const size_type _tmp =
        static_cast<size_type>(_pos._node - _impl._begin._node);
    _Base::__reallocate_map(_impl, _len, true);
    _pos._node = _impl._begin._node + _tmp;
  }
  iterator _old_begin = _impl._begin;
  iterator _new_begin = _impl._begin - _len;
  try {
    if (_front_elm_num <= _len) {
      ginshio::stl::uninitialized_move(_old_begin, _pos, _new_begin);
      ginshio::stl::destroy(_old_begin, _pos);
    } else {
      ginshio::stl::uninitialized_move_n(_old_begin, _len, _new_begin);
      ginshio::stl::destroy(ginshio::stl::
                            uninitialized_move(_old_begin + _len,
                                               _pos, _old_begin), _pos);
    }
    _impl._begin = _new_begin;
  } catch (...) {
    _Base::__put_chunk(_impl, _new_begin._node,
                       _impl._begin._node - _new_begin._node);
    throw;
  }
  return _impl._begin + _front_elm_num;
}

template <typename T, typename Allocator>
auto deque<T, Allocator>::
__exten_cap_back(_BaseImpl& _impl, iterator _pos, const size_type& _len)
    -> typename deque<T, Allocator>::iterator {
  const size_type _back_elm_num = static_cast<size_type>(_impl._end - _pos);
  if (static_cast<size_type>(_impl._end._last - _impl._end._cur) < _len) {
    const size_type _tmp =
        static_cast<size_type>(_pos._node - _impl._begin._node);
    _Base::__reallocate_map(_impl, _len, false);
    _pos._node = _impl._begin._node + _tmp;
  }
  iterator _old_end = _impl._end;
  iterator _new_end = _impl._end + _len;
  try {
    if (_back_elm_num <= _len) {
      _impl._end = ginshio::stl::
          uninitialized_move(_pos, _old_end, _pos + _len);
      ginshio::stl::destroy(_pos, _old_end);
    } else {
      _impl._end = ginshio::stl::
          uninitialized_move(_old_end - _len, _old_end, _old_end);
      ginshio::stl::destroy(_pos, ginshio::stl::
                            move_backward(_pos, _old_end - _len, _old_end));
    }
  } catch (...) {
    _Base::__put_chunk(_impl, _impl._end._node + 1,
                       _new_end._node - _impl._end._node);
    throw;
  }
  return _pos;
}

} // namespace stl
} // namespace ginshio

#endif // GINSHIO_STL__STL_DEQUE_TCC_
