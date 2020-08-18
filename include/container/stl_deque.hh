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


#ifndef GINSHIO_STL__STL_DEQUE_HH_
#define GINSHIO_STL__STL_DEQUE_HH_ 1

namespace ginshio {
namespace stl {

namespace __container_base {
///////////////////////// deque constexpr /////////////////////////
constexpr std::size_t _DEUQE_DEFAULT_CHUNK_SIZE = 4096;
template <typename _T> constexpr std::size_t __deque_chunk_size() noexcept {
  return sizeof(_T) <= 128 ? _DEUQE_DEFAULT_CHUNK_SIZE : sizeof(_T) * 16;
}
template <typename _T> constexpr std::size_t __deque_chunk_capacity() noexcept {
  return sizeof(_T) <= 128 ? _DEUQE_DEFAULT_CHUNK_SIZE / sizeof(_T) : 16;
}


///////////////////////// deque iterator /////////////////////////
template <typename _T, typename _Ptr, typename _Ref>
struct _DequeIterator {
  /////////////// traits type ///////////////
 public:
  using difference_type = std::ptrdiff_t;
  using value_type = _T;
  using pointer = _Ptr;
  using reference = _Ref;
  using iterator_category = std::random_access_iterator_tag;

  /////////////// iterator type ///////////////
 public:
  using _Iterator = _DequeIterator<_T, _T*, _T&>;
  using _ConstIterator = _DequeIterator<_T, const _T*, const _T&>;
  using _SelfIterator = _DequeIterator<_T, _Ptr, _Ref>;
  using _OtherIterator =
      typename std::conditional<std::is_same<_SelfIterator, _Iterator>::value,
                                _ConstIterator, _Iterator>::type;

  /////////////// element type ///////////////
 public:
  using _ValuePtr = _T*;
  using _ChunkPtr = _ValuePtr*;

  /////////////// data member ///////////////
 public:
  _ValuePtr _cur;
  _ValuePtr _first;
  _ValuePtr _last;
  _ChunkPtr _node;

  /////////////// constructor ///////////////
 public:
  _DequeIterator() = default;
  explicit _DequeIterator(_ChunkPtr _c) {
    this->assign(*_c, _c);
  }
  explicit _DequeIterator(_ValuePtr _p, _ChunkPtr _c) {
    this->assign(_p, _c);
  }
  _DequeIterator(const _SelfIterator&) = default;
  template <typename _Iter, typename = typename
            std::enable_if<std::is_same<_SelfIterator, _ConstIterator>::value &&
                           std::is_same<_OtherIterator, _Iter>::value>::type*>
  _DequeIterator(const _Iter& _it) : _cur(_it._cur), _first(_it._first),
                                     _last(_it._last), _node(_it._node) {}

  /////////////// assign ///////////////
 public:
  _SelfIterator& operator=(const _SelfIterator&) = default;
  template <typename _Iter, typename = typename
            std::enable_if<std::is_same<_SelfIterator, _ConstIterator>::value &&
                           std::is_same<_OtherIterator, _Iter>::value>::type*>
  _SelfIterator& operator=(const _Iter& _it) {
    this->assign(_it._cur, _it._node);
    return *this;
  }
  void assign(_ValuePtr _p, _ChunkPtr _c) {
    _cur = _p;
    _first = *_c;
    _last = *_c + __deque_chunk_capacity<value_type>();
    _node = _c;
  }

  /////////////// member function ///////////////
 public:
  constexpr _SelfIterator& base() const {
    return const_cast<_SelfIterator&>(*this);
  }
  void __set_node(const difference_type& _n) {
    _node += _n;
    _first = *_node;
    _last = *_node + __deque_chunk_capacity<value_type>();
  }

  /////////////// member access operators ///////////////
 public:
  constexpr reference operator*() const noexcept { return *_cur; }
  constexpr pointer operator->() const { return _cur; }
  constexpr reference operator[](difference_type _n) const {
    return *(this->operator+(_n));
  }

