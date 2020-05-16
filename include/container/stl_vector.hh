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


#ifndef GINSHIO_STL__STL_VECTOR_HH_
#define GINSHIO_STL__STL_VECTOR_HH_ 1

namespace ginshio {
namespace stl {

namespace __container_base {
///////////////////////// vector base /////////////////////////
template <typename _T, typename _Allocator>
struct _VectorBase {
  /////////////// member type ///////////////
 protected:
  using _ValueType = _T;
  using _AllocatorType = _Allocator;
  using _AllocatorTraits = std::allocator_traits<_AllocatorType>;

  /////////////// implement ///////////////
 protected:
  struct _VectorImpl : public _AllocatorType {
    _ValueType* _begin;
    _ValueType* _end;
    _ValueType* _finish;
    _VectorImpl() = default;
    _VectorImpl(const _AllocatorType& _alloc) : _AllocatorType(_alloc) {}
    void __swap(_VectorImpl& _impl) {
      std::swap(_impl._begin, _begin);
      std::swap(_impl._end, _end);
      std::swap(_impl._finish, _finish);
    }
  };

  /////////////// constructor ///////////////
 public:
  _VectorBase() { _VectorBase::__get(0, _impl); }
  _VectorBase(const std::size_t& _cap) : _VectorBase(_cap, _AllocatorType()) {}
  _VectorBase(const _AllocatorType& _alloc) : _VectorBase(0, _alloc) {}
  _VectorBase(const std::size_t& _cap,
              const _AllocatorType& _alloc) : _impl(_alloc) {
    ginshio::stl::
        __check_length_error("vector", _cap, _VectorBase::__max_size());
    _VectorBase::__get(_cap, _impl);
  }
  _VectorBase(_VectorBase&& _other) noexcept : _impl(std::move(_other._impl)) {
    _other._impl._begin = _other._impl._end = _other._impl._finish = nullptr;
  }

  /////////////// destructor ///////////////
 public:
  ~_VectorBase() noexcept {
    if (_impl._begin) {
      ginshio::stl::destroy(_impl._begin, _impl._end);
      _VectorBase::__put(_impl);
    }
  }

  /////////////// data member ///////////////
 protected:
  _VectorImpl _impl;

  /////////////// static function ///////////////
 protected:
  static void __get(const std::size_t& _cap, _VectorImpl& _impl) {
    if (_cap != 0) {
      _impl._end = _impl._begin = _AllocatorTraits::allocate(_impl, _cap);
      _impl._finish = _impl._begin + _cap;
    } else {
      _impl._begin = _impl._end = _impl._finish = nullptr;
    }
  }
  static void __put(_VectorImpl& _impl) {
    _AllocatorTraits::
        deallocate(_impl, _impl._begin, _impl._finish - _impl._begin);
    _impl._begin = _impl._end = _impl._finish = nullptr;
  }
  static void __swap_allocator(_AllocatorType& _a, _AllocatorType& _b) {
    std::swap(_a, _b);
  }
  static constexpr std::size_t __max_size() noexcept {
    return
        static_cast<std::size_t>(std::numeric_limits<std::ptrdiff_t>::max()) /
        sizeof(_ValueType);
  }
};
}  // namespace __container_base



///////////////////////// vector /////////////////////////
template <typename T, typename Allocator = std::allocator<T>>
class vector : protected __container_base::_VectorBase<T, Allocator> {
  /////////////// private type ///////////////
 private:
  using _Base = __container_base::_VectorBase<T, Allocator>;
  using _BaseImpl = typename _Base::_VectorImpl;
  using _AllocatorTraits = typename _Base::_AllocatorTraits;
  /////////////// member type ///////////////
 public:
  using value_type = T;
  using allocator_type = typename _AllocatorTraits::allocator_type;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = typename _AllocatorTraits::pointer;
  using const_pointer = typename _AllocatorTraits::const_pointer;
  using iterator = value_type*;
  using const_iterator = const value_type*;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  /////////////// data member ///////////////
 private:
  using _Base::_impl;

