#ifndef GINSHIO_STL__STL_RB_TREE_HH_
#define GINSHIO_STL__STL_RB_TREE_HH_ 1

#include "base/stl_init.hh"
#include "base/stl_tree_algo.hh"

#include <initializer_list>
#include <limits>
#include <memory>
#include <utility>

#include <cstddef>

namespace ginshio {
namespace stl {

namespace __container_base {
///////////////////////// red black tree node /////////////////////////
struct _RBTreeNodeBase {
  _RBTreeColor _tag;
  _RBTreeNodeBase* _left;
  _RBTreeNodeBase* _right;
  _RBTreeNodeBase* _parent;
  _RBTreeNodeBase() : _tag(_RBTreeColor_RED),
                      _left(nullptr), _right(nullptr), _parent(nullptr) {}
  /////////////// color ///////////////
  constexpr const bool is_black() const {
    return this->_tag == _RBTreeColor_BLACK;
  }
  void set_color(const _RBTreeColor c) {
    this->_tag = c;
  }
};

struct _RBTreeNodeHeader : public _RBTreeNodeBase {
  std::size_t _size;
  _RBTreeNodeHeader() { this->__init_default(); }
  _RBTreeNodeHeader(_RBTreeNodeHeader&& _header) noexcept {
    if (_header._size == 0) {
      this->__init_default();
    } else {
      this->__init_other(_header);
      _header.__init_default();
    }
  }
  _RBTreeNodeHeader& operator=(_RBTreeNodeHeader&& _header) {
    if (_header._size == 0) {
      this->__init_default();
    } else {
      this->__init_other(_header);
      _header.__init_default();
    }
    return *this;
  }
  void __init_default() {
    this->_parent = nullptr;
    this->_left = this->_right = this;
    this->_size = 0;
  }
  void __init_other(_RBTreeNodeHeader& _other) {
    this->_left = _other._left;
    this->_right = _other._right;
    this->_parent = _other._parent;
    this->_size = _other._size;
  }
};

template <typename _T>
struct _RBTreeNode : public _RBTreeNodeBase {
  _T _data;
  _T* __addr() noexcept { return std::addressof(this->_data); }
  constexpr const _T* __addr() const noexcept {
    return std::addressof(this->_data);
  }
};



///////////////////////// red black tree iterator /////////////////////////
template <typename _T, typename _Ptr, typename _Ref>
struct _RBTreeIterator {
  /////////////// traits type ///////////////
 public:
  using difference_type = std::ptrdiff_t;
  using value_type = _T;
  using pointer = _Ptr;
  using reference = _Ref;
  using iterator_category = std::bidirectional_iterator_tag;

  /////////////// iterator type ///////////////
 public:
  using _Iterator = _RBTreeIterator<_T, _T*, _T&>;
  using _ConstIterator = _RBTreeIterator<_T, const _T*, const _T&>;
  using _SelfIterator = _RBTreeIterator<_T, _Ptr, _Ref>;
  using _OtherIterator =
      typename std::conditional<std::is_same<_SelfIterator, _Iterator>::value,
                                _ConstIterator, _Iterator>::type;

  /////////////// node type ///////////////
 public:
  using _NodeBase = _RBTreeNodeBase;
  using _NodeType = _RBTreeNode<_T>;

  /////////////// data member ///////////////
 public:
  _NodeBase* _node;