  /////////////// arithmetic operators ///////////////
 public:
  _SelfIterator& operator++() noexcept {
    ++_cur;
    if (_cur == _last) {
      this->__set_node(1);
      _cur = _first;
    }
    return *this;
  }
  _SelfIterator operator++(int) noexcept {
    _SelfIterator _tmp = *this;
    this->operator++();
    return _tmp;
  }
  _SelfIterator& operator+=(const difference_type& _n) noexcept {
    constexpr difference_type _chunk_cap = __deque_chunk_capacity<value_type>();
    const difference_type _offset = _n + (_cur - _first);
    if (0 <= _offset && _offset < _chunk_cap) {
      _cur += _n;
    } else {
      const difference_type _tmp =
          _offset < 0 ? -((-_offset - 1) / _chunk_cap) - 1 :
          _offset / _chunk_cap;
      this->__set_node(_tmp);
      _cur = _first + (_offset - _tmp * _chunk_cap);
    }
    return *this;
  }
  _SelfIterator operator+(const difference_type& _n) const noexcept {
    _SelfIterator _tmp = *this;
    return _tmp += _n;
  }
  _SelfIterator& operator--() noexcept {
    if (_cur == _first) {
      this->__set_node(-1);
      _cur = _last;
    }
    --_cur;
    return *this;
  }
  _SelfIterator operator--(int) noexcept {
    _SelfIterator _tmp = *this;
    this->operator--();
    return _tmp;
  }
  _SelfIterator& operator-=(const difference_type& _n) noexcept {
    return this->operator+=(-_n);
  }
  _SelfIterator operator-(const difference_type& _n) const noexcept {
    _SelfIterator _tmp = *this;
    return _tmp += -_n;
  }
};
///////////////////////// iterator comparison operator /////////////////////////
template <typename _T, typename _Ptr, typename _Ref>
constexpr bool operator==(const _DequeIterator<_T, _Ptr, _Ref>& _lit,
                          const _DequeIterator<_T, _Ptr, _Ref>& _rit) noexcept {
  return _lit._cur == _rit._cur;
}
template <typename _T, typename _PtrL, typename _RefL,
          typename _PtrR, typename _RefR>
constexpr bool operator==(
    const _DequeIterator<_T, _PtrL, _RefL>& _lit,
    const _DequeIterator<_T, _PtrR, _RefR>& _rit) noexcept {
  return _lit._cur == _rit._cur;
}
template <typename _T, typename _Ptr, typename _Ref>
constexpr bool operator!=(const _DequeIterator<_T, _Ptr, _Ref>& _lit,
                          const _DequeIterator<_T, _Ptr, _Ref>& _rit) noexcept {
  return _lit._cur != _rit._cur;
}
template <typename _T, typename _PtrL, typename _RefL,
          typename _PtrR, typename _RefR>
constexpr bool operator!=(
    const _DequeIterator<_T, _PtrL, _RefL>& _lit,
    const _DequeIterator<_T, _PtrR, _RefR>& _rit) noexcept {
  return _lit._cur != _rit._cur;
}
template <typename _T, typename _Ptr, typename _Ref>
constexpr bool operator<(const _DequeIterator<_T, _Ptr, _Ref>& _lit,
                         const _DequeIterator<_T, _Ptr, _Ref>& _rit) noexcept {
  return _lit._node == _rit._node ?
      _lit._cur < _rit._cur : _lit._node < _rit._node;
}
template <typename _T, typename _PtrL, typename _RefL,
          typename _PtrR, typename _RefR>
constexpr bool operator<(
    const _DequeIterator<_T, _PtrL, _RefL>& _lit,
    const _DequeIterator<_T, _PtrR, _RefR>& _rit) noexcept {
  return _lit._node == _rit._node ?
      _lit._cur < _rit._cur : _lit._node < _rit._node;
}
template <typename _T, typename _Ptr, typename _Ref>
constexpr bool operator>(const _DequeIterator<_T, _Ptr, _Ref>& _lit,
                         const _DequeIterator<_T, _Ptr, _Ref>& _rit) noexcept {
  return _lit._node == _rit._node ?
      _lit._cur > _rit._cur : _lit._node > _rit._node;
}
template <typename _T, typename _PtrL, typename _RefL,
          typename _PtrR, typename _RefR>
constexpr bool operator>(
    const _DequeIterator<_T, _PtrL, _RefL>& _lit,
    const _DequeIterator<_T, _PtrR, _RefR>& _rit) noexcept {
  return _lit._node == _rit._node ?
      _lit._cur > _rit._cur : _lit._node > _rit._node;
}
template <typename _T, typename _Ptr, typename _Ref>
constexpr bool operator<=(const _DequeIterator<_T, _Ptr, _Ref>& _lit,
                          const _DequeIterator<_T, _Ptr, _Ref>& _rit) noexcept {
  return !operator<(_rit, _lit);
}
template <typename _T, typename _PtrL, typename _RefL,
          typename _PtrR, typename _RefR>
constexpr bool operator<=(
    const _DequeIterator<_T, _PtrL, _RefL>& _lit,
    const _DequeIterator<_T, _PtrR, _RefR>& _rit) noexcept {
  return !operator<(_rit, _lit);
}
template <typename _T, typename _Ptr, typename _Ref>
constexpr bool operator>=(const _DequeIterator<_T, _Ptr, _Ref>& _lit,
                          const _DequeIterator<_T, _Ptr, _Ref>& _rit) noexcept {
  return !operator<(_lit, _rit);
}
template <typename _T, typename _PtrL, typename _RefL,
          typename _PtrR, typename _RefR>
constexpr bool operator>=(
    const _DequeIterator<_T, _PtrL, _RefL>& _lit,
    const _DequeIterator<_T, _PtrR, _RefR>& _rit) noexcept {
  return !operator<(_lit, _rit);
}
///////////////////////// iterator arithmetic operator /////////////////////////
template <typename _T, typename _Ptr, typename _Ref>
constexpr auto operator+(
    const typename _DequeIterator<_T, _Ptr, _Ref>::difference_type _n,
    const _DequeIterator<_T, _Ptr, _Ref>& _it) noexcept
    -> _DequeIterator<_T, _Ptr, _Ref> {
  return _it.operator+(_n);
}
template <typename _T, typename _Ptr, typename _Ref>
constexpr auto operator-(
    const typename _DequeIterator<_T, _Ptr, _Ref>::difference_type _n,
    const _DequeIterator<_T, _Ptr, _Ref>& _it) noexcept
    -> _DequeIterator<_T, _Ptr, _Ref> {
  return _it.operator-(_n);
}
template <typename _T, typename _Ptr, typename _Ref>
constexpr auto operator-(const _DequeIterator<_T, _Ptr, _Ref>& _lit,
                         const _DequeIterator<_T, _Ptr, _Ref>& _rit) noexcept
    -> typename _DequeIterator<_T, _Ptr, _Ref>::difference_type {
  return _lit._node == _rit._node ? _lit._cur - _rit._cur :
      (__deque_chunk_capacity<_T>() * (_lit._node - _rit._node - 1) +
       (_lit._cur - _lit._first) + (_rit._last - _rit._cur));
}
template <typename _T, typename _PtrL, typename _RefL,
          typename _PtrR, typename _RefR>
constexpr auto operator-(const _DequeIterator<_T, _PtrL, _RefL>& _lit,
                         const _DequeIterator<_T, _PtrR, _RefR>& _rit) noexcept
    -> typename _DequeIterator<_T, _PtrL, _RefR>::difference_type {
  return _lit._node == _rit._node ? _lit._cur - _rit._cur :
      (__deque_chunk_capacity<_T>() * (_lit._node - _rit._node - 1) +
       (_lit._cur - _lit._first) + (_rit._last - _rit._cur));
}


///////////////////////// deque base /////////////////////////
template <typename _T, typename _Allocator>
struct _DequeBase {
  /////////////// member type ///////////////
 protected:
  using _ValueType = _T;
  using _ValuePtr = _ValueType*;
  using _DataAllocType = _Allocator;
  using _DataAllocTraits = std::allocator_traits<_DataAllocType>;
  using _ChunkPtr = _ValuePtr*;
  using _ChunkAllocType =
      typename _DataAllocTraits::template rebind_alloc<_ValuePtr>;
  using _ChunkAllocTraits =
      typename _DataAllocTraits::template rebind_traits<_ValuePtr>;
  using _Iterator = _DequeIterator<_ValueType, _ValueType*, _ValueType&>;
  using _ConstIterator =
      _DequeIterator<_ValueType, const _ValueType*, const _ValueType&>;

