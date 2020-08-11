#ifndef GINSHIO_STL__STL_FORWARD_LIST_HH_
#define GINSHIO_STL__STL_FORWARD_LIST_HH_ 1

namespace ginshio {
namespace stl {

namespace __container_base {
///////////////////////// forward_list node /////////////////////////
struct _FwdListNodeBase {
  _FwdListNodeBase* _next;
  void __transfer_after(_FwdListNodeBase* _begin, _FwdListNodeBase* _end) {
    _FwdListNodeBase* _start = _begin->_next;
    if (_end == nullptr) {
      _begin->_next = nullptr;
    } else {
      _begin->_next = _end->_next;
      _end->_next = this->_next;
    }
    this->_next = _start;
  }
  _FwdListNodeBase* __hook(_FwdListNodeBase* _prev) {
    this->_next = _prev->_next;
    _prev->_next = this;
    return this;
  }
  _FwdListNodeBase* __unhook(_FwdListNodeBase* _prev) {
    _prev->_next = this->_next;
    return this;
  }
};

struct _FwdListNodeHeader : public _FwdListNodeBase {
  std::size_t _size;
  _FwdListNodeHeader() { this->__init_default(); }
  _FwdListNodeHeader(_FwdListNodeHeader&& _header) noexcept {
    this->__init_other(_header);
    _header.__init_default();
  }
  _FwdListNodeHeader& operator=(_FwdListNodeHeader&& _header) {
    this->__init_other(_header);
    _header.__init_default();
    return *this;
  }
  void __init_default() {
    this->_FwdListNodeBase::_next = nullptr;
    this->_size = 0;
  }
  void __init_other(_FwdListNodeHeader& _other) {
    this->_FwdListNodeBase::_next = _other._FwdListNodeBase::_next;
    this->_size = _other._size;
  }
};


template <typename _T>
struct _FwdListNode : public _FwdListNodeBase {
  _T _data;
  _T* __addr() noexcept { return std::addressof(this->_data); }
  constexpr const _T* __addr() const noexcept {
    return std::addressof(this->_data);
  }
};



///////////////////////// forward_list iterator /////////////////////////
template <typename _T, typename _Ptr, typename _Ref>
struct _FwdListIterator {
  /////////////// traits type ///////////////
 public:
  using difference_type = std::ptrdiff_t;
  using value_type = _T;
  using pointer = _Ptr;
  using reference = _Ref;
  using iterator_category = std::forward_iterator_tag;

  /////////////// iterator type ///////////////
 public:
  using _Iterator = _FwdListIterator<_T, _T*, _T&>;
  using _ConstIterator = _FwdListIterator<_T, const _T*, const _T&>;
  using _SelfIterator = _FwdListIterator<_T, _Ptr, _Ref>;
  using _OtherIterator =
      typename std::conditional<std::is_same<_SelfIterator, _Iterator>::value,
                                _ConstIterator, _Iterator>::type;

  /////////////// node type ///////////////
 public:
  using _NodeBase = _FwdListNodeBase;
  using _NodeType = _FwdListNode<_T>;

  /////////////// data member ///////////////
 public:
  _NodeBase* _node;