  /////////////// constructor ///////////////
 public:
  _RBTreeIterator() : _node(nullptr) {}
  explicit _RBTreeIterator(_NodeBase* _n) : _node(_n) {}
  _RBTreeIterator(const _SelfIterator&) = default;
  template <typename _Iter, typename = typename
            std::enable_if<std::is_same<_SelfIterator, _ConstIterator>::value &&
                           std::is_same<_OtherIterator, _Iter>::value>::type*>
  _RBTreeIterator(const _Iter& _it) : _node(_it._node) {}

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
    this->_node = tree::__node_increment(this->_node);
    return *this;
  }
  _SelfIterator operator++(int) {
    _SelfIterator _tmp = *this;
    this->_node = tree::__node_increment(this->_node);
    return *_tmp;
  }
  _SelfIterator& operator--() {
    this->_node = tree::__node_decrement(this->_node);
    return *this;
  }
  _SelfIterator& operator--(int) {
    _SelfIterator _tmp = *this;
    this->_node = tree::__node_decrement(this->_node);
    return _tmp;
  }
};
///////////////////////// iterator comparison operator /////////////////////////
template <typename _T, typename _Ptr, typename _Ref>
constexpr bool operator==(
    const _RBTreeIterator<_T, _Ptr, _Ref>& _lit,
    const _RBTreeIterator<_T, _Ptr, _Ref>& _rit) noexcept {
  return _lit._node == _rit._node;
}
template <typename _T, typename _PtrL, typename _RefL,
          typename _PtrR, typename _RefR>
constexpr bool operator==(
    const _RBTreeIterator<_T, _PtrL, _RefL>& _lit,
    const _RBTreeIterator<_T, _PtrR, _RefR>& _rit) noexcept {
  return _lit._node == _rit._node;
}
template <typename _T, typename _Ptr, typename _Ref>
constexpr bool operator!=(
    const _RBTreeIterator<_T, _Ptr, _Ref>& _lit,
    const _RBTreeIterator<_T, _Ptr, _Ref>& _rit) noexcept {
  return _lit._node != _rit._node;
}
template <typename _T, typename _PtrL, typename _RefL,
          typename _PtrR, typename _RefR>
constexpr bool operator!=(
    const _RBTreeIterator<_T, _PtrL, _RefL>& _lit,
    const _RBTreeIterator<_T, _PtrR, _RefR>& _rit) noexcept {
  return _lit._node != _rit._node;
}



///////////////////////// red black tree base /////////////////////////
template <typename _T, typename _Allocator>
struct _RBTreeBase {
  /////////////// member type ///////////////
 protected:
  using _ValueType = _T;
  using _DataAllocType = _Allocator;
  using _DataAllocTraits = std::allocator_traits<_DataAllocType>;
  using _NodeBase = _RBTreeNodeBase;
  using _NodeType = _RBTreeNode<_ValueType>;
  using _NodeAllocType =
      typename _DataAllocTraits::template rebind_alloc<_NodeType>;
  using _NodeAllocTraits =
      typename _DataAllocTraits::template rebind_traits<_NodeType>;
  using _Iterator = _RBTreeIterator<_ValueType, _ValueType*, _ValueType&>;
  using _ConstIterator =
      _RBTreeIterator<_ValueType, const _ValueType*, const _ValueType&>;

  /////////////// implement ///////////////
 protected:
  struct _RBTreeImpl : public _NodeAllocType {
    _RBTreeNodeHeader _header;
    _RBTreeImpl() = default;
    _RBTreeImpl(const _NodeAllocType& _alloc) : _NodeAllocType(_alloc) {}
    void __swap(_RBTreeImpl& _impl) {
      std::swap(_impl._header, this->_header);
    }
  };

  /////////////// data member ///////////////
 protected:
  _RBTreeImpl _impl;

  /////////////// constructor ///////////////
 public:
  _RBTreeBase() = default;
  _RBTreeBase(const _NodeAllocType& _alloc) : _impl(_alloc) {}
  _RBTreeBase(_RBTreeBase&& _other) noexcept : _impl(std::move(_other._impl)) {}
  ~_RBTreeBase() { _RBTreeBase::__clear(_impl); }