  /////////////// implement ///////////////
 protected:
  struct _DequeImpl : public _DataAllocType {
    std::size_t _chunk_size;
    _ChunkPtr _set;
    _Iterator _begin;
    _Iterator _end;
    _DequeImpl() = default;
    _DequeImpl(const _DataAllocType& _alloc) : _DataAllocType(_alloc) {}
    void __swap(_DequeImpl& _impl) {
      std::swap(_chunk_size, _impl._chunk_size);
      std::swap(_set, _impl._set);
      std::swap(_begin, _impl._begin);
      std::swap(_end, _impl._end);
    }
  };

  /////////////// data member ///////////////
 protected:
  _DequeImpl _impl;

  /////////////// constructor ///////////////
 public:
  _DequeBase() { _DequeBase::__allocate(_impl, 0); }
  _DequeBase(const _DataAllocType& _alloc) : _impl(_alloc) {
    _DequeBase::__allocate(_impl, 0);
  }
  _DequeBase(const std::size_t& _elm_num,
             const _DataAllocType& _alloc) : _impl(_alloc) {
    ginshio::stl::
        __check_length_error("deque", _elm_num, _DequeBase::__max_size());
    _DequeBase::__allocate(_impl, _elm_num);
  }
  _DequeBase(_DequeBase&& _other) noexcept : _impl(std::move(_other._impl)) {
    _other._impl = _DequeBase::__allocate(_impl, 0);
  }
  ~_DequeBase() noexcept {
    if (_impl._set) {
      _DequeBase::__deallocate(_impl);
    }
  }

  /////////////// utils function ///////////////
 protected:
  static void __swap_allocator(_DataAllocType& _a, _DataAllocType& _b) {
    return std::swap(_a, _b);
  }
  static constexpr std::size_t __max_size() noexcept {
    return
        static_cast<std::size_t>(std::numeric_limits<std::ptrdiff_t>::max()) /
        sizeof(_ValueType);
  }

  /////////////// growth ///////////////
 protected:
  static void __get_chunk(_DataAllocType& _a,
                          _ChunkPtr _first, const std::size_t& _cnt);
  static void __put_chunk(_DataAllocType& _a,
                          _ChunkPtr _first, const std::size_t& _cnt);
  static _ChunkPtr __get_map(_ChunkAllocType _a, const std::size_t& _cnt) {
    return _ChunkAllocTraits::allocate(_a, _cnt);
  }
  static void __put_map(_ChunkAllocType _a,
                        _ChunkPtr _ptr, const std::size_t& _cnt) {
    _ChunkAllocTraits::deallocate(_a, _ptr, _cnt);
  }
  static void __reallocate_map(_DequeImpl& _impl,
                               const std::size_t& _len, const bool& _is_front);