  /////////////// constructor ///////////////
 public:
  vector() = default;
  explicit vector(const allocator_type& alloc) : _Base(alloc) {}
  explicit vector(size_type count, const value_type& value,
                  const allocator_type& alloc = allocator_type()) :
      _Base(count, alloc) {
    _impl._end = ginshio::stl::
        uninitialized_fill_n(_impl._begin, count, value);
  }
  explicit vector(size_type count,
                  const allocator_type& alloc = allocator_type()) :
      _Base(count, alloc) {
    try {
      for (size_type _n = 0; _n < count; ++_n) {
        ginshio::stl::construct(_impl._end);
        ++_impl._end;
      }
    } catch (...) {
      ginshio::stl::destroy(_impl._begin, _impl._end);
      _impl._end = _impl._begin;
      throw;
    }
  }
  template <typename InputIt, typename = typename
            std::enable_if<std::is_base_of<
                             std::input_iterator_tag, typename
                             std::iterator_traits<InputIt>::iterator_category>::
                           value>::type*>
  vector(InputIt first, InputIt last,
         const allocator_type& alloc = allocator_type()) : _Base(alloc) {
    using _Category = typename std::iterator_traits<InputIt>::iterator_category;
    vector::__range_init(first, last, _Category());
  }
  vector(const vector& other) :
      _Base(other.size(), _AllocatorTraits::
            select_on_container_copy_construction(other.get_allocator())) {
    _impl._end = ginshio::stl::
        uninitialized_copy(other.begin(), other.end(), _impl._begin);
  }
  vector(const vector& other, const allocator_type& alloc)
      : _Base(other.size(), alloc) {
    _impl._end = ginshio::stl::
        uninitialized_copy(other.begin(), other.end(), _impl._begin);
  }
  vector(vector&& other) noexcept = default;
  vector(vector&& other, const allocator_type& alloc) : _Base(alloc) {
    if (other.get_allocator() == alloc) {
      _impl.__swap(other._impl);
      return;
    }
    _Base::__get(other.size(), _impl);
    _impl._end = ginshio::stl::
        uninitialized_move(other.begin(), other.end(), _impl._begin);
    other._impl._end = other._impl._begin;
  }
  vector(std::initializer_list<value_type> ilist,
         const allocator_type& alloc = allocator_type()) :
      _Base(ilist.size(), alloc) {
    _impl._end = ginshio::stl::
        uninitialized_copy(ilist.begin(), ilist.end(), _impl._begin);
  }

  /////////////// destructor ///////////////
 public:
  ~vector() noexcept = default;

  /////////////// member function ///////////////
 public:
  void assign(size_type count, const value_type& value) {
    if (this->capacity() < count) {
      return vector{count, value, this->get_allocator()}._impl.__swap(_impl);
    }
    if (this->size() < count) {
      _impl._end = ginshio::stl::uninitialized_fill_n(
          ginshio::stl::fill_n(_impl._begin, this->size(), value),
          count - this->size(), value);
    } else {
      ginshio::stl::destroy(ginshio::stl::fill_n(_impl._begin, count, value),
                            _impl._end);
      _impl._end = _impl._begin + count;
    }
  }
  template <typename InputIt, typename = typename
            std::enable_if<std::is_base_of<
                             std::input_iterator_tag, typename
                             std::iterator_traits<InputIt>::iterator_category>::
                           value>::type*>
  void assign(InputIt first, InputIt last) {
    using _Category = typename std::iterator_traits<InputIt>::iterator_category;
    this->__assign_range_dispatch(first, last, _Category());
  }
  void assign(std::initializer_list<value_type> ilist) {
    this->__assign_range_dispatch(ilist.begin(), ilist.end(),
                                  std::random_access_iterator_tag());
  }
  vector& operator=(const vector& other) {
    if (this == &other) {
      return *this;
    }
    if (_AllocatorTraits::propagate_on_container_copy_assignment::value) {
      if (other.get_allocator() != this->get_allocator()) {
        ginshio::stl::destroy(_impl._begin, _impl._end);
        _Base::__put(_impl);
      }
      static_cast<allocator_type>(_impl) =
          static_cast<allocator_type>(other._impl);
    }
    this->__assign_range_dispatch(other._impl._begin, other._impl._end,
                                  std::random_access_iterator_tag());
    return *this;
  }
  vector& operator=(vector&& other);
  vector& operator=(std::initializer_list<value_type> ilist) {
    this->__assign_range_dispatch(ilist.begin(), ilist.end(),
                                  std::random_access_iterator_tag());
    return *this;
  }
  constexpr allocator_type get_allocator() const {
    return static_cast<allocator_type>(_impl);
  }