  /////////////// utils function ///////////////
 protected:
  static void __clear_aux(_RBTreeImpl& _impl, _NodeBase* _node) {
    if (!_node || _node == _impl._header._nil) {
      return;
    }
    _RBTreeBase::__clear_aux(_impl, _node->_left);
    _RBTreeBase::__clear_aux(_impl, _node->_right);
    _RBTreeBase::__put(_impl, static_cast<_NodeType*>(_node));
  }
  static void __clear(_RBTreeImpl& _impl) {
    if (_impl._header._size == 0) {
      return;
    }
    _RBTreeBase::__clear_aux(_impl, _impl._header._root);
    _impl._header._init_default();
  }
  static void __swap_allocator(_NodeAllocType& _a, _NodeAllocType& _b) {
    std::swap(_a, _b);
  }
  static constexpr std::size_t __max_size() noexcept {
    return std::numeric_limits<std::ptrdiff_t>::max() / sizeof(_NodeType);
  }

  /////////////// get and put node ///////////////
 protected:
  template <typename... Args>
  static _NodeBase* __get(_RBTreeImpl& _impl, Args&&... args) {
    _NodeType* _node = _NodeAllocTraits::allocate(_impl, 1);
    try {
      ginshio::stl::construct(_node->__addr(), std::forward<Args>(args)...);
    } catch (...) {
      _NodeAllocTraits::deallocate(_impl, _node, 1);
      throw;
    }
    _node->_NodeBase::_parent =
        _node->_NodeBase::_left = _node->_NodeBase::_right = nullptr;
    return static_cast<_NodeBase*>(_node);
  }
  static void __put(_RBTreeImpl& _impl, _NodeType* _node) {
    ginshio::stl::destroy(_node->__addr());
    _NodeAllocTraits::deallocate(_impl, _node, 1);
  }
};
} // namespace __container_base



///////////////////////// red black tree /////////////////////////
template <typename T, typename Allocator = std::allocator<T>>
class rb_tree : protected __container_base::_RBTreeBase<T, Allocator> {
  /////////////// private type ///////////////
 private:
  using _Base = __container_base::_RBTreeBase<T, Allocator>;
  using _BaseImpl = typename _Base::_RBTreeImpl;
  using _DataAllocTraits = typename _Base::_DataAllocTraits;
  using _NodeBase = typename _Base::_NodeBase;
  using _NodeHeader = __container_base::_RBTreeNodeHeader;
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
  rb_tree() = default;
  explicit rb_tree(const allocator_type& alloc) :
      _Base(_NodeAllocType(alloc)) {}
  rb_tree(const rb_tree& other) : _Base(_NodeAllocType(
      _DataAllocTraits::
      select_on_container_copy_construction(other.get_allocator()))) {
    // TODO
  }
  rb_tree(const rb_tree& other, const allocator_type& alloc) :
      _Base(_NodeAllocType(alloc)) {
    // TODO
  }
  rb_tree(rb_tree&& other) noexcept = default;
  rb_tree(rb_tree&& other, const allocator_type& alloc) :
      _Base(_NodeAllocType(alloc)) {
    // TODO
  }
  rb_tree(std::initializer_list<value_type> ilist,
          const allocator_type& alloc = allocator_type()) :
      _Base(_NodeAllocType(alloc)) {
    // TODO
  }

  /////////////// destructor ///////////////
 public:
  ~rb_tree() noexcept = default;

  /////////////// element access ///////////////
 public:
  reference front() { return *iterator(_impl._header._left); }
  constexpr const_reference front() const {
    return *iterator(_impl._header._left);
  }
  reference back() { return *iterator(_impl._header._right); }
  constexpr const_reference back() const {
    return *iterator(_impl._header._right);
  }