  /////////////// allocate ///////////////
 protected:
  static void __allocate(_DequeImpl& _impl, const std::size_t& _elm_num) {
    std::size_t _size = _elm_num / __deque_chunk_capacity<_ValueType>() + 3;
    _impl._set = _DequeBase::__get_map(_impl, _size);
    _impl._chunk_size = _size;
    try {
      _DequeBase::__get_chunk(_impl, _impl._set + 1, _size - 2);
    } catch (...) {
      _DequeBase::__put_map(_impl, _impl._set, _impl._chunk_size);
      _impl._chunk_size = 0;
      _impl._set = nullptr;
      throw;
    }
    _impl._begin = _impl._end = _Iterator(_impl._set + 1);
  }

  /////////////// deallocate ///////////////
 protected:
  static void __deallocate(_DequeImpl& _impl) {
    _DequeBase::__deallocate_chunk(_impl);
    _DequeBase::__put_map(_impl, _impl._set, _impl._chunk_size);
    _impl._chunk_size = 0;
    _impl._set = nullptr;
  }
  static void __deallocate_chunk(_DequeImpl& _impl) {
    using _BoolType = typename
        std::conditional<std::is_trivially_destructible<_ValueType>::value,
                         std::true_type, std::false_type>::type;
    _DequeBase::__deallocate_chunk_aux(_impl, _BoolType());
  }
  static void __deallocate_chunk_aux(_DequeImpl& _impl, std::true_type) {
    _DequeBase::__put_chunk(_impl, _impl._begin._node,
                            _impl._end._node - _impl._begin._node + 1);
  }
  static void __deallocate_chunk_aux(_DequeImpl& _impl, std::false_type);
};

} // namespace __container_base



///////////////////////// deque /////////////////////////
template <typename T, typename Allocator = std::allocator<T>>
class deque : protected __container_base::_DequeBase<T, Allocator> {
  /////////////// private type ///////////////
 private:
  using _Base = __container_base::_DequeBase<T, Allocator>;
  using _BaseImpl = typename _Base::_DequeImpl;
  using _DataAllocTraits = typename _Base::_DataAllocTraits;
  using _ChunkPtr = typename _Base::_ChunkPtr;
  using _ChunkAllocType = typename _Base::_ChunkAllocType;
  using _ChunkAllocTraits = typename _Base::_ChunkAllocTraits;
  /////////////// member type ///////////////
 public:
  using value_type = T;
  using allocator_type = typename _DataAllocTraits::allocator_type;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = typename _DataAllocTraits::pointer;
  using const_pointer = typename _DataAllocTraits::const_pointer;
  using iterator = typename _Base::_Iterator;
  using const_iterator = typename _Base::_ConstIterator;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  /////////////// data member ///////////////
 private:
  using _Base::_impl;

  /////////////// constructor ///////////////
 public:
  deque() = default;
  explicit deque(const Allocator& alloc) : _Base(alloc) {}
  explicit deque(size_type count, const T& value,
                 const Allocator& alloc = allocator_type()) :
      _Base(count, alloc) {
    try {
      _impl._end = ginshio::stl::
          uninitialized_fill_n(_impl._begin, count, value);
    } catch (...) {
      _Base::__put_chunk(_impl, _impl._begin._node + 1, _impl._chunk_size - 3);
      throw;
    }
  }
  explicit deque(size_type count, const Allocator& alloc = allocator_type()) :
      _Base(count, alloc) {
    try {
      for (size_type _n = 0; _n < count; ++_n) {
        ginshio::stl::construct(_impl._end._cur);
        ++_impl._end;
      }
    } catch (...) {
      ginshio::stl::destroy(_impl._begin, _impl._end);
      _Base::__put_chunk(_impl, _impl._begin._node + 1, _impl._chunk_size - 3);
      _impl._end = _impl._begin;
      throw;
    }
  }
  template <typename InputIt, typename = typename
            std::enable_if<std::is_base_of<
                             std::input_iterator_tag, typename
                             std::iterator_traits<InputIt>::iterator_category>::
                           value>::type*>
  deque(InputIt first, InputIt last,
        const allocator_type& alloc = allocator_type()) : _Base(alloc) {
    try {
      _impl._end = ginshio::stl::uninitialized_copy(first, last, _impl._begin);
    } catch (...) {
      _Base::__put_chunk(_impl, _impl._begin._node + 1, _impl._chunk_size - 3);
      throw;
    }
  }
  deque(const deque& other) :
      _Base(other.size(), _DataAllocTraits::
            select_on_container_copy_construction(other.get_allocator())) {
    try {
      _impl._end = ginshio::stl::
          uninitialized_copy(other._impl._begin, other._impl._end,
                             _impl._begin);
    } catch (...) {
      _Base::__put_chunk(_impl, _impl._begin._node + 1,
                         _impl._chunk_size - 3);
      throw;
    }
  }
  deque(const deque& other, const allocator_type& alloc) :
      _Base(other.size(), alloc) {
    try {
      _impl._end = ginshio::stl::
          uninitialized_copy(other._impl._begin, other._impl._end,
                             _impl._begin);
    } catch (...) {
      _Base::__put_chunk(_impl, _impl._begin._node + 1,
                         _impl._chunk_size - 3);
      throw;
    }
  }
  deque(deque&& other) noexcept = default;
  deque(deque&& other, const allocator_type& alloc) : _Base(alloc) {
    if (other.get_allocator() == alloc) {
      _impl.__swap(other._impl);
      return;
    }
    if (0 < other.size()) {
      _Base::__reallocate_map(_impl, other.size(), true);
    }
    iterator _new_begin = _impl._begin - other.size();
    try {
      ginshio::stl::
          uninitialized_move(other.begin(), other.end(), _new_begin);
    } catch (...) {
      _Base::__put_chunk(_impl, _new_begin._node,
                         _impl._begin._node - _new_begin._node);
      throw;
    }
    _impl._begin = _new_begin;
  }
  deque(std::initializer_list<value_type> ilist,
        const allocator_type& alloc) : _Base(ilist.size(), alloc) {
    try {
      _impl._end = ginshio::stl::
          uninitialized_copy(ilist.begin(), ilist.end(), _impl._begin);
    } catch (...) {
      _Base::__put_chunk(_impl, _impl._begin._node + 1,
                         _impl._chunk_size - 3);
      throw;
    }
  }
  ~deque() noexcept = default;