  /////////////// element access ///////////////
 public:
  reference at(size_type pos) {
    ginshio::stl::
        __check_out_of_range("vector::at", pos, this->size());
    return _impl._begin[pos];
  }
  const_reference at(size_type pos) const {
    ginshio::stl::
        __check_out_of_range("vector::at", pos, this->size());
    return _impl._begin[pos];
  }
  reference operator[](size_type pos) { return _impl._begin[pos]; }
  constexpr const_reference operator[](size_type pos) const {
    return _impl._begin[pos];
  }
  reference front() { return *_impl._begin; }
  constexpr const_reference front() const { return *_impl._begin; }
  reference back() { return *(_impl._end - 1); }
  constexpr const_reference back() const { return *(_impl._end - 1); }
  pointer data() noexcept { return _impl._begin; }
  constexpr const_pointer data() const noexcept { return _impl._begin; }

  /////////////// iterator ///////////////
 public:
  iterator begin() noexcept { return _impl._begin; }
  constexpr const_iterator begin() const noexcept { return _impl._begin; }
  constexpr const_iterator cbegin() const noexcept { return _impl._begin; }
  iterator end() noexcept { return _impl._end; }
  constexpr const_iterator end() const noexcept { return _impl._end; }
  constexpr const_iterator cend() const noexcept { return _impl._end; }
  reverse_iterator rbegin() noexcept { return reverse_iterator(_impl._end); }
  constexpr const_reverse_iterator rbegin() const noexcept {
    return const_reverse_iterator(_impl._end);
  }
  constexpr const_reverse_iterator crbegin() const noexcept {
    return const_reverse_iterator(_impl._end);
  }
  reverse_iterator rend() noexcept { return reverse_iterator(_impl._begin); }
  constexpr const_reverse_iterator rend() const noexcept {
    return const_reverse_iterator(_impl._begin);
  }
  constexpr const_reverse_iterator crend() const noexcept {
    return const_reverse_iterator(_impl._begin);
  }

  /////////////// capacity ///////////////
 public:
  constexpr bool empty() const noexcept { return _impl._begin == _impl._end; }
  constexpr size_type size() const noexcept {
    return static_cast<size_type>(_impl._end - _impl._begin);
  }
  constexpr size_type max_size() const noexcept { return _Base::__max_size(); }
  void reserve(size_type new_cap) {
    if (new_cap <= this->capacity()) {
      return;
    }
    ginshio::stl::
        __check_length_error("vector::reserve", new_cap, _Base::__max_size());
    _BaseImpl _old{_impl};
    _Base::__get(new_cap, _impl);
    _impl._end = ginshio::stl::
        uninitialized_move(_old._begin, _old._end, _impl._begin);
    ginshio::stl::destroy(_old._begin, _old._end);
    _Base::__put(_old);
  }
  constexpr size_type capacity() const noexcept {
    return static_cast<size_type>(_impl._finish - _impl._begin);
  }
  void shrink_to_fit() {
    if (_impl._end != _impl._finish) {
      vector _tmp{std::move_iterator<iterator>(_impl._begin),
                  std::move_iterator<iterator>(_impl._end),
                  this->get_allocator()};
      _tmp._impl.__swap(_impl);
      _tmp._impl._end = _tmp._impl._begin;
    }
  }