  /////////////// constructor ///////////////
 public:
  _FwdListIterator() : _node(nullptr) {}
  explicit _FwdListIterator(_NodeBase* _n) : _node(_n) {}
  _FwdListIterator(const _SelfIterator&) = default;
  // @function _FwdListIterator: const_iterator is constructed from iterator
  template <typename _Iter, typename = typename
            std::enable_if<std::is_same<_SelfIterator, _ConstIterator>::value &&
                           std::is_same<_OtherIterator, _Iter>::value>::type*>
  _FwdListIterator(const _Iter& _it) : _node(_it._node) {}

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
};
///////////////////////// iterator comparison operator /////////////////////////
template <typename _T, typename _Ptr, typename _Ref>
constexpr bool operator==(
    const _FwdListIterator<_T, _Ptr, _Ref>& _lit,
    const _FwdListIterator<_T, _Ptr, _Ref>& _rit) noexcept {
  return _lit._node == _rit._node;
}
template <typename _T, typename _PtrL, typename _RefL,
          typename _PtrR, typename _RefR>
constexpr bool operator==(
    const _FwdListIterator<_T, _PtrL, _RefL>& _lit,
    const _FwdListIterator<_T, _PtrR, _RefR>& _rit) noexcept {
  return _lit._node == _rit._node;
}
template <typename _T, typename _Ptr, typename _Ref>
constexpr bool operator!=(
    const _FwdListIterator<_T, _Ptr, _Ref>& _lit,
    const _FwdListIterator<_T, _Ptr, _Ref>& _rit) noexcept {
  return _lit._node != _rit._node;
}
template <typename _T, typename _PtrL, typename _RefL,
          typename _PtrR, typename _RefR>
constexpr bool operator!=(
    const _FwdListIterator<_T, _PtrL, _RefL>& _lit,
    const _FwdListIterator<_T, _PtrR, _RefR>& _rit) noexcept {
  return _lit._node != _rit._node;
}



///////////////////////// forward_list base /////////////////////////
template <typename _T, typename _Allocator>
struct _FwdListBase {
  /////////////// member type ///////////////
 protected:
  using _ValueType = _T;
  using _DataAllocType = _Allocator;
  using _DataAllocTraits = std::allocator_traits<_DataAllocType>;
  using _NodeBase = _FwdListNodeBase;
  using _NodeType = _FwdListNode<_ValueType>;
  using _NodeAllocType =
      typename _DataAllocTraits::template rebind_alloc<_NodeType>;
  using _NodeAllocTraits =
      typename _DataAllocTraits::template rebind_traits<_NodeType>;
  using _Iterator = _FwdListIterator<_ValueType, _ValueType*, _ValueType&>;
  using _ConstIterator =
      _FwdListIterator<_ValueType, const _ValueType*, const _ValueType&>;

  /////////////// implement ///////////////
 protected:
  struct _FwdListImpl : public _NodeAllocType {
    _FwdListNodeHeader _header;
    _NodeBase* _finish;
    _FwdListImpl() { _finish = static_cast<_NodeBase*>(&_header); }
    _FwdListImpl(const _NodeAllocType& _alloc) : _NodeAllocType(_alloc) {
      _finish = static_cast<_NodeBase*>(&_header);
    }
    void __swap(_FwdListImpl& _impl) {
      std::swap(_impl._header, _header);
      std::swap(_impl._finish, _finish);
      if (_impl._header._size == 0) {
        _impl._finish = static_cast<_NodeBase*>(&_impl._header);
      }
      if (_header._size == 0) {
        _finish = static_cast<_NodeBase*>(&_header);
      }
    }
  };

  /////////////// data member ///////////////
 protected:
  _FwdListImpl _impl;

  /////////////// constructor ///////////////
 public:
  _FwdListBase() = default;
  _FwdListBase(const _NodeAllocType& _alloc) : _impl(_alloc) {}
  _FwdListBase(_FwdListBase&& _other) noexcept :
      _impl(std::move(_other._impl)) {}
  ~_FwdListBase() noexcept { _FwdListBase::__clear(_impl); }

  /////////////// utils function ///////////////
 protected:
  static void __clear_aux(_FwdListImpl& _impl,
                          _NodeBase* _start_before, const std::size_t& _count);
  static void __clear(_FwdListImpl& _impl) {
    if (_impl._header._size == 0) {
      return;
    }
    _FwdListBase::
        __clear_aux(_impl, static_cast<_NodeBase*>(&_impl._header), 0);
    _impl._finish = static_cast<_NodeBase*>(&_impl._header);
  }
  static void __swap_allocator(_NodeAllocType& _a, _NodeAllocType& _b) {
    std::swap(_a, _b);
  }
  static constexpr std::size_t __max_size() noexcept {
    return std::numeric_limits<std::ptrdiff_t>::max() / sizeof(_NodeType);
  }
  static _NodeBase* __get_before_node(_FwdListImpl& _impl, _NodeBase* _node) {
    if (_impl._header._next == _node) {
      return static_cast<_NodeBase*>(&_impl._header);
    }
    _NodeBase* _cur = _impl._header._next;
    while (_cur->_next != _node) {
      _cur = _cur->_next;
    }
    return _cur;
  }