    /////////////// iterator ///////////////
 public:
  iterator begin() noexcept { return iterator(_impl._header._left); }
  constexpr const_iterator begin() const noexcept {
    return const_iterator(_impl._header._left);
  }
  constexpr const_iterator cbegin() const noexcept {
    return const_iterator(_impl._header._left);
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
    return reverse_iterator(iterator(_impl._header._left));
  }
  constexpr const_reverse_iterator rend() const noexcept {
    return const_reverse_iterator(const_iterator(_impl._header._left));
  }
  constexpr const_reverse_iterator crend() const noexcept {
    return const_reverse_iterator(const_iterator(_impl._header._left));
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
  iterator insert_equal(const value_type& value) {
    return this->emplace_equal(value);
  }
  iterator insert_equal(value_type&& value) {
    return this->emplace_equal(std::forward<value_type>(value));
  }
  std::pair<iterator, bool> insert_unique(const value_type& value) {
    return this->emplace_unique(value);
  }
  std::pair<iterator, bool> insert_unique(value_type&& value) {
    return this->emplace_unique(std::forward<value_type>(value));
  }
  void swap(rb_tree& other)
      noexcept(_NodeAllocTraits::propagate_on_container_swap::value ||
               _NodeAllocTraits::is_always_equal::value) {
    if (this == &other) {
      return;
    }
    _Base::__swap_allocator(_impl, other._impl);
    _impl.__swap(other._impl);
  }

  /////////////// TODO: insert ///////////////
 public:
  template <typename... Args>
  iterator emplace_equal(Args&&... args) {
    _NodeBase* _pa = _impl._header._parent;
    _NodeBase* _node;
    if (!_pa) {
      _node = _Base::__get(_impl, std::forward<Args>(args)...);
      _impl._header._parent =
          _impl._header._left = _impl._header._right = _node;
      _node->_parent = _pa;
      _node->_tag = _RBTreeColor_BLACK;
    } else {
      _NodeBase** _tmp;
      auto _val = value_type(std::forward<Args>(args)...);
      do {
        _tmp = _val < static_cast<_NodeType*>(_pa)->_data ?
               &_pa->_left : &_pa->_right;
      } while (*_tmp && (_pa = *_tmp));
      _node = _Base::__get(_impl, std::move(_val));
      _node->_parent = _pa;
      *_tmp = _node;
      __insert(_node, static_cast<_NodeBase*>(&_impl._header));
    }
    ++_impl._header._size;
    return iterator(_node);
  }
  template <typename... Args>
  std::pair<iterator, bool> emplace_unique(Args&&... args) {
    _NodeBase* _pa = _impl._header._parent;
    _NodeBase* _node;
    if (!_pa) {
      _node = _Base::__get(_impl, std::forward<Args>(args)...);
      _impl._header._parent =
          _impl._header._left = _impl._header._right = _node;
      _node->_parent = _pa;
      _node->_tag = _RBTreeColor_BLACK;
    } else {
      _NodeBase** _tmp;
      auto _val = value_type(std::forward<Args>(args)...);
      do {
        _tmp = _val < static_cast<_NodeType*>(_pa)->_data ?
               &_pa->_left : &_pa->_right;
      } while (*_tmp && (_pa = *_tmp));
      if (_tmp == &_pa->_left && _pa != _impl._header._left) {
        auto _it = --iterator(_pa);
        if (!(static_cast<_NodeType*>(_it._node)->_data < _val)) {
          return {_it, false};
        }
      }
      _node = _Base::__get(_impl, std::move(_val));
      _node->_parent = _pa;
      *_tmp = _node;
      __insert(_node, static_cast<_NodeBase*>(&_impl._header));
    }
    ++_impl._header._size;
    return {iterator(_node), true};
  }
 private:
  static void __insert(_NodeBase* _node, _NodeBase* _header) {
    if (tree::__is_left(_node) & (_header->_left == _node->_parent)) {
      _header->_left = _node;
    } else if (tree::__is_right(_node) & (_header->_right == _node->_parent)) {
      _header->_right = _node;
    }
    _NodeBase* _uncle,* _gp;
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
      if (tree::__is_right(_node->_parent)) {
        if (tree::__is_left(_node)) {
          _node = _node->_parent;
          tree::__rotate_right(_node, _header->_parent);
        }
        _node->_parent->_tag = _RBTreeColor_BLACK;
        _gp->_parent->_tag = _RBTreeColor_RED;
        tree::__rotate_left(_gp, _header->_parent);
      } else {
        if (tree::__is_right(_node)) {
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

  /////////////// TODO: erase ///////////////
 public:
 private:
};

} // namespace stl
} // namespace ginshio

#endif // GINSHIO_STL__STL_RB_TREE_HH_
