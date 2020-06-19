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


#ifndef GINSHIO_STL__STL_LIST_HH_
#define GINSHIO_STL__STL_LIST_HH_ 1

namespace ginshio {
namespace stl {

namespace __container_base {
///////////////////////// list node /////////////////////////
struct _ListNodeBase {
  _ListNodeBase* _prev;
  _ListNodeBase* _next;
  void __transfer(_ListNodeBase* _begin, _ListNodeBase* _end) {
    _end->_prev->_next = this;
    _begin->_prev->_next = _end;
    this->_prev->_next = _begin;
    _ListNodeBase* _tmp = this->_prev;
    this->_prev = _end->_prev;
    _end->_prev = _begin->_prev;
    _begin->_prev = _tmp;
  }
  _ListNodeBase* __hook(_ListNodeBase* _other) {
    this->_next = _other;
    this->_prev = _other->_prev;
    this->_prev->_next = this->_next->_prev = this;
    return this;
  }
  _ListNodeBase* __unhook() {
    this->_next->_prev = this->_prev;
    this->_prev->_next = this->_next;
    return this;
  }
};

struct _ListNodeHeader : public _ListNodeBase {
  std::size_t _size;
  _ListNodeHeader() { this->__init_default(); }
  _ListNodeHeader(_ListNodeHeader&& _header) noexcept {
    if (_header._size == 0) {
      this->__init_default();
    } else {
      this->__init_other(_header);
      _header.__init_default();
    }
  }
  _ListNodeHeader& operator=(_ListNodeHeader&& _header) {
    if (_header._size == 0) {
      this->__init_default();
    } else {
      this->__init_other(_header);
      _header.__init_default();
    }
    return *this;
  }
  void __init_default() {
    this->_ListNodeBase::_next = this->_ListNodeBase::_prev = this;
    this->_size = 0;
  }
  void __init_other(_ListNodeHeader& _other) {
    this->_ListNodeBase::_next = _other._ListNodeBase::_next;
    this->_ListNodeBase::_prev = _other._ListNodeBase::_prev;
    this->_ListNodeBase::_next->_prev =
        this->_ListNodeBase::_prev->_next = this;
    this->_size = _other._size;
  }
};

template <typename _T>
struct _ListNode : public _ListNodeBase {
  _T _data;
  _T* __addr() noexcept { return std::addressof(this->_data); }
  constexpr const _T* __addr() const noexcept {
    return std::addressof(this->_data);
  }
};


///////////////////////// list iterator /////////////////////////
template <typename _T, typename _Ptr, typename _Ref>
struct _ListIterator {
  /////////////// traits type ///////////////
 public:
  using difference_type = std::ptrdiff_t;
  using value_type = _T;
  using pointer = _Ptr;
  using reference = _Ref;
  using iterator_category = std::bidirectional_iterator_tag;

  /////////////// iterator type ///////////////
 public:
  using _Iterator = _ListIterator<_T, _T*, _T&>;
  using _ConstIterator = _ListIterator<_T, const _T*, const _T&>;
  using _SelfIterator = _ListIterator<_T, _Ptr, _Ref>;
  using _OtherIterator =
      typename std::conditional<std::is_same<_SelfIterator, _Iterator>::value,
                                _ConstIterator, _Iterator>::type;

  /////////////// node type ///////////////
 public:
  using _NodeBase = _ListNodeBase;
  using _NodeType = _ListNode<_T>;

  /////////////// data member ///////////////
 public:
  _NodeBase* _node;

  /////////////// constructor ///////////////
 public:
  _ListIterator() : _node(nullptr) {}
  explicit _ListIterator(_NodeBase* _n) : _node(_n) {}
  _ListIterator(const _SelfIterator&) = default;
  // @function _ListIterator: const_iterator is constructed from iterator
  template <typename _Iter, typename = typename
            std::enable_if<std::is_same<_SelfIterator, _ConstIterator>::value &&
                           std::is_same<_OtherIterator, _Iter>::value>::type*>
  _ListIterator(const _Iter& _it) : _node(_it._node) {}