  /////////////// get and put node ///////////////
 protected:
  template <typename... Args>
  static _NodeBase* __get(_FwdListImpl& _impl, Args&&... args) {
    _NodeType* _node = _NodeAllocTraits::allocate(_impl, 1);
    try {
      ginshio::stl::construct(_node->__addr(), std::forward<Args>(args)...);
    } catch (...) {
      _NodeAllocTraits::deallocate(_impl, _node, 1);
      throw;
    }
    _node->_NodeBase::_next = nullptr;
    return static_cast<_NodeBase*>(_node);
  }
  static void __put(_FwdListImpl& _impl, _NodeType* _node) {
    ginshio::stl::destroy(_node->__addr());
    _NodeAllocTraits::deallocate(_impl, _node, 1);
  }
};
} // namespace __container_base



///////////////////////// forward_list /////////////////////////
template <typename T, typename Allocator = std::allocator<T>>
class forward_list : protected __container_base::_FwdListBase<T, Allocator> {
  /////////////// private type ///////////////
 private:
  using _Base = __container_base::_FwdListBase<T, Allocator>;
  using _BaseImpl = typename _Base::_FwdListImpl;
  using _DataAllocTraits = typename _Base::_DataAllocTraits;
  using _NodeBase = typename _Base::_NodeBase;
  using _NodeHeader = __container_base::_FwdListNodeHeader;
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

  /////////////// data member ///////////////
 private:
  using _Base::_impl;

  /////////////// constructor ///////////////
 public:
  forward_list() = default;
  explicit forward_list(const allocator_type& alloc) :
      _Base(_NodeAllocType(alloc)) {}
  explicit forward_list(size_type count, const value_type& value,
                        const allocator_type& alloc = allocator_type()) :
      _Base(_NodeAllocType(alloc)) {
    ginshio::stl::
        __check_length_error("forward_list", count, _Base::__max_size());
    _impl._finish = forward_list::
        __fill_n(_impl, this->cbefore_begin(), count, value);
  }
  explicit forward_list(size_type count,
                        const allocator_type& alloc = allocator_type()) :
      _Base(_NodeAllocType(alloc)) {
    ginshio::stl::
        __check_length_error("forward_list", count, _Base::__max_size());
    _impl._finish = forward_list::__fill_n(_impl, this->cbefore_begin(), count);
  }
  template <typename InputIt, typename = typename
            std::enable_if<std::is_base_of<
                             std::input_iterator_tag, typename
                             std::iterator_traits<InputIt>::iterator_category>::
                           value>::type*>
  forward_list(InputIt first, InputIt last,
               const allocator_type& alloc = allocator_type()) :
      _Base(_NodeAllocType(alloc)) {
    using _Category = typename std::iterator_traits<InputIt>::iterator_category;
    _impl._finish = forward_list::
        __copy(_impl, this->cbefore_begin(), first, last, _Category());
  }
  forward_list(const forward_list& other) : _Base(_NodeAllocType(
      _DataAllocTraits::
      select_on_container_copy_construction(other.get_allocator()))) {
    _impl._finish = forward_list::
        __copy_n(_impl, this->cbefore_begin(), other.begin(), other.size());
  }
  forward_list(const forward_list& other, const allocator_type& alloc) :
      _Base(_NodeAllocType(alloc)) {
    _impl._finish = forward_list::
        __copy_n(_impl, this->cbefore_begin(), other.begin(), other.size());
  }
  forward_list(forward_list&& other) noexcept = default;
  forward_list(forward_list&& other, const allocator_type& alloc) :
      _Base(_NodeAllocType(alloc)) {
    if (alloc == other.get_allocator()) {
      _impl.__swap(other._impl);
      return;
    }
    _impl._finish = forward_list::
        __copy_n(_impl, this->cbefore_begin(), other.begin(), other.size());
  }
  forward_list(std::initializer_list<value_type> ilist,
               const allocator_type& alloc = allocator_type()) :
      _Base(_NodeAllocType(alloc)) {
    _impl._finish = forward_list::
        __copy_n(_impl, this->cbefore_begin(), ilist.begin(), ilist.size());
  }