  /////////////// modifier ///////////////
 public:
  void clear() noexcept {
    ginshio::stl::destroy(_impl._begin, _impl._end);
    _impl._end = _impl._begin;
  }
  iterator insert(const_iterator pos, const value_type& value) {
    return this->emplace(pos, value);
  }
  iterator insert(const_iterator pos, value_type&& value) {
    return this->emplace(pos, std::forward<value_type>(value));
  }
  iterator insert(const_iterator pos,
                  size_type count, const value_type& value);
  template <typename InputIt, typename = typename
            std::enable_if<std::is_base_of<
                             std::input_iterator_tag, typename
                             std::iterator_traits<InputIt>::iterator_category>::
                           value>::type*>
  iterator insert(const_iterator pos, InputIt first, InputIt last) {
    using _Category = typename std::iterator_traits<InputIt>::iterator_category;
    if (first != last) {
      const size_type _dist = static_cast<size_type>(pos - this->cbegin());
      this->__insert_range_dispatch(pos, first, last, _Category());
      return _impl._begin + _dist;
    }
    return first;
  }
  iterator insert(const_iterator pos, std::initializer_list<value_type> ilist) {
    const size_type _dist = static_cast<size_type>(pos - this->cbegin());
    if (0 < ilist.size()) {
      this->__insert_range(_impl, const_cast<iterator>(pos),
                           ilist.begin(), ilist.end(),
                           ilist.size(), std::false_type());
    }
    return _impl._begin + _dist;
  }
  iterator emplace(const_iterator pos, const value_type& value);
  template <typename... Args>
  iterator emplace(const_iterator pos, Args&&... args);
  iterator erase(const_iterator pos) {
    if (pos == this->cend()) {
      return _impl._end;
    }
    _impl._end = ginshio::stl::move(const_cast<iterator>(pos) + 1, _impl._end,
                                    const_cast<iterator>(pos));
    ginshio::stl::destroy(_impl._end);
    return const_cast<iterator>(pos);
  }
  iterator erase(const_iterator first, const_iterator last) {
    if (first == last) {
      return const_cast<iterator>(last);
    }
    iterator _old_end = _impl._end;
    _impl._end = ginshio::stl::
        move(const_cast<iterator>(last), _old_end, const_cast<iterator>(first));
    ginshio::stl::destroy(_impl._end, _old_end);
    return const_cast<iterator>(first);
  }
  void push_back(const value_type& value) {
    this->emplace_back(value);
  }
  void push_back(value_type&& value) {
    this->emplace_back(std::forward<value_type>(value));
  }
  reference emplace_back(const value_type& value) {
    if (_impl._end == _impl._finish) {
      value_type _tmp = value;
      ginshio::stl::construct(vector::__reallocate(_impl, this->size(), 1),
                              std::move(_tmp));
    } else {
      ginshio::stl::construct(_impl._end++, value);
    }
    return *(_impl._end - 1);
  }
  template <typename... Args>
  reference emplace_back(Args&&... args) {
    if (_impl._end == _impl._finish) {
      ginshio::stl::construct(vector::__reallocate(_impl, this->size(), 1),
                              std::forward<Args>(args)...);
    } else {
      ginshio::stl::construct(_impl._end++, std::forward<Args>(args)...);
    }
    return *(_impl._end - 1);
  }
  void pop_back() {
    --_impl._end;
    ginshio::stl::destroy(_impl._end);
  }
  void resize(size_type count) { this->resize(count, value_type()); }
  void resize(size_type count, const value_type& value) {
    size_type _size = this->size();
    if (_size < count) {
      __check_length_error("vector::resize", count, _Base::__max_size());
      this->insert(_impl._end, count - _size, value);
    } else if (count < _size) {
      ginshio::stl::destroy(_impl._begin + count, _impl._end);
      _impl._end = _impl._begin + count;
    }
  }
  void swap(vector& other)
      noexcept(_AllocatorTraits::propagate_on_container_swap::value ||
               _AllocatorTraits::is_always_equal::value) {
    if (this == &other) {
      return;
    }
    _Base::__swap_allocator(_impl, other._impl);
    _impl.__swap(other._impl);
  }