  /////////////// assign ///////////////
 public:
  _SelfIterator& operator=(const _SelfIterator&) = default;
  template <typename _Iter, typename = typename
            std::enable_if<std::is_same<_SelfIterator, _ConstIterator>::value &&
                           std::is_same<_OtherIterator, _Iter>::value>::type*>
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
    _node = _node->_next;
    return *this;
  }
  _SelfIterator operator++(int) {
    _SelfIterator _tmp = *this;
    _node = _node->_next;
    return *_tmp;
  }
  _SelfIterator& operator--() {
    _node = _node->_prev;
    return *this;
  }
  _SelfIterator& operator--(int) {
    _SelfIterator _tmp = *this;
    _node = _node->_prev;
    return _tmp;
  }
};
///////////////////////// iterator comparison operator /////////////////////////
template <typename _T, typename _Ptr, typename _Ref>
constexpr bool operator==(const _ListIterator<_T, _Ptr, _Ref>& _lit,
                          const _ListIterator<_T, _Ptr, _Ref>& _rit) {
  return _lit._node == _rit._node;
}
template <typename _T, typename _PtrL, typename _RefL,
          typename _PtrR, typename _RefR>
constexpr bool operator==(const _ListIterator<_T, _PtrL, _RefL>& _lit,
                          const _ListIterator<_T, _PtrR, _RefR>& _rit) {
  return _lit._node == _rit._node;
}
template <typename _T, typename _Ptr, typename _Ref>
constexpr bool operator!=(const _ListIterator<_T, _Ptr, _Ref>& _lit,
                          const _ListIterator<_T, _Ptr, _Ref>& _rit) {
  return _lit._node != _rit._node;
}
template <typename _T, typename _PtrL, typename _RefL,
          typename _PtrR, typename _RefR>
constexpr bool operator!=(const _ListIterator<_T, _PtrL, _RefL>& _lit,
                          const _ListIterator<_T, _PtrR, _RefR>& _rit) {
  return _lit._node != _rit._node;
}


///////////////////////// list base /////////////////////////
template <typename _T, typename _Allocator>
struct _ListBase {
  /////////////// member type ///////////////
 protected:
  using _ValueType = _T;
  using _DataAllocType = _Allocator;
  using _DataAllocTraits = std::allocator_traits<_DataAllocType>;
  using _NodeBase = _ListNodeBase;
  using _NodeType = _ListNode<_ValueType>;
  using _NodeAllocType =
      typename _DataAllocTraits::template rebind_alloc<_NodeType>;
  using _NodeAllocTraits =
      typename _DataAllocTraits::template rebind_traits<_NodeType>;
  using _Iterator = _ListIterator<_ValueType, _ValueType*, _ValueType&>;
  using _ConstIterator =
      _ListIterator<_ValueType, const _ValueType*, const _ValueType&>;

  /////////////// implement ///////////////
 protected:
  struct _ListImpl : public _NodeAllocType {
    _ListNodeHeader _header;
    _ListImpl() = default;
    _ListImpl(const _NodeAllocType& _alloc) : _NodeAllocType(_alloc) {}
    void __swap(_ListImpl& _impl) {
      std::swap(_impl._header, _header);
    }
  };

  /////////////// data member ///////////////
 protected:
  _ListImpl _impl;

  /////////////// constructor ///////////////
 public:
  _ListBase() = default;
  _ListBase(const _NodeAllocType& _alloc) : _impl(_alloc) {}
  _ListBase(_ListBase&& _other) noexcept : _impl(std::move(_other._impl)) {}
  ~_ListBase() noexcept { _ListBase::__clear(_impl); }

