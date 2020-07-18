#ifndef GINSHIO_STL__STL_RB_TREE_HH_
#define GINSHIO_STL__STL_RB_TREE_HH_ 1

#include "base/stl_init.hh"

#include <limits>
#include <memory>
#include <utility>

#include <cstddef>

namespace ginshio {
namespace stl {

namespace __container_base {

enum _RBTreeColor {RED, BLACK};

///////////////////////// red black tree node /////////////////////////
struct _RBTreeNodeBase {
  _RBTreeColor _color;
  _RBTreeNodeBase* _left;
  _RBTreeNodeBase* _right;
  _RBTreeNodeBase* _parent;
  /////////////// static ///////////////
  static constexpr _RBTreeNodeBase* __get_left(const _RBTreeNodeBase* _node) {
    return _node->_left;
  }
  static constexpr _RBTreeNodeBase* __get_right(const _RBTreeNodeBase* _node) {
    return _node->_right;
  }
  static constexpr _RBTreeNodeBase* __get_grandparent(
      const _RBTreeNodeBase* _node) {
    return _node->parent ? _node->parent->parent : nullptr;
  }
  static _RBTreeNodeBase* __get_uncle(const _RBTreeNodeBase* _node) {
    _RBTreeNodeBase* gp = _node->get_grandparent();
    if (!gp) {
      return nullptr;
    }
    return _node->parent == gp->left ? gp->right : gp->left;
  }
  static _RBTreeNodeBase* get_sibling(const _RBTreeNodeBase* _node) {
    if (!_node->parent) {
      return nullptr;
    }
    return _node == _node->parent->left ?
        _node->parent->right : _node->parent->left;
  }
};

struct _RBTreeHeader {
  _RBTreeNodeBase* _root;
  _RBTreeNodeBase* _nil;
  std::size_t _size;
  _RBTreeHeader() { this->__init_default(); }
  _RBTreeHeader(_RBTreeHeader&& _header) noexcept {
    this->__init_other(_header);
    _header.__init_default();
  }
  _RBTreeHeader& operator=(_RBTreeHeader&& _header) {
    this->__init_other(_header);
    _header.__init_default();
    return *this;
  }
  void __init_default() {
    this->_root = this->_nil = nullptr;
    this->_size = 0;
  }
  void __init_other(_RBTreeNodeHeader& _other) {
    this->_root = _other._root;
    this->_nil = _other._nil;
    this->_size = _other._size;
  }
};

template <typename _T>
struct _RBTreeNode : public _RBTreeNodeBase {
  _T _data;
  _T* __addr() noexcept { return std::adressof(this->_data); }
  constexpr const _T* __addr() const noexcept {
    return std::addressof(this->_data);
  }
};



///////////////////////// red black tree base /////////////////////////
template <typename _T, typename _Allocator>
struct _RBTreeBase {
  /////////////// member type ///////////////
 protected:
  using _ValueType _T;
  using _DataAllocType = _Allocator;
  using _DataAllocTraits = std::allocator_traits<_DataAllocType>;
  using _NodeBase = _RBTreeNodeBase;
  using _NodeType = _RBTreeNode<_ValueType>;
  using _NodeAllocType =
      typename _DataAllocTraits::template rebind_alloc<_NodeType>;
  using _NodeAllocTraits =
      typename _DataAllocTraits::template rebind_traits<_NodeType>;
  // TODO: using iterator type

  /////////////// implement ///////////////
 protected:
  struct _RBTreeImpl : public _NodeAllocType {
    _RBTreeHeader _header;
    _RBTreeImpl() = default;
    _RBTreeImpl(const _NodeAllocType& _alloc) : _NodeAllocType(_alloc) {}
    void __swap(_ListImpl& _impl) {
      std::swap(_impl._header, header);
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
  ~RBTreeBase() { _RBTreeBase::__clear(_impl); }

  /////////////// utils function ///////////////
 protected:
  static void __clear_aux(_RBTreeImpl& _impl, _NodeBase* _node) {
    if (!_node || _node == nil) {
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
  static constexpr void __max_size() const noexcept {
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
template <typename T, typename Compare, typename Alloctor = std::allocator<T>>
class rb_tree : protected __container_base::_RBTreeBase<T, Alloctor> {
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
  // TODO: using iterator type

  /////////////// data member ///////////////
 private:
  using _Base::_impl;

  /////////////// constructor ///////////////
 public:
  rb_tree() = default;

  /////////////// destructor ///////////////
 public:
  ~rb_tree() noexcept = default;

  /////////////// insert ///////////////
 private:

};

} // namespace stl
} // namespace ginshio

#endif // GINSHIO_STL__STL_RB_TREE_HH_
