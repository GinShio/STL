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

#ifndef GINSHIO_STL__CONTAINER_STL_VECTOR_TCC_
#define GINSHIO_STL__CONTAINER_STL_VECTOR_TCC_ 1

namespace ginshio {
namespace stl {

template <typename T, typename Allocator>
auto vector<T, Allocator>::operator=(vector<T, Allocator>&& other) ->
    vector<T, Allocator>& {
  if (this->get_allocator() == other.get_allocator()) {
    _impl.__swap(other._impl);
    return *this;
  }
  this->clear();
  if (_AllocatorTraits::propagate_on_container_move_assignment::value) {
    _Base::__put(_impl);
    _impl.__swap(other._impl);
    static_cast<allocator_type&>(_impl) = ::std::move(
          static_cast<vector<T, Allocator>::allocator_type&>(other._impl));
  } else {
    vector::__insert_range(_impl, const_iterator(_impl._begin), other.begin(),
                           other.end(), other.size(), ::std::false_type());
  }
  return *this;
}

template <typename T, typename Allocator>
auto vector<T, Allocator>::insert(const_iterator pos, size_type count,
                                  const value_type& value) -> iterator {
  const size_type _dist = static_cast<size_type>(pos - this->cbegin());
  if (this->capacity() - this->size() < count) {
    ::ginshio::stl::uninitialized_fill_n(
        vector::__reallocate(_impl, _dist, count), count, value);
  } else if (count != 0) {
    const_iterator _old_end = _impl._end;
    size_type _back_elm_num = static_cast<size_type>(this->cend() - pos);
    if (_back_elm_num <= count) {
      _impl._end = ::ginshio::stl::uninitialized_move(
          pos, _old_end, ::ginshio::stl::uninitialized_fill_n(
              _impl._end, count - _back_elm_num, value));
      count = _back_elm_num;
    } else {
      _impl._end = ::ginshio::stl::uninitialized_move(
          _old_end - count, _old_end, _impl._end);
      _old_end = ::ginshio::stl::move_backward(pos, _old_end - count,
                                               const_cast<iterator>(_old_end));
    }
    ::ginshio::stl::fill_n(const_cast<iterator>(pos), count, value);
  }
  return _impl._begin + _dist;
}

template <typename T, typename Allocator>
auto vector<T, Allocator>::emplace(const_iterator pos, const T& value)
    -> iterator {
  const size_type _dist = static_cast<size_type>(pos - this->cbegin());
  if (_impl._end == _impl._finish) {
    value_type tmp = value;
    ::ginshio::stl::construct(vector::__reallocate(_impl, _dist, 1),
                              ::std::move(tmp));
  } else if (pos == this->cend()) {
    ::ginshio::stl::construct(_impl._end++, value);
  } else {
    value_type tmp = value;
    ::ginshio::stl::construct(_impl._end, ::std::move(*(_impl._end - 1)));
    ::ginshio::stl::move_backward(const_cast<iterator>(pos),
                                  _impl._end - 1, _impl._end);
    *const_cast<iterator>(pos) = ::std::move(tmp);
    ++_impl._end;
  }
  return _impl._begin + _dist;
}

template <typename T, typename Allocator>
template <typename... Args>
auto vector<T, Allocator>::emplace(const_iterator pos, Args&&... args)
    -> iterator {
  const size_type _dist = static_cast<size_type>(pos - this->cbegin());
  if (_impl._end == _impl._finish) {
    ::ginshio::stl::construct(vector::__reallocate(_impl, _dist, 1),
                              ::std::forward<Args>(args)...);
  } else if (pos == this->cend()) {
    ::ginshio::stl::construct(_impl._end++, std::forward<Args>(args)...);
  } else {
    ::ginshio::stl::construct(_impl._end, ::std::move(*(_impl._end - 1)));
    ::ginshio::stl::move_backward(const_cast<iterator>(pos),
                                  _impl._end - 1, _impl._end);
    ::ginshio::stl::construct(const_cast<iterator>(pos),
                              ::std::forward<Args>(args)...);
    ++_impl._end;
  }
  return _impl._begin + _dist;
}

template <typename T, typename Allocator>
template <typename _InputIt>
void vector<T, Allocator>::__assign_range_dispatch(
    _InputIt _first, _InputIt _last, ::std::input_iterator_tag) {
  iterator _cur = _impl._begin;
  for (size_type _sz = this->size(); _sz > 0 && _first != _last; --_sz) {
    *_cur = *_first;
    ++_first, ++_cur;
  }
  if (_first == _last) {
    ::ginshio::stl::destroy(_cur, _impl._end);
    _impl._end = _cur;
    return;
  }
  for (; _first != _last; ++_first) {
    this->emplace_back(*_first);
  }
}

template <typename T, typename Allocator>
template <typename _RandomIt>
void vector<T, Allocator>::__insert_range(_BaseImpl& _impl, iterator _pos,
                                          _RandomIt _first, _RandomIt _last,
                                          const size_type& _len,
                                          std::true_type) {
  _RandomIt _it = _pos;
  if (static_cast<size_type>(_impl._finish - _impl._end) < _len) {
    _RandomIt _old_first = _first;
    size_type _dist_first_begin = _first - _impl._begin;
    _it = vector::__reallocate(_impl, _it - _impl._begin, _len);
    _first = _impl._begin + _dist_first_begin + (_old_first < _pos ? 0 : _len);
    _last = _first + _len + (_pos < _last && _old_first < _pos ? _len : 0);
    if (_first < _it && _it < _last) {
      ginshio::stl::uninitialized_copy(
          _it + _len, _last,
          ginshio::stl::uninitialized_copy(_first, _it,
                                           const_cast<iterator>(_it)));
    } else {
      ginshio::stl::uninitialized_copy(_first, _last,
                                       const_cast<iterator>(_it));
    }
    return;
  }
  _RandomIt _old_end = _impl._end;
  const size_type _back_elm_num = static_cast<size_type>(_impl._end - _it);
  if (_back_elm_num < _len) {
    _impl._end = ginshio::stl::uninitialized_move(
        _it, _old_end, const_cast<iterator>(_it + _len));
  } else {
    _impl._end =
        ginshio::stl::uninitialized_move(_old_end - _len, _old_end, _impl._end);
    ginshio::stl::move_backward(_it, _old_end - _len,
                                const_cast<iterator>(_old_end));
  }
  _first += _first < _it ? 0 : _len;
  _last += _last <= _it ? 0 : _len;
  if (_first < _it && _it < _last) {
    ginshio::stl::copy(
        _it + _len, _last,
        ginshio::stl::copy(_first, _it, const_cast<iterator>(_it)));
  } else {
    ginshio::stl::copy(_first, _last, const_cast<iterator>(_it));
  }
}

template <typename T, typename Allocator>
template <typename _InputIt>
void vector<T, Allocator>::__insert_range(_BaseImpl& _impl, iterator _pos,
                                          _InputIt _first, _InputIt _last,
                                          const size_type& _len,
                                          std::false_type) {
  if (static_cast<size_type>(_impl._finish - _impl._end) < _len) {
    return static_cast<void>(ginshio::stl::uninitialized_copy(
        _first, _last,
        vector::__reallocate(_impl, static_cast<size_type>(_pos - _impl._begin),
                             _len)));
  }
  iterator _old_end = _impl._end;
  const size_type _back_elm_num = static_cast<size_type>(_impl._end - _pos);
  if (_back_elm_num < _len) {
    _impl._end = ginshio::stl::uninitialized_move(_pos, _old_end, _pos + _len);
    ginshio::stl::uninitialized_copy(
        ginshio::stl::copy_n(_first, _len - _back_elm_num, _pos).first, _last,
        _old_end);
  } else {
    _impl._end =
        ginshio::stl::uninitialized_move(_old_end - _len, _old_end, _old_end);
    ginshio::stl::move_backward(_pos, _old_end - _len, _old_end);
    ginshio::stl::copy(_first, _last, _pos);
  }
}

template <typename T, typename Allocator>
template <typename _InputIt>
void vector<T, Allocator>::__insert_range_dispatch(const_iterator _pos,
                                                   _InputIt _first,
                                                   _InputIt _last,
                                                   std::input_iterator_tag) {
  if (_pos == this->cend()) {
    for (; _first != _last; ++_first) {
      this->emplace_back(*_first);
    }
    return;
  }
  vector _vec{_first, _last, this->get_allocator()};
  vector::__insert_range(_impl, _pos,
                         std::move_iterator<iterator>(_vec._impl._begin),
                         std::move_iterator<iterator>(_vec._impl._end),
                         _vec.size(), std::false_type());
}

template <typename T, typename Allocator>
auto vector<T, Allocator>::__reallocate(_BaseImpl& _impl,
                                        const size_type& _dist,
                                        const size_type& _len) -> iterator {
  const size_type _need = _impl._end - _impl._begin + _len;
  const size_type _cap2 = (_impl._finish - _impl._begin) << 1;
  _BaseImpl _new{static_cast<allocator_type>(_impl)};
  _Base::__get(_need > _cap2 ? _need : _cap2, _new);
  if (static_cast<size_type>(_impl._end - _impl._begin) == _dist) {
    ::ginshio::stl::uninitialized_move_n(_impl._begin, _dist, _new._begin);
    _new._end = _new._begin + _need;
  } else {
    ::ginshio::stl::uninitialized_move_n(_impl._begin, _dist, _new._begin);
    _new._end = ::ginshio::stl::uninitialized_move(
        _impl._begin + _dist, _impl._end, _new._begin + _dist + _len);
  }
  _Base::__put(_impl);
  _impl.__swap(_new);
  return _impl._begin + _dist;
}

template <typename T, typename Allocator>
template <typename _InputIt>
void vector<T, Allocator>::__range_init(_InputIt _first, _InputIt _last,
                                        ::std::input_iterator_tag) {
  try {
    for (; _first != _last; ++_first) {
      this->emplace_back(*_first);
    }
  } catch (...) {
    this->clear();
    throw;
  }
}

}  // namespace stl
}  // namespace ginshio

#endif  // GINSHIO_STL__CONTAINER_STL_VECTOR_TCC_