  /////////////// utils function ///////////////
 protected:
  static void __clear_aux(_ListImpl& _impl, const std::size_t& _count) {
    _NodeBase* _cur = _impl._header._prev,* _prev = _cur->_prev;
    for (std::size_t _n = _count; _n < _impl._header._size; ++_n) {
      _ListBase::__put(_impl, static_cast<_NodeType*>(_cur));
      _cur = _prev;
      _prev = _prev->_prev;
    }
    _cur->_next = static_cast<_NodeBase*>(&_impl._header);
    _impl._header._prev = _cur;
    _impl._header._size = _count;
  }
  static void __clear(_ListImpl& _impl) {
    if (_impl._header._size == 0) {
      return;
    }
    _ListBase::__clear_aux(_impl, 0);
  }
  static void __swap_allocator(_NodeAllocType& _a, _NodeAllocType& _b) {
    std::swap(_a, _b);
  }
  static constexpr std::size_t __max_size() const noexcept {
    return std::numeric_limits<std::ptrdiff_t>::max() / sizeof(_NodeType);
  }

  /////////////// get and put node ///////////////
 protected:
  template <typename... Args>
  static _NodeBase* __get(_ListImpl& _impl, Args&&... args) {
    _NodeType* _node = _NodeAllocTraits::allocate(_impl, 1);
    try {
      ginshio::stl::construct(_node->__addr(), std::forward<Args>(args)...);
    } catch (...) {
      _NodeAllocTraits::deallocate(_impl, _node, 1);
      throw;
    }
    return static_cast<_NodeBase*>(_node);
  }
  static void __put(_ListImpl& _impl, _NodeType* _node) {
    ginshio::stl::destroy(_node->__addr());
    _NodeAllocTraits::deallocate(_impl, _node, 1);
  }
};
} // namespace __container_base



///////////////////////// list /////////////////////////
template <typename T, typename Allocator = std::allocator<T>>
class list : protected __container_base::_ListBase<T, Allocator> {
  /////////////// private type ///////////////
 private:
  using _Base = __container_base::_ListBase<T, Allocator>;
  using _BaseImpl = typename _Base::_ListImpl;
  using _DataAllocTraits = typename _Base::_DataAllocTraits;
  using _NodeBase = typename _Base::_NodeBase;
  using _NodeHeader = __container_base::_ListNodeHeader;
  using _NodeType = typename _Base::_NodeType;
  using _NodeAllocType = typename _Base::_NodeAllocType;
  using _NodeAllocTraits = typename _Base::_NodeAllocTraits;
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
  list() = default;
  explicit list(const allocator_type& alloc) : _Base(_NodeAllocType(alloc)) {}
  explicit list(size_type count, const value_type& value,
                const allocator_type& alloc = allocator_type()) :
      _Base(_NodeAllocType(alloc)) {
    ginshio::stl::__check_length_error("list", count, _Base::__max_size());
    list::__fill_n(_impl, this->cbegin(), count, value);
  }
  explicit list(size_type count, const allocator_type& alloc = allocator_type())
      : _Base(_NodeAllocType(alloc)) {
    ginshio::stl::__check_length_error("list", count, _Base::__max_size());
    list::__fill_n(_impl, this->cbegin(), count);
  }
  template <typename InputIt, typename = typename
            std::enable_if<std::is_base_of<
                             std::input_iterator_tag, typename
                             std::iterator_traits<InputIt>::iterator_category>::
                           value>::type*>
  list(InputIt first, InputIt last,
       const allocator_type& alloc = allocator_type()) :
      _Base(_NodeAllocType(alloc)) {
    using _Category = typename std::iterator_traits<InputIt>::iterator_category;
    list::__copy(_impl, this->begin(), first, last, _Category());
  }
  list(const list& other) : _Base(_NodeAllocType(
      _DataAllocTraits::
      select_on_container_copy_construction(other.get_allocator()))) {
    list::__copy_n(_impl, this->begin(), other.begin(), other.size());
  }
  list(const list& other, const allocator_type& alloc) :
      _Base(_NodeAllocType(alloc)) {
    list::__copy_n(_impl, this->begin(), other.begin(), other.size());
  }
  list(list&& other) noexcept = default;
  list(list&& other, const allocator_type& alloc) :
      _Base(_NodeAllocType(alloc)) {
    if (alloc == other.get_allocator()) {
      _impl.__swap(other._impl);
      return;
    }
    list::__copy_n(_impl, this->begin(), other.begin(), other.size());
  }
  list(std::initializer_list<value_type> ilist,
       const allocator_type& alloc = allocator_type()) :
      _Base(_NodeAllocType(alloc)) {
    list::__copy_n(_impl, this->begin(), ilist.begin(), ilist.size());
  }