  /////////////// assign ///////////////
 private:
  template <typename _InputIt>
  void __assign_range_dispatch(_InputIt _first, _InputIt _last,
                               std::input_iterator_tag);
  template <typename _ForwardIt>
  void __assign_range_dispatch(_ForwardIt _first, _ForwardIt _last,
                               std::forward_iterator_tag) {
    const size_type _len = static_cast<size_type>(std::distance(_first, _last));
    if (this->capacity() < _len) {
      return vector{_first, _last, this->get_allocator()}._impl.__swap(_impl);
    }
    if (this->size() < _len) {
      _impl._end = ginshio::stl::
          uninitialized_copy(ginshio::stl::
                             copy_n(_first, this->size(), _impl._begin).first,
                             _last, _impl._end);
      return;
    }
    ginshio::stl::destroy(ginshio::stl::
                          copy(_first, _last, _impl._begin), _impl._end);
    _impl._end = _impl._begin + _len;
  }

  /////////////// insert ///////////////
 private:
  template <typename _InputIt>
  static void __insert_range(_BaseImpl& _impl, iterator _pos,
                             _InputIt _first, _InputIt _last,
                             const size_type& _len, std::false_type);
  // @function __insert_range(void): insert self range
  template <typename _RandomIt>
  static void __insert_range(_BaseImpl& _impl, iterator _pos,
                             _RandomIt _first, _RandomIt _last,
                             const size_type& _len, std::true_type);
  template <typename _InputIt>
  void __insert_range_dispatch(const_iterator _pos,
                               _InputIt _first, _InputIt _last,
                               std::input_iterator_tag);
  template <typename _ForwardIt>
  void __insert_range_dispatch(const_iterator _pos,
                               _ForwardIt _first, _ForwardIt _last,
                               std::forward_iterator_tag) {
    vector::__insert_range(_impl, const_cast<iterator>(_pos), _first, _last,
                           std::distance(_first, _last), std::false_type());
  }
  template <typename _RandomIt, typename = typename
            std::enable_if<std::is_same<_RandomIt, iterator>::value ||
                           std::is_same<_RandomIt, const_iterator>::value>::
            type*>
  void __insert_range_dispatch(const_iterator _pos,
                               _RandomIt _first, _RandomIt _last,
                               std::random_access_iterator_tag) {
    if (_impl._begin <= std::addressof(*_first) &&
        std::addressof(*_last) <= _impl._end) {
      return vector::__insert_range(_impl, const_cast<iterator>(_pos),
                                    _first, _last, _last - _first,
                                    std::true_type());
    }
    vector::__insert_range(_impl, const_cast<iterator>(_pos), _first, _last,
                           _last - _first, std::false_type());
  }

  /////////////// initialized ///////////////
 private:
  /**
   * @function __reallocate(iterator):
   **** reallocate capacity, move original data
   **** 重新分配容量, 使用 move 将原始数据移动到新空间
   * @param _impl(_BaseImpl&): vector original data; vector 原始数据
   * @param _dist(const size_type&): length of the first half of the element;
   **** 原始数据中, 从 pos 到 begin 的距离
   * @param _len(const size_type&): length of the element to be inserted;
   **** 待插入元素的长度
   * @return (iterator): start iterator of to be inserted; 待插入元素的起始迭代器
   */
  static iterator __reallocate(_BaseImpl& _impl,
                               const size_type& _dist, const size_type& _len);