  /////////////// member function ///////////////
 public:
  void assign(size_type count, const value_type& value) {
    iterator _cur = _impl._begin;
    size_type _n = 0, _size = static_cast<size_type>(_impl._begin - _impl._end);
    for (; _n < _size && _n < count; ++_n) {
      *_cur = value;
    }
    _n < count ?
         this->insert(_cur, count - _n, value) : this->erase(_cur, _impl._end);
  }
  template <typename InputIt, typename = typename
            std::enable_if<std::is_base_of<
                             std::input_iterator_tag, typename
                             std::iterator_traits<InputIt>::iterator_category>::
                           value>::type*>
  void assign(InputIt first, InputIt last) {
    using _Category = typename std::iterator_traits<InputIt>::iterator_category;
    this->__assign_aux(first, last, _Category());
  }
  void assign(std::initializer_list<value_type> ilist) {
    this->__assign_aux(ilist.begin(), ilist.end(),
                       std::random_access_iterator_tag());
  }
  deque& operator=(const deque& other) {
    if (this == &other) {
      return *this;
    }
    if (_DataAllocTraits::propagate_on_container_copy_assignment::value) {
      if (other.get_allocator() != this->get_allocator()) {
        _Base::__deallocate(_impl);
      }
      _impl = static_cast<allocator_type>(other._impl);
    }
    this->__assign_aux(other._impl._begin, other._impl._end,
                       std::random_access_iterator_tag());
    return *this;
  }
  deque& operator=(deque&& other) {
    if (this->get_allocator() == other.get_allocator()) {
      this->clear();
      _impl.__swap(other._impl);
      return *this;
    }
    if (_DataAllocTraits::propagate_on_container_move_assignment::value) {
      _Base::__deallocate(_impl);
      _impl.__swap(other._impl);
      _impl = std::move(static_cast<allocator_type>(other._impl));
    } else {
      this->__assign_aux(std::move_iterator<iterator>(other.begin()),
                         std::move_iterator<iterator>(other.end()),
                         std::random_access_iterator_tag());
    }
    return *this;
  }
  deque& operator=(std::initializer_list<value_type> ilist) {
    this->__assign_aux(ilist.begin(), ilist.end(),
                       std::random_access_iterator_tag());
    return *this;
  }
  constexpr allocator_type get_allocator() const {
    return static_cast<allocator_type>(_impl);
  }

  /////////////// element access ///////////////
 public:
  reference at(size_type pos) {
    ginshio::stl::__check_out_of_range("deque::at", pos, this->size());
    return _impl._begin[pos];
  }
  const_reference at(size_type pos) const {
    ginshio::stl::__check_out_of_range("deque::at", pos, this->size());
    return _impl._begin[pos];
  }
  reference operator[](size_type pos) { return _impl._begin[pos]; }
  constexpr const_reference operator[](size_type pos) const {
    return _impl._begin[pos];
  }
  reference front() { return *_impl._begin; }
  constexpr const_reference front() const { return *_impl._begin; }
  reference back() { return *(_impl.end - 1); }
  constexpr const_reference back() const { return *(_impl.end - 1); }

  /////////////// iterator ///////////////
 public:
  iterator begin() noexcept { return _impl._begin; }
  constexpr const_iterator begin() const noexcept {
    return const_iterator(_impl._begin);
  }
  constexpr const_iterator cbegin() const noexcept {
    return const_iterator(_impl._begin);
  }
  iterator end() noexcept { return _impl._end; }
  constexpr const_iterator end() const noexcept {
    return const_iterator(_impl._end);
  }
  constexpr const_iterator cend() const noexcept {
    return const_iterator(_impl._end);
  }
  reverse_iterator rbegin() noexcept {
    return reverse_iterator(_impl._end);
  }
  constexpr const_reverse_iterator rbegin() const noexcept {
    return const_reverse_iterator(const_iterator(_impl._end));
  }
  constexpr const_reverse_iterator crbegin() const noexcept {
    return const_reverse_iterator(const_iterator(_impl._end));
  }
  reverse_iterator rend() noexcept {
    return reverse_iterator(_impl._begin);
  }
  constexpr const_reverse_iterator rend() const noexcept {
    return const_reverse_iterator(const_iterator(_impl._begin));
  }
  constexpr const_reverse_iterator crend() const noexcept {
    return const_reverse_iterator(const_iterator(_impl._begin));
  }