  /////////////// destructor ///////////////
 public:
  ~list() noexcept = default;

  /////////////// member function ///////////////
 public:
  void assign(size_type count, const value_type& value) {
    iterator _it = iterator(_impl._header._next);
    size_type _n = 0;
    for (; _n < _impl._header._size && _n < count; ++_n) {
      *_it = value;
      ++_it;
    }
    if (_n < count) {
      this->insert(_it, count - _n, value);
    } else {
      this->erase(_it, const_iterator(static_cast<_NodeBase*>(&_impl._header)));
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
  list& operator=(const list& other) {
    if (this == &other) {
      return *this;
    }
    if (_DataAllocTraits::propagate_on_container_copy_assignment::value) {
      if (other.get_allocator() != this->get_allocator()) {
        _Base::__clear_aux(_impl, 0);
      }
      _impl = static_cast<_NodeAllocType>(other._impl);
    }
    this->__assign_range_dispatch(other.begin(), other.end(),
                                  std::bidirectional_iterator_tag());
    return *this;
  }
  list& operator=(list&& other) {
    if (this->get_allocator() == other.get_allocator()) {
      _Base::__clear_aux(_impl, 0);
      _impl.__swap(other._impl);
      return *this;
    }
    if (_DataAllocTraits::propagate_on_container_move_assignment::value) {
      _Base::__clear_aux(_impl, 0);
      _impl.__swap(other._impl);
      _impl = std::move(static_cast<allocator_type>(other._impl));
    } else {
      this->__assign_range_dispatch(std::move_iterator<iterator>(other.begin()),
                                    std::move_iterator<iterator>(other.end()),
                                    std::bidirectional_iterator_tag());
    }
    return *this;
  }
  list& operator=(std::initializer_list<value_type> ilist) {
    this->__assign_range_dispatch(ilist.begin(), ilist.end(),
                                  std::random_access_iterator_tag());
    return *this;
  }
  constexpr allocator_type get_allocator() const {
    return allocator_type(static_cast<_NodeAllocType>(_impl));
  }

  /////////////// element access ///////////////
 public:
  reference front() { return *iterator(_impl._header._next); }
  constexpr const_reference front() const {
    return *iterator(_impl._header._next);
  }
  reference back() { return *iterator(_impl._header._prev); }
  constexpr const_reference back() const {
    return *iterator(_impl._header._prev);
  }

  /////////////// iterator ///////////////
 public:
  iterator begin() noexcept { return iterator(_impl._header._next); }
  constexpr const_iterator begin() const noexcept {
    return const_iterator(_impl._header._next);
  }
  constexpr const_iterator cbegin() const noexcept {
    return const_iterator(_impl._header._next);
  }
  iterator end() noexcept { return iterator(&_impl._header); }
  constexpr const_iterator end() const noexcept {
    return const_iterator(const_cast<_NodeHeader*>(&_impl._header));
  }
  constexpr const_iterator cend() const noexcept {
    return const_iterator(const_cast<_NodeHeader*>(&_impl._header));
  }
  reverse_iterator rbegin() noexcept {
    return reverse_iterator(iterator(static_cast<_NodeBase*>(&_impl._header)));
  }
  constexpr const_reverse_iterator rbegin() const noexcept {
    return const_reverse_iterator(
        const_iterator(const_cast<_NodeHeader*>(&_impl._header)));
  }
  constexpr const_reverse_iterator crbegin() const noexcept {
    return const_reverse_iterator(
        const_iterator(const_cast<_NodeHeader*>(&_impl._header)));
  }
  reverse_iterator rend() noexcept {
    return reverse_iterator(iterator(_impl._header._next));
  }
  constexpr const_reverse_iterator rend() const noexcept {
    return const_reverse_iterator(const_iterator(_impl._header._next));
  }
  constexpr const_reverse_iterator crend() const noexcept {
    return const_reverse_iterator(const_iterator(_impl._header._next));
  }

  /////////////// capacity ///////////////
 public:
  constexpr bool empty() const noexcept { return _impl._header._size == 0; }
  constexpr size_type size() const noexcept { return _impl._header._size; }
  constexpr size_type max_size() const noexcept { return _Base::__max_size(); }
  void shrink_to_fit() {}
  constexpr size_type capacity() const noexcept { return _impl._header._size; }

  /////////////// modifier ///////////////
 public:
  void clear() noexcept { _Base::__clear(_impl); }
  iterator insert(const_iterator pos, const value_type& value) {
    return this->emplace(pos, value);
  }
  iterator insert(const_iterator pos, value_type&& value) {
    return this->emplace(pos, std::forward<value_type>(value));
  }
  iterator insert(const_iterator pos, size_type count, const value_type& value) {
    if (count != 0) {
      list _tmp{count, value, this->get_allocator()};
      iterator _it = _tmp.begin();
      this->splice(pos, std::move(_tmp));
      return _it;
    }
    return iterator(pos._node);
  }
  template <typename InputIt, typename = typename
            std::enable_if<std::is_base_of<
                             std::input_iterator_tag, typename
                             std::iterator_traits<InputIt>::iterator_category>::
                           value>::type*>
  iterator insert(const_iterator pos, InputIt first, InputIt last) {
    list _tmp{first, last, this->get_allocator()};
    if (!_tmp.empty()) {
      iterator _it = _tmp.begin();
      this->splice(pos, std::move(_tmp));
      return _it;
    }
    return iterator(pos._node);
  }
  iterator insert(const_iterator pos, std::initializer_list<value_type> ilist) {
    if (ilist.size() != 0) {
      list _tmp{ilist, this->get_allocator()};
      iterator _it = _tmp.begin();
      this->splice(pos, std::move(_tmp));
      return _it;
    }
    return iterator(pos._node);
  }
  template <typename... Args>
  iterator emplace(const_iterator pos, Args&&... args) {
    _NodeBase* _node = _Base::__get(_impl, std::forward<Args>(args)...);
    ++_impl._header._size;
    return iterator(_node->__hook(pos._node));
  }
  iterator erase(const_iterator pos) {
    iterator _ret = iterator(pos._node->_next);
    _Base::__put(_impl, static_cast<_NodeType*>(pos._node->__unhook()));
    --_impl._header._size;
    return _ret;
  }
  iterator erase(const_iterator first, const_iterator last);
  void push_back(const value_type& value) { this->emplace_back(value); }
  void push_back(value_type&& value) {
    this->emplace_back(std::forward<value_type>(value));
  }
  template <typename... Args>
  reference emplace_back(Args&&... args) {
    _NodeBase* _node = _Base::__get(_impl, std::forward<Args>(args)...);
    ++_impl._header._size;
    return *iterator(_node->__hook(static_cast<_NodeBase*>(&_impl._header)));
  }
  void pop_back() {
    _Base::
        __put(_impl, static_cast<_NodeType*>(_impl._header._prev->__unhook()));
    --_impl._header._size;
  }
  void push_front(const value_type& value) { this->emplace_front(value); }
  void push_front(value_type&& value) {
    this->emplace_front(std::forward<value_type>(value));
  }
  template <typename... Args>
  reference emplace_front(Args&&... args) {
    _NodeBase* _node = _Base::__get(_impl, std::forward<Args>(args)...);
    ++_impl._header._size;
    return *iterator(_node->__hook(_impl._header._next));
  }
  void pop_front() {
    _Base::
        __put(_impl, static_cast<_NodeType*>(_impl._header._next->__unhook()));
    --_impl._header._size;
  }
  void resize(size_type count) {
    if (_impl._header._size < count) {
      ginshio::stl::
          __check_length_error("list::resize", count, _Base::__max_size());
      return list::__fill_n(_impl, this->cend(), count - _impl._header._size);
    } else if (count < _impl._header._size) {
      _Base::__clear_aux(_impl, count);
    }
  }
  void resize(size_type count, const value_type& value) {
    if (_impl._header._size < count) {
      ginshio::stl::
          __check_length_error("list::resize", count, _Base::__max_size());
      return list::__fill_n(_impl, this->cend(),
                            count - _impl._header._size, value);
    } else if (count < _impl._header._size) {
      _Base::__clear_aux(_impl, count);
    }
  }
  void swap(list& other)
      noexcept(_NodeAllocTraits::propagate_on_container_swap::value ||
               _NodeAllocTraits::is_always_equal::value) {
    if (this == &other) {
      return;
    }
    _Base::__swap_allocator(_impl, other._impl);
    _impl.__swap(other._impl);
  }

  /////////////// operation ///////////////
 public:
  void merge(list& other) { this->merge(std::move(other)); }
  void merge(list&& other);
  template <typename Compare>
  void merge(list& other, Compare comp) { this->merge(std::move(other), comp); }
  template <typename Compare>
  void merge(list&& other, Compare comp);
  void splice(const_iterator pos, list& other) {
    this->splice(pos, std::move(other));
  }
  void splice(const_iterator pos, list&& other) {
    if (this == &other || this->get_allocator() != other.get_allocator() ||
        other._impl._header._size == 0) {
      return;
    }
    pos._node->__transfer(other._impl._header._next,
                          static_cast<_NodeBase*>(&other._impl._header));
    _impl._header._size += other._impl._header._size;
    other._impl._header._size = 0;
  }
  void splice(const_iterator pos, list& other, const_iterator it) {
    this->splice(pos, std::move(other), it);
  }
  void splice(const_iterator pos, list&& other, const_iterator it) {
    if (this->get_allocator() != other.get_allocator() ||
        other._impl._header._size == 0 ||
        it._node == pos._node || it._node->_next == pos._node) {
      return;
    }
    pos._node->__transfer(it._node, it._node->_next);
    ++_impl._header._size;
    --other._impl._header._size;
  }
  void splice(const_iterator pos, list& other,
              const_iterator first, const_iterator last) {
    this->splice(pos, std::move(other), first, last);
  }
  void splice(const_iterator pos, list&& other,
              const_iterator first, const_iterator last) {
    if (first == last || this->get_allocator() != other.get_allocator() ||
        other._impl._header._size == 0) {
      return;
    }
    size_type _len = std::distance(first, last);
    pos._node->__transfer(first._node, last._node);
    _impl._header._size += _len;
    other._impl._header._size -= _len;
  }
  size_type remove(const value_type& value);
  template <typename UnaryPredicate> size_type remove_if(UnaryPredicate p);
  void reverse() noexcept;
  size_type unique();
  template <typename BinaryPredicate> size_type unique(BinaryPredicate p);
  void sort();
  template <typename Compare> void sort(Compare comp);

  /////////////// assign ///////////////
 private:
  template <typename _InputIt>
  void __assign_range_dispatch(_InputIt _first, _InputIt _last,
                               std::input_iterator_tag);
  template <typename _RandomIt>
  void __assign_range_dispatch(_RandomIt _first, _RandomIt _last,
                               std::random_access_iterator_tag);

  /////////////// constructor aux ///////////////
 private:
  template <typename... Args>
  static void __fill_n(_BaseImpl& _impl, const_iterator _next,
                       const size_type& _count, Args&&... args);
  template <typename _InputIt>
  static void __copy(_BaseImpl& _impl, const_iterator _next,
                     _InputIt _first, _InputIt _last, std::input_iterator_tag);
  template <typename _RandomIt>
  static void __copy(_BaseImpl& _impl, const_iterator _next,
                     _RandomIt _first, _RandomIt _last,
                     std::random_access_iterator_tag) {
    list::__copy_n(_impl, _next, _first, _last - _first);
  }
  template <typename _InputIt>
  static void __copy_n(_BaseImpl& _impl, const_iterator _next,
                       _InputIt _first, const size_type& _cnt);
};



///////////////////////// java style iterator /////////////////////////
template <typename T>
using ListIterator = __container_base::_ListIterator<T, T*, T&>;
template <typename T>
using ListConstIterator =
    __container_base::_ListIterator<T, const T*, const T&>;



///////////////////////// list comparison operators /////////////////////////
template <typename T, typename Allocator>
constexpr bool operator==(const list<T, Allocator>& lhs,
                          const list<T, Allocator>& rhs) {
  return &lhs == &rhs || (lhs.size() == rhs.size() &&
                          std::equal(lhs.begin(), lhs.end(), rhs.begin()));
}
template <typename T, typename Allocator>
constexpr bool operator!=(const list<T, Allocator>& lhs,
                          const list<T, Allocator>& rhs) {
  return &lhs != &rhs && (lhs.size() != rhs.size() ||
                          std::equal(lhs.begin(), lhs.end(), rhs.begin()));
}
template <typename T, typename Allocator>
constexpr bool operator<(const list<T, Allocator>& lhs,
                         const list<T, Allocator>& rhs) {
  return std::lexicographical_compare(lhs.begin(), lhs.end(),
                                      rhs.begin(), rhs.end());
}
template <typename T, typename Allocator>
constexpr bool operator>(const list<T, Allocator>& lhs,
                         const list<T, Allocator>& rhs) {
  return std::lexicographical_compare(rhs.begin(), rhs.end(),
                                      lhs.begin(), lhs.end());
}
template <typename T, typename Allocator>
constexpr bool operator<=(const list<T, Allocator>& lhs,
                          const list<T, Allocator>& rhs) {
  return !operator<(rhs, lhs);
}
template <typename T, typename Allocator>
constexpr bool operator>=(const list<T, Allocator>& lhs,
                          const list<T, Allocator>& rhs) {
  return !operator<(lhs, rhs);
}



///////////////////////// specialization /////////////////////////
template <typename T, typename Allocator>
inline void swap(list<T, Allocator>& lhs, list<T, Allocator>& rhs) {
  lhs.swap(rhs);
}

template <typename T, typename Allocator, typename U>
inline auto erase(list<T, Allocator>& l, const U& value)
    -> typename list<T, Allocator>::size_type {
  return l.remove(value);
}
template <typename T, typename Allocator, typename Pred>
inline auto erase_if(list<T, Allocator>& l, Pred pred)
    -> typename list<T, Allocator>::size_type {
  return l.remove_if(pred);
}

} // namespace stl
} // namespace ginshio



namespace std {
///////////////////////// specialization /////////////////////////
template <typename T, typename Allocator>
inline void swap(ginshio::stl::list<T, Allocator>& lhs,
                 ginshio::stl::list<T, Allocator>& rhs) {
  lhs.swap(rhs);
}

template <typename T, typename Allocator, typename U>
inline auto erase(ginshio::stl::list<T, Allocator>& l, const U& value)
    -> typename ginshio::stl::list<T, Allocator>::size_type {
  return l.remove(value);
}
template <typename T, typename Allocator, typename Pred>
inline auto erase_if(ginshio::stl::list<T, Allocator>& l, Pred pred)
    -> typename ginshio::stl::list<T, Allocator>::size_type {
  return l.remove_if(pred);
}
} // namespace std

#endif // GINSHIO_STL__STL_LIST_HH_