  template <typename _InputIt>
  void __range_init(_InputIt _first, _InputIt _last,
                    std::input_iterator_tag);
  template <typename _ForwardIt>
  void __range_init(_ForwardIt _first, _ForwardIt _last,
                    std::forward_iterator_tag) {
    _Base::__get(std::distance(_first, _last), _impl);
    _impl._end = ginshio::stl::uninitialized_copy(_first, _last, _impl._begin);
  }
};



///////////////////////// vector comparison operators /////////////////////////
template <typename T, typename Allocator>
constexpr bool operator==(const vector<T, Allocator>& lhs,
                          const vector<T, Allocator>& rhs) {
  return &lhs == &rhs || (lhs.size() == rhs.size() &&
                          std::equal(lhs.begin(), lhs.end(), rhs.begin()));
}
template <typename T, typename Allocator>
constexpr bool operator!=(const vector<T, Allocator>& lhs,
                          const vector<T, Allocator>& rhs) {
  return &lhs != &rhs && (lhs.size() != rhs.size() ||
                          std::equal(lhs.begin(), lhs.end(), rhs.begin()));
}
template <typename T, typename Allocator>
constexpr bool operator<(const vector<T, Allocator>& lhs,
                         const vector<T, Allocator>& rhs) {
  return std::lexicographical_compare(lhs.begin(), lhs.end(),
                                      rhs.begin(), rhs.end());
}
template <typename T, typename Allocator>
constexpr bool operator>(const vector<T, Allocator>& lhs,
                         const vector<T, Allocator>& rhs) {
  return std::lexicographical_compare(rhs.begin(), rhs.end(),
                                      lhs.begin(), lhs.end());
}
template <typename T, typename Allocator>
constexpr bool operator<=(const vector<T, Allocator>& lhs,
                          const vector<T, Allocator>& rhs) {
  return !operator<(rhs, lhs);
}
template <typename T, typename Allocator>
constexpr bool operator>=(const vector<T, Allocator>& lhs,
                          const vector<T, Allocator>& rhs) {
  return !operator<(lhs, rhs);
}



///////////////////////// specialization /////////////////////////
template <typename T, typename Allocator>
inline void swap(vector<T, Allocator>& lhs, vector<T, Allocator>& rhs) {
  lhs.swap(rhs);
}

template <typename T, typename Allocator, typename U>
inline auto erase(vector<T, Allocator>& v, const U& value)
    -> typename vector<T, Allocator>::size_type {
  typename vector<T, Allocator>::size_type size = v.size();
  v.erase(std::remove(v.begin(), v.end(), value), v.end());
  return size - v.size();
}
template <typename T, typename Allocator, typename Pred>
inline auto erase_if(vector<T, Allocator>& v, Pred pred)
    -> typename vector<T, Allocator>::size_type {
  typename vector<T, Allocator>::size_type size = v.size();
  v.erase(std::remove_if(v.begin(), v.end(), pred), v.end());
  return size - v.size();
}

}  // namespace stl
}  // namespace ginshio



namespace std {
///////////////////////// specialization /////////////////////////
template <typename T, typename Allocator>
inline void swap(ginshio::stl::vector<T, Allocator>& lhs,
                 ginshio::stl::vector<T, Allocator>& rhs) {
  lhs.swap(rhs);
}

template <typename T, typename Allocator, typename U>
inline auto erase(ginshio::stl::vector<T, Allocator>& v, const U& value)
    -> typename ginshio::stl::vector<T, Allocator>::size_type {
  auto size = v.size();
  v.erase(std::remove(v.begin(), v.end(), value), v.end());
  return size - v.size();
}
template <typename T, typename Allocator, typename Pred>
inline auto erase_if(ginshio::stl::vector<T, Allocator>& v, Pred pred)
    -> typename ginshio::stl::vector<T, Allocator>::size_type {
  auto size = v.size();
  v.erase(std::remove_if(v.begin(), v.end(), pred), v.end());
  return size - v.size();
}
} // namespace std

#endif // GINSHIO_STL__STL_VECTOR_HH_