  /////////////// capacity ///////////////
 public:
  constexpr bool empty() const noexcept { return _impl._begin == _impl._end; }
  constexpr size_type size() const noexcept {
    return _impl._end - _impl._begin;
  }
  constexpr size_type max_size() const noexcept { return _Base::__max_size(); }
  void shrink_to_fit() {
    if (_impl._begin._cur == _impl._begin._first ||
        _impl._begin._cur == _impl._end._cur) {
      return;
    }
    std::size_t _new_size = (_impl._end - _impl._begin) /
        __container_base::__deque_chunk_capacity<value_type>() + 3;
    if (_impl._chunk_size <= _new_size) {
      return;
    }
    _ChunkPtr _chunks = _Base::__get_map(_impl, _new_size);
    size_type _dist =
        static_cast<size_type>(_impl._end._node - _impl._begin._node) + 1;
    std::memcpy(_chunks + 1, _impl._begin._node, sizeof(_ChunkPtr) * _dist);
    _Base::__put_map(_impl, _impl._set, _impl._chunk_size);
    _impl._begin._node = _chunks + 1;
    _impl._end._node = _impl._begin._node + _dist - 1;
    _impl._chunk_size = _new_size;
    _impl._set = _chunks;
  }
  constexpr size_type capacity() const noexcept {
    return _impl._chunk_size *
        __container_base::__deque_chunk_capacity<value_type>();
  }

  /////////////// modifier ///////////////
 public:
  void clear() noexcept {
    ginshio::stl::destroy(_impl._begin, _impl._end);
    _Base::__put_chunk(_impl, _impl._begin._node + 1,
                       _impl._end._node - _impl._begin._node);
    _impl._end = _impl._begin;
  }
  iterator insert(const_iterator pos, const value_type& value) {
    const difference_type _dist = pos - _impl._begin;
    this->emplace(pos, value);
    return _impl._begin + _dist;
  }
  iterator insert(const_iterator pos, value_type&& value) {
    const difference_type _dist = pos - _impl._begin;
    this->emplace(pos, std::forward<value_type>(value));
    return _impl._begin + _dist;
  }
  iterator insert(const_iterator pos,
                  size_type count, const value_type& value) {
    iterator _it = this->__insert_aux(iterator(pos._cur, pos._node), count);
    ginshio::stl::uninitialized_fill_n(_it, count, value);
    return _it;
  }
  template <typename InputIt, typename = typename
            std::enable_if<std::is_base_of<
                             std::input_iterator_tag, typename
                             std::iterator_traits<InputIt>::iterator_category>::
                           value>::type*>
  iterator insert(const_iterator pos, InputIt first, InputIt last) {
    using _Category = typename std::iterator_traits<InputIt>::iterator_category;
    if (first != last) {
      const size_type _dist = static_cast<size_type>(pos - this->cbegin());
      this->__insert_aux(iterator(pos._cur, pos._node),
                         first, last, _Category());
      return _impl._begin + _dist;
    }
    return iterator(pos._cur, pos._node);
  }
  iterator insert(const_iterator pos, std::initializer_list<value_type> ilist) {
    iterator _it = deque::
        __insert_aux(_impl, iterator(pos._cur, pos._node), ilist.size());
    ginshio::stl::uninitialized_copy(ilist.begin(), ilist.end(), _it);
    return _it;
  }
  template <typename... Args>
  reference emplace(const_iterator pos, Args&&... args) {
    if (pos == _impl._begin) {
      return this->emplace_front(std::forward<Args>(args)...);
    } else if (pos == _impl._end) {
      return this->emplace_back(std::forward<Args>(args)...);
    }
    iterator _it = deque::
        __is_front(static_cast<size_type>(pos - _impl._begin),
                   static_cast<size_type>(_impl._end - _impl._begin) - 1) ?
        deque::__exten_cap_front(_impl, iterator(pos._cur, pos._node), 1) :
        deque::__exten_cap_back(_impl, iterator(pos._cur, pos._node), 1);
    ginshio::stl::construct(_it._cur, std::forward<Args>(args)...);
    return *_it;
  }
  iterator erase(const_iterator pos) {
    size_type _dist = static_cast<size_type>(pos - _impl._begin);
    if (deque::__is_front(_dist, _impl._end - _impl._begin)) {
      ginshio::stl::move_backward(const_iterator(_impl._begin), pos,
                                  ++iterator(pos._cur, pos._node));
      this->pop_front();
      return _impl._begin + _dist;
    }
    if (pos != _impl._end) {
      ginshio::stl::move(pos + 1, const_iterator(_impl._end),
                         iterator(pos._cur, pos._node));
      this->pop_back();
    }
    return iterator(pos._cur, pos._node);
  }
  iterator erase(const_iterator first, const_iterator last);
  void push_back(const value_type& value) { this->emplace_back(value); }
  void push_back(value_type&& value) {
    this->emplace_back(std::forward<value_type>(value));
  }
  template <typename... Args>
  reference emplace_back(Args&&... args) {
    if (_impl._end._cur == _impl._end._last - 1) {
      _Base::__reallocate_map(_impl, 1, false);
    }
    try {
      ginshio::stl::construct(_impl._end._cur, std::forward<Args>(args)...);
    } catch (...) {
      _DataAllocTraits::
          deallocate(_impl, *(_impl._end._node + 1),
                     __container_base::__deque_chunk_capacity<value_type>());
      throw;
    }
    ++_impl._end;
    return *(_impl._end - 1);
  }
  void pop_back() {
    if (_impl._end._cur == _impl._end._first) {
      _DataAllocTraits::
          deallocate(_impl, *_impl._end._node,
                     __container_base::__deque_chunk_capacity<value_type>());
      _impl._end.__set_node(-1);
      _impl._end._cur = _impl._end._last - 1;
    } else {
      --_impl._end;
    }
    ginshio::stl::destroy(_impl._end._cur);
  }
  void push_front(const value_type& value) { this->emplace_front(value); }
  void push_front(value_type&& value) {
    this->emplace_front(std::forward<value_type>(value));
  }
  template <typename... Args>
  reference emplace_front(Args&&... args) {
    if (_impl._begin._cur == _impl._begin._first) {
      _Base::__reallocate_map(_impl, 1, true);
    }
    try {
      --_impl._begin;
      ginshio::stl::construct(_impl._begin._cur, std::forward<Args>(args)...);
    } catch (...) {
      ++_impl._begin;
      _DataAllocTraits::
          deallocate(_impl, *(_impl._begin._node - 1),
                     __container_base::__deque_chunk_capacity<value_type>());
      throw;
    }
    return *_impl._begin;
  }
  void pop_front() {
    ginshio::stl::destroy(_impl._begin._cur);
    if (_impl._begin._cur + 1 == _impl._begin._last) {
      _impl._begin.__set_node(1);
      _impl._begin._cur = *_impl._begin._node;
      _DataAllocTraits::
          deallocate(_impl, *(_impl._begin._node - 1),
                     __container_base::__deque_chunk_capacity<value_type>());
    } else {
      ++_impl._begin;
    }
  }
  void resize(size_type count);
  void resize(size_type count, const value_type& value) {
    if (this->size() < count) {
      ginshio::stl::
          __check_length_error("deque::resize", count, _Base::__max_size());
      this->insert(_impl._end, count - this->size(), value);
    } else if (this->size() > count) {
      deque::__erase_back(_impl, _impl._begin + count);
    }
  }
  void swap(deque& other)
      noexcept(_DataAllocTraits::propagate_on_container_swap::value ||
               _DataAllocTraits::is_always_equal::value) {
    if (this == &other) {
      return;
    }
    _Base::__swap_allocator(_impl, other._impl);
    _impl.__swap(other._impl);
  }