  /////////////// destructor ///////////////
 public:
  ~forward_list() noexcept = default;

  /////////////// member function ///////////////
 public:
  void assign(size_type count, const value_type& value);
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
  forward_list& operator=(const forward_list& other) {
    if (this == &other) {
      return *this;
    }
    if (_DataAllocTraits::propagate_on_container_copy_assignment::value) {
      if (other.get_allocator() != this->get_allocator()) {
        _Base::__clear(_impl);
      }
      _impl = static_cast<_NodeAllocType>(other._impl);
    }
    this->__assign_aux(other.begin(), other.end(), std::forward_iterator_tag());
    return *this;
  }
  forward_list& operator=(forward_list&& other) {
    if (this->get_allocator() == other.get_allocator()) {
      _Base::__clear(_impl);
      _impl.__swap(other._impl);
      return *this;
    }
    if (_DataAllocTraits::propagate_on_container_move_assignment::value) {
      _Base::__clear(_impl);
      _impl.__swap(other._impl);
      _impl = std::move(static_cast<allocator_type>(other._impl));
    } else {
      this->__assign_aux(std::move_iterator<iterator>(other.begin()),
                         std::move_iterator<iterator>(other.end()),
                         std::forward_iterator_tag());
    }
    return *this;
  }
  forward_list& operator=(std::initializer_list<value_type> ilist) {
    this->__assign_aux(ilist.begin(), ilist.end(),
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
  reference back() { return *iterator(_impl._finish); }
  constexpr const_reference back() const { return *iterator(_impl._finish); }

  /////////////// iterator ///////////////
 public:
  iterator before_begin() noexcept { return iterator(&_impl._header); }
  constexpr const_iterator before_begin() const noexcept {
    return const_iterator(const_cast<_NodeHeader*>(&_impl._header));
  }
  constexpr const_iterator cbefore_begin() const noexcept {
    return const_iterator(const_cast<_NodeHeader*>(&_impl._header));
  }
  iterator begin() noexcept { return iterator(_impl._header._next); }
  constexpr const_iterator begin() const noexcept {
    return const_iterator(_impl._header._next);
  }
  constexpr const_iterator cbegin() const noexcept {
    return const_iterator(_impl._header._next);
  }
  iterator before_end() noexcept { return iterator(_impl._finish); }
  constexpr const_iterator before_end() const noexcept {
    return const_iterator(_impl._finish);
  }
  constexpr const_iterator cbefore_end() const noexcept {
    return const_iterator(_impl._finish);
  }
  iterator end() noexcept { return iterator(nullptr); }
  constexpr const_iterator end() const noexcept {
    return const_iterator(nullptr);
  }
  constexpr const_iterator cend() const noexcept {
    return const_iterator(nullptr);
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
  iterator insert_after(const_iterator pos, const value_type& value) {
    return this->emplace_after(pos, value);
  }
  iterator insert_after(const_iterator pos, value_type&& value) {
    return this->emplace_after(pos, std::forward<value_type>(value));
  }
  iterator insert_after(const_iterator pos,
                        size_type count, const value_type& value) {
    if (count != 0) {
      forward_list _tmp{count, value, this->get_allocator()};
      iterator _it = _tmp.begin();
      this->splice_after(pos, std::move(_tmp));
      return _it;
    }
    return iterator(pos._node);
  }
  template <typename InputIt, typename = typename
            std::enable_if<std::is_base_of<
                             std::input_iterator_tag, typename
                             std::iterator_traits<InputIt>::iterator_category>::
                           value>::type*>
  iterator insert_after(const_iterator pos, InputIt first, InputIt last) {
    forward_list _tmp{first, last, this->get_allocator()};
    if (!_tmp.empty()) {
      iterator _it = _tmp.begin();
      this->splice_after(pos, std::move(_tmp));
      return _it;
    }
    return iterator(pos._node);
  }
  iterator insert_after(const_iterator pos,
                        std::initializer_list<value_type> ilist) {
    if (ilist.size() != 0) {
      forward_list _tmp{ilist, this->get_allocator()};
      iterator _it = _tmp.begin();
      this->splice_after(pos, std::move(_tmp));
      return _it;
    }
    return iterator(pos._node);

  }
  template <typename... Args>
  iterator emplace_after(const_iterator pos, Args&&... args) {
    _NodeBase* _node = _Base::__get(_impl, std::forward<Args>(args)...);
    ++_impl._header._size;
    if (pos._node == _impl._finish) {
      _impl._finish = _node;
    }
    return iterator(_node->__hook(pos._node));
  }
  iterator erase_after(const_iterator pos) {
    if (pos._node == nullptr || pos._node == _impl._finish) {
      return iterator(nullptr);
    }
    if (pos._node->_next == _impl._finish) {
      _impl._finish = pos._node;
    }
    _Base::__put(
        _impl, static_cast<_NodeType*>(pos._node->_next->__unhook(pos._node)));
    --_impl._header._size;
    return iterator(pos._node->_next);
  }
  iterator erase_after(const_iterator first, const_iterator last) {
    _NodeBase* _cur = first._node->_next;
    for (_NodeBase* _next; _cur != last._node; _cur = _next) {
      _next = _cur->_next;
      _Base::__put(_impl, static_cast<_NodeType*>(_cur));
      --_impl._header._size;
    }
    first._node->_next = last._node;
    if (last._node == nullptr) {
      _impl._finish = first._node;
    }
    return iterator(last._node);
  }
  void push_back(const value_type& value) { this->emplace_back(value); }
  void push_back(value_type&& value) {
    this->emplace_back(std::forward<value_type>(value));
  }
  template <typename... Args>
  reference emplace_back(Args&&... args) {
    _impl._finish =
        _Base::__get(_impl, std::forward<Args>(args)...)->__hook(_impl._finish);
    ++_impl._header._size;
    return *iterator(_impl._finish);
  }
  void pop_back() {
    _NodeBase* _before = _Base::__get_before_node(_impl, _impl._finish);
    _Base::__put(_impl, _impl._finish->__unhook(_before));
    _impl._finish = _before;
    --_impl._header._size;
  }
  void push_front(const value_type& value) { this->emplace_front(value); }
  void push_front(value_type&& value) {
    this->emplace_front(std::forward<value_type>(value));
  }
  template <typename... Args>
  reference emplace_front(Args&&... args) {
    _NodeBase* _node = _Base::__get(_impl, std::forward<Args>(args)...)->
        __hook(static_cast<_NodeBase*>(&_impl._header));
    if (_impl._header._size == 0) {
      _impl._finish = _node;
    }
    ++_impl._header._size;
    return *iterator(_impl._header._next);
  }
  void pop_front() {
    _Base::__put(_impl, _impl._header._next->
                 __unhook(static_cast<_NodeBase*>(&_impl._header)));
    if (_impl._header._size == 1) {
      _impl._finish = static_cast<_NodeBase*>(&_impl._header);
    }
    --_impl._header._size;
  }
  void resize(size_type count) {
    if (_impl._header._size < count) {
      ginshio::stl::__check_length_error("forward_list::resize",
                                         count, _Base::__max_size());
      _impl._finish = forward_list::
          __fill_n(_impl, const_iterator(_impl._finish),
                   count - _impl._header._size);
    } else if (count < _impl._header._size) {
      iterator _start_before = this->before_begin();
      std::advance(_start_before, count);
      _Base::__clear_aux(_impl, _start_before._node, count);
      _impl._finish = _start_before._node;
    }
  }
  void resize(size_type count, const value_type& value) {
    if (_impl._header._size < count) {
      ginshio::stl::__check_length_error("forward_list::resize",
                                         count, _Base::__max_size());
      _impl._finish = forward_list::
          __fill_n(_impl, const_iterator(_impl._finish),
                   count - _impl._header._size, value);
    } else if (count < _impl._header._size) {
      iterator _start_before = this->before_begin();
      std::advance(_start_before, count);
      _Base::__clear_aux(_impl, _start_before._node, count);
      _impl._finish = _start_before._node;
    }
  }
  void swap(forward_list& other)
      noexcept(_NodeAllocTraits::is_always_equal::value) {
    if (this == &other) {
      return;
    }
    _Base::__swap_allocator(_impl, other._impl);
    _impl.__swap(other._impl);
  }

  /////////////// operation ///////////////
 public:
  void merge(forward_list& other) { this->merge(std::move(other)); }
  void merge(forward_list&& other);
  template <typename Compare>
  void merge(forward_list& other, Compare comp) {
    this->merge(std::move(other), comp);
  }
  template <typename Compare> void merge(forward_list&& other, Compare comp);
  void splice_after(const_iterator pos, forward_list& other) {
    this->splice_after(pos, std::move(other));
  }
  void splice_after(const_iterator pos, forward_list&& other) {
    if (this == &other || this->get_allocator() != other.get_allocator() ||
        other._impl._header._size == 0) {
      return;
    }
    pos._node->__transfer_after(static_cast<_NodeBase*>(&other._impl._header),
                                other._impl._finish);
    if (pos._node == _impl._finish) {
      _impl._finish = other._impl._finish;
    }
    _impl._header._size += other._impl._header._size;
    other._impl._header._size = 0;
    other._impl._finish = static_cast<_NodeBase*>(&other._impl._header);
  }
  void splice_after(const_iterator pos,
                    forward_list& other, const_iterator it) {
    this->splice_after(pos, std::move(other), it);
  }
  void splice_after(const_iterator pos,
                    forward_list&& other, const_iterator it) {
    if (this->get_allocator() != other.get_allocator() ||
        other._impl._header._size == 0 ||
        it._node == pos._node || it._node == pos._node->_next ||
        it._node == other._impl._finish) {
      return;
    }
    if (it._node->_next == other._impl._finish) {
      other._impl._finish = it._node;
    }
    pos._node->__transfer_after(it._node, it._node->_next);
    if (pos._node == _impl._finish) {
      _impl._finish = _impl._finish->_next;
    }
    ++_impl._header._size;
    --other._impl._header._size;
  }
  void splice_after(const_iterator pos, forward_list& other,
                    const_iterator first, const_iterator last) {
    this->splice_after(pos, std::move(other), first, last);
  }
  void splice_after(const_iterator pos, forward_list&& other,
                    const_iterator first, const_iterator last);
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
  void __assign_aux(_InputIt _first, _InputIt _last, std::input_iterator_tag);
  template <typename _RandomIt>
  void __assign_aux(_RandomIt _first, _RandomIt _last,
                    std::random_access_iterator_tag);

  /////////////// constructor aux ///////////////
 private:
  template <typename... Args>
  static _NodeBase* __fill_n(_BaseImpl& _impl, const_iterator _prev,
                             const size_type& _count, Args&&... args);
  template <typename _InputIt>
  static _NodeBase* __copy(_BaseImpl& _impl, const_iterator _prev,
                           _InputIt _first, _InputIt _last,
                           std::input_iterator_tag);
  template <typename _RandomIt>
  static _NodeBase* __copy(_BaseImpl& _impl, const_iterator _prev,
                           _RandomIt _first, _RandomIt _last,
                           std::random_access_iterator_tag) {
    return forward_list::__copy_n(_impl, _prev, _first, _last - _first);
  }
  template <typename _InputIt>
  static _NodeBase* __copy_n(_BaseImpl& _impl, const_iterator _prev,
                             _InputIt _first, const size_type& _cnt);
};



///////////////////////// java style iterator /////////////////////////
template <typename T>
using ForwardListIterator = __container_base::_FwdListIterator<T, T*, T&>;
template <typename T>
using ForwardListConstIterator =
    __container_base::_FwdListIterator<T, const T*, const T&>;



///////////////////////// fwdlist comparison operators /////////////////////////
template <typename T, typename Allocator>
constexpr bool operator==(const forward_list<T, Allocator>& lhs,
                          const forward_list<T, Allocator>& rhs) {
  return &lhs == &rhs || (lhs.size() == rhs.size() &&
                          std::equal(lhs.begin(), lhs.end(), rhs.begin()));
}
template <typename T, typename Allocator>
constexpr bool operator!=(const forward_list<T, Allocator>& lhs,
                          const forward_list<T, Allocator>& rhs) {
  return &lhs != &rhs && (lhs.size() != rhs.size() ||
                          std::equal(lhs.begin(), lhs.end(), rhs.begin()));
}
template <typename T, typename Allocator>
constexpr bool operator<(const forward_list<T, Allocator>& lhs,
                         const forward_list<T, Allocator>& rhs) {
  return std::lexicographical_compare(lhs.begin(), lhs.end(),
                                      rhs.begin(), rhs.end());
}
template <typename T, typename Allocator>
constexpr bool operator>(const forward_list<T, Allocator>& lhs,
                         const forward_list<T, Allocator>& rhs) {
  return std::lexicographical_compare(rhs.begin(), rhs.end(),
                                      lhs.begin(), lhs.end());
}
template <typename T, typename Allocator>
constexpr bool operator<=(const forward_list<T, Allocator>& lhs,
                          const forward_list<T, Allocator>& rhs) {
  return !operator<(rhs, lhs);
}
template <typename T, typename Allocator>
constexpr bool operator>=(const forward_list<T, Allocator>& lhs,
                          const forward_list<T, Allocator>& rhs) {
  return !operator<(lhs, rhs);
}



///////////////////////// specialization /////////////////////////
template <typename T, typename Allocator>
inline void swap(forward_list<T, Allocator>& lhs,
                 forward_list<T, Allocator>& rhs) {
  lhs.swap(rhs);
}

template <typename T, typename Allocator, typename U>
inline auto erase(forward_list<T, Allocator>& l, const U& value)
    -> typename forward_list<T, Allocator>::size_type {
  return l.remove(value);
}
template <typename T, typename Allocator, typename Pred>
inline auto erase_if(forward_list<T, Allocator>& l, Pred pred)
    -> typename forward_list<T, Allocator>::size_type {
  return l.remove_if(pred);
}

} // namespace stl
} // namespace ginshio



namespace std {
///////////////////////// specialization /////////////////////////
template <typename T, typename Allocator>
inline void swap(ginshio::stl::forward_list<T, Allocator>& lhs,
                 ginshio::stl::forward_list<T, Allocator>& rhs) {
  lhs.swap(rhs);
}

template <typename T, typename Allocator, typename U>
inline auto erase(ginshio::stl::forward_list<T, Allocator>& l, const U& value)
    -> typename ginshio::stl::forward_list<T, Allocator>::size_type {
  return l.remove(value);
}
template <typename T, typename Allocator, typename Pred>
inline auto erase_if(ginshio::stl::forward_list<T, Allocator>& l, Pred pred)
    -> typename ginshio::stl::forward_list<T, Allocator>::size_type {
  return l.remove_if(pred);
}
} // namespace std

#endif // GINSHIO_STL__STL_FORWARD_LIST_HH_