  /////////////// assign ///////////////
 private:
  template <typename _InputIt>
  void __assign_aux(_InputIt _first, _InputIt _last, std::input_iterator_tag);
  template <typename _ForwardIt>
  void __assign_aux(_ForwardIt _first, _ForwardIt _last,
                    std::forward_iterator_tag) {
    const size_type _len = static_cast<size_type>(std::distance(_first, _last));
    if (this->size() < _len) {
      ginshio::stl::uninitialized_copy(
          ginshio::stl::copy_n(_first, this->size(), _impl._begin).first,
          _last, this->__insert_aux(_impl._end, _len - this->size()));
      return;
    }
    deque::__erase_back(_impl, ginshio::stl::copy(_first, _last, _impl._begin));
  }

  /////////////// insert ///////////////
 private:
  template <typename _InputIt>
  void __insert_aux(iterator _pos, _InputIt _first, _InputIt _last,
                    std::input_iterator_tag);
  template <typename _ForwardIt>
  void __insert_aux(iterator _pos, _ForwardIt _first, _ForwardIt _last,
                    std::forward_iterator_tag) {
    ginshio::stl::uninitialized_copy(
        _first, _last,
        this->__insert_aux(_pos, std::distance(_first, _last)));
  }
  template <typename _RandomIt, typename = typename
            std::enable_if<std::is_same<_RandomIt, iterator>::value ||
                           std::is_same<_RandomIt, const_iterator>::value>::
            type*>
  void __insert_aux(iterator _pos, _RandomIt _first, _RandomIt _last,
                    std::random_access_iterator_tag) {
    const size_type _len = _last - _first;
    if (_impl._begin <= _first && _last <= _impl._end) {
      const difference_type _first_dist = _first - _pos;
      const difference_type _last_dist = _last - _pos;
      iterator _it = this->__insert_aux(_pos, _len);
      _first = _it + (_first_dist < 0 ? _first_dist : _first_dist + _len);
      _last = _it + (_last_dist <= 0 ? _last_dist : _last_dist + _len);
      if (_first_dist < 0 && 0 < _last_dist) {
        ginshio::stl::uninitialized_copy(_it + _len, _last, ginshio::stl::
                                         uninitialized_copy(_first, _it, _it));
      } else {
        ginshio::stl::copy(_first, _last, _it);
      }
      return;
    }
    ginshio::stl::uninitialized_copy(_first, _last,
                                     this->__insert_aux(_pos, _len));
  }
  iterator __insert_aux(iterator _pos, const size_type& _len);

  /////////////// erase ///////////////
 private:
  static void __erase_front(_BaseImpl& _impl, iterator _pos) {
    ginshio::stl::destroy(_impl._begin, _pos);
    _Base::__put_chunk(_impl, _impl._begin._node,
                       _pos._node - _impl._begin._node);
    _impl._begin = _pos;
  }
  static void __erase_back(_BaseImpl& _impl, iterator _pos) {
    ginshio::stl::destroy(_pos, _impl._end);
    _Base::__put_chunk(_impl, _pos._node + 1,
                       _impl._end._node - _pos._node);
    _impl._end = _pos;
  }

  /////////////// move elements ///////////////
 private:
  static bool __is_front(const size_type& _dist, const size_type& _size) {
    return _dist < (_size >> 1);
  }
  static iterator __exten_cap_front(_BaseImpl& _impl, iterator _pos,
                                    const size_type& _len);
  static iterator __exten_cap_back(_BaseImpl& _impl, iterator _pos,
                                   const size_type& _len);
};



///////////////////////// java style iterator /////////////////////////
template <typename T>
using DequeIterator = __container_base::_DequeIterator<T, T*, T&>;
template <typename T>
using DequeConstIterator =
    __container_base::_DequeIterator<T, const T*, const T&>;



///////////////////////// deque comparison operators /////////////////////////
template <typename T, typename Allocator>
constexpr bool operator==(const deque<T, Allocator>& lhs,
                          const deque<T, Allocator>& rhs) {
  return &lhs == &rhs || (lhs.size() == rhs.size() &&
                          std::equal(lhs.begin(), lhs.end(), rhs.begin()));
}
template <typename T, typename Allocator>
constexpr bool operator!=(const deque<T, Allocator>& lhs,
                          const deque<T, Allocator>& rhs) {
  return &lhs != &rhs && (lhs.size() != rhs.size() ||
                          !std::equal(lhs.begin(), lhs.end(), rhs.begin()));
}
template <typename T, typename Allocator>
constexpr bool operator<(const deque<T, Allocator>& lhs,
                         const deque<T, Allocator>& rhs) {
  return std::lexicographical_compare(lhs.begin(), lhs.end(),
                                      rhs.begin(), rhs.end());
}
template <typename T, typename Allocator>
constexpr bool operator>(const deque<T, Allocator>& lhs,
                         const deque<T, Allocator>& rhs) {
  return std::lexicographical_compare(rhs.begin(), rhs.end(),
                                      lhs.begin(), lhs.end());
}
template <typename T, typename Allocator>
constexpr bool operator<=(const deque<T, Allocator>& lhs,
                          const deque<T, Allocator>& rhs) {
  return !operator<(rhs, lhs);
}
template <typename T, typename Allocator>
constexpr bool operator>=(const deque<T, Allocator>& lhs,
                          const deque<T, Allocator>& rhs) {
  return !operator<(lhs, rhs);
}



///////////////////////// specialization /////////////////////////
template <typename T, typename Allocator>
inline void swap(deque<T, Allocator>& lhs, deque<T, Allocator>& rhs) {
  lhs.swap(rhs);
}

template <typename T, typename Allocator, typename U>
inline auto erase(deque<T, Allocator>& d, const U& value)
    -> typename deque<T, Allocator>::size_type {
  typename deque<T, Allocator>::size_type size = d.size();
  d.erase(std::remove(d.begin(), d.end(), value), d.end());
  return size - d.size();
}
template <typename T, typename Allocator, typename Pred>
inline auto erase_if(deque<T, Allocator>& d, Pred pred)
    -> typename deque<T, Allocator>::size_type {
  typename deque<T, Allocator>::size_type size = d.size();
  d.erase(std::remove_if(d.begin(), d.end(), pred), d.end());
  return size - d.size();
}

} // namespace stl
} // namespace ginshio





namespace std {
///////////////////////// specialization /////////////////////////
template <typename T, typename Allocator>
inline void swap(ginshio::stl::deque<T, Allocator>& lhs,
                 ginshio::stl::deque<T, Allocator>& rhs) {
  lhs.swap(rhs);
}

template <typename T, typename Allocator, typename U>
inline auto erase(ginshio::stl::deque<T, Allocator>& d, const U& value)
    -> typename ginshio::stl::deque<T, Allocator>::size_type {
  auto size = d.size();
  d.erase(std::remove(d.begin(), d.end(), value), d.end());
  return size - d.size();
}
template <typename T, typename Allocator, typename Pred>
inline auto erase_if(ginshio::stl::deque<T, Allocator>& d, Pred pred)
    -> typename ginshio::stl::deque<T, Allocator>::size_type {
  auto size = d.size();
  d.erase(std::remove_if(d.begin(), d.end(), pred), d.end());
  return size - d.size();
}
} // namespace std

#endif // GINSHIO_STL__STL_DEQUE_HH_
