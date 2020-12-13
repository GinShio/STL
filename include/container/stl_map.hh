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

#ifndef GINSHIO_STL__CONTAINER_STL_MAP_HH_
#define GINSHIO_STL__CONTAINER_STL_MAP_HH_ 1

namespace ginshio {
namespace stl {

template <typename Key, typename T,
          typename Compare, typename Allocator, typename Container>
class multimap;

///////////////////////// map /////////////////////////
template <typename Key, typename T,
          typename Compare = ::std::less<Key>,
          typename Allocator = ::std::allocator<::std::pair<const Key, T>>,
          typename Container = ::ginshio::stl::rb_tree<
            ::std::pair<const Key, T>, Compare,
            ::ginshio::stl::tree::_KeyOfValue<::std::pair<const Key, T>>,
            Allocator>>
class map {
  /////////////// private type ///////////////
 private:
  template <typename _Alloc>
  using _UseAlloc = typename ::std::enable_if<
      ::std::uses_allocator<Container, _Alloc>::value>::type;
  /////////////// member type ///////////////
 public:
  using key_type = Key;
  using mapped_type = T;
  using container_type = Container;
  using value_type = typename Container::value_type;
  using allocator_type = typename Container::allocator_type;
  using size_type = typename Container::size_type;
  using difference_type = typename Container::difference_type;
  using key_of_value = typename Container::key_of_value;
  using key_compare = typename Container::key_compare;
  using reference = typename Container::reference;
  using const_reference = typename Container::const_reference;
  using pointer = typename Container::pointer;
  using const_pointer = typename Container::const_pointer;
  using iterator = typename Container::iterator;
  using const_iterator = typename Container::const_iterator;
  using reverse_iterator = typename Container::reverse_iterator;
  using const_reverse_iterator = typename Container::const_reverse_iterator;
  using node_type = typename Container::node_type;
  // TODO: insert_return_type

  /////////////// data member ///////////////
 protected:
  container_type c;

  /////////////// constructor ///////////////
 public:
  map() = default;
  explicit map(const allocator_type& alloc) : c(alloc) {}
  template <typename InputIt,
            typename = typename ::std::enable_if<
              ::ginshio::stl::is_input_iterator<InputIt>::value>::type>
  map(InputIt first, InputIt last,
      const allocator_type& alloc = allocator_type()) : c(first, last, alloc) {}
  map(const map& other) : c(other.c) {}
  map(const map& other, const allocator_type& alloc) : c(other.c, alloc) {}
  map(map&& other) noexcept = default;
  map(map&& other, const allocator_type& alloc)
      : c(::std::move(other.c), alloc) {}
  map(::std::initializer_list<value_type> ilist,
      const allocator_type& alloc = allocator_type()) : c(alloc) {
    for (auto& _val : ilist) {
      c.emplace_unique(_val);
    }
  }

  /////////////// destructor ///////////////
 public:
  ~map() noexcept = default;

  /////////////// member function ///////////////
 public:
  template <typename InputIt,
            typename = typename ::std::enable_if<
              ::ginshio::stl::is_input_iterator<InputIt>::value>::type>
  void assign(InputIt first, InputIt last) {
    c.assign_unique(first, last);
  }
  void assign(std::initializer_list<value_type> ilist) {
    c.assign_unique(ilist);
  }
  template <typename Tree,
            typename = typename ::std::enable_if<::std::is_base_of<
              __container_base::_TreeBase<
                value_type, key_compare, key_of_value, allocator_type>,
              Tree>::value>::type>
  map& operator=(const multimap<key_type, mapped_type,
                 key_compare, allocator_type, Tree>& other) {
    c.assign_unique_copy_allocator(other.c);
    return *this;
  }
  template <typename Tree,
            typename = typename ::std::enable_if<::std::is_base_of<
              __container_base::_TreeBase<
                value_type, key_compare, key_of_value, allocator_type>,
              Tree>::value>::type>
  map& operator=(const map<key_type, mapped_type,
                 key_compare, allocator_type, Tree>& other) {
    c.assign_unique_copy_allocator(other.c);
    return *this;
  }
  map& operator=(map&& other) {
    c.assign_unique_move_allocator(::std::move(other.c));
    return *this;
  }
  map& operator=(::std::initializer_list<value_type> ilist) {
    c.assign_unique(ilist);
    return *this;
  }
  constexpr const container_type& get_container() const noexcept { return c; }
  constexpr const allocator_type get_allocator() const noexcept {
    return c.get_allocator();
  }

  /////////////// element access ///////////////
 public:
  mapped_type& at(const key_type& key) {
    iterator _pos = this->find(key);
    ::ginshio::stl::__check_out_of_range("map::at", _pos, this->end());
    return _pos->second;
  }
  const mapped_type& at(const key_type& key) const {
    const_iterator _pos = this->find(key);
    ::ginshio::stl::__check_out_of_range("map::at", _pos, this->cend());
    return _pos->second;
  }
  mapped_type& operator[](const key_type& key) {
    iterator _pos = this->find(key);
    if (_pos == this->end()) {
      return this->insert(::std::make_pair(key, mapped_type())).first->second;
    } else {
      return _pos->second;
    }
  }
  mapped_type& operator[](key_type&& key) {
    iterator _pos = this->find(key);
    if (_pos == this->end()) {
      return this->insert(::std::make_pair(::std::forward<key_type>(key),
                                           mapped_type())).first->second;
    } else {
      return _pos->second;
    }
  }

  /////////////// iterator ///////////////
 public:
  iterator begin() noexcept { return c.begin(); }
  constexpr const_iterator begin() const noexcept { return c.begin(); }
  constexpr const_iterator cbegin() const noexcept { return c.cbegin(); }
  iterator end() noexcept { return c.end(); }
  constexpr const_iterator end() const noexcept { return c.end(); }
  constexpr const_iterator cend() const noexcept { return c.cend(); }
  reverse_iterator rbegin() noexcept { return c.rbegin(); }
  constexpr const_reverse_iterator rbegin() const noexcept {
    return c.rbegin();
  }
  constexpr const_reverse_iterator crbegin() const noexcept {
    return c.crbegin();
  }
  reverse_iterator rend() noexcept { return c.rend(); }
  constexpr const_reverse_iterator rend() const noexcept { return c.rend(); }
  constexpr const_reverse_iterator crend() const noexcept { return c.crend(); }

  /////////////// capacity ///////////////
 public:
  constexpr bool empty() const noexcept { return c.empty(); }
  constexpr size_type size() const noexcept { return c.size(); }
  constexpr size_type max_size() const noexcept { return c.max_size(); }
  void shrink_to_fit() {}
  constexpr size_type capacity() const noexcept { return c.capacity(); }

  /////////////// modifier function ///////////////
 public:
  void clear() noexcept { c.clear(); }
  ::std::pair<iterator, bool> insert(const value_type& value) {
    return c.emplace_unique(value);
  }
  ::std::pair<iterator, bool> insert(value_type&& value) {
    return c.emplace_unique(::std::forward<value_type>(value));
  }
  template <typename P>
  ::std::pair<iterator, bool> insert(P&& value) {
    return c.emplace_unique(::std::forward<P>(value));
  }
  iterator insert(const_iterator hint, const value_type& value) {
    return c.emplace_hint_unique(hint, value).first;
  }
  iterator insert(const_iterator hint, value_type&& value) {
    return c.emplace_hint_unique(hint, ::std::forward<value_type>(value)).first;
  }
  template <typename P>
  iterator insert(const_iterator hint, P&& value) {
    return c.emplace_hint_unique(hint, ::std::forward<P>(value)).first;
  }
  template <typename InputIt,
            typename = typename ::std::enable_if<
              ::ginshio::stl::is_input_iterator<InputIt>::value>::type>
  void insert(InputIt first, InputIt last) {
    c.insert_unique(first, last);
  }
  void insert(::std::initializer_list<value_type> ilist) {
    for (auto& value : ilist) {
      c.emplace_unique(value);
    }
  }
  // TODO: insert_return_type insert(node_type&& nh);
  // TODO: iterator insert(const_iterator hint, node_type&& nh);
  template <typename M>
  ::std::pair<iterator, bool> insert_or_assign(const key_type& key, M&& obj) {
    iterator _pos = this->find(key);
    if (_pos == this->end()) {
      return {c.emplace_equal(key, ::std::forward<M>(obj)), true};
    } else {
      _pos->second = ::std::forward<M>(obj);
      return {_pos, false};
    }
  }
  template <typename M>
  ::std::pair<iterator, bool> insert_or_assign(key_type&& key, M&& obj) {
    iterator _pos = this->find(key);
    if (_pos == this->end()) {
      return {c.emplace_equal(::std::move(key), ::std::forward<M>(obj)), true};
    } else {
      _pos->second = ::std::forward<M>(obj);
      return {_pos, false};
    }
  }
  template <typename M>
  iterator insert_or_assign(const_iterator hint, const key_type& key, M&& obj) {
    iterator _pos = this->find(key);
    if (_pos == this->end()) {
      return c.emplace_hint_equal(hint, key, ::std::forward<M>(obj));
    } else {
      _pos->second = ::std::forward<M>(obj);
      return _pos;
    }
  }
  template <typename M>
  iterator insert_or_assign(const_iterator hint, key_type&& key, M&& obj) {
    iterator _pos = this->find(key);
    if (_pos == this->end()) {
      return c.emplace_hint_equal(hint, ::std::move(key),
                                  ::std::forward<M>(obj));
    } else {
      _pos->second = ::std::forward<M>(obj);
      return _pos;
    }
  }
  template <typename... Args>
  ::std::pair<iterator, bool> emplace(Args&&... args) {
    return c.emplace_unique(::std::forward<Args>(args)...);
  }
  template <typename... Args>
  iterator emplace_hint(const_iterator hint, Args&&... args) {
    return c.emplace_hint_unique(hint, ::std::forward<Args>(args)...).first;
  }
  template <typename... Args>
  ::std::pair<iterator, bool> try_emplace(const key_type& key, Args&&... args) {
    iterator _pos = this->find(key);
    if (_pos == this->end()) {
      return {c.emplace_equal(key, ::std::forward<Args>(args)...), true};
    }
    return {_pos, false};
  }
  template <typename... Args>
  ::std::pair<iterator, bool> try_emplace(key_type&& key, Args&&... args) {
    iterator _pos = this->find(key);
    if (_pos == this->end()) {
      return {c.emplace_equal(::std::forward<key_type>(key),
                              ::std::forward<Args>(args)...), true};
    }
    return {_pos, false};
  }
  template <typename... Args>
  iterator try_emplace(const_iterator hint, const key_type& key,
                       Args&&... args) {
    iterator _pos = this->find(key);
    if (_pos == this->end()) {
      return c.emplace_hint_equal(hint, key, ::std::forward<Args>(args)...);
    }
    return _pos;
  }
  template <typename... Args>
  iterator try_emplace(const_iterator hint, key_type&& key, Args&&... args) {
    iterator _pos = this->find(key);
    if (_pos == this->end()) {
      return c.emplace_hint_equal(hint, ::std::move(key),
                                  ::std::forward<Args>(args)...);
    }
    return _pos;
  }
  iterator erase(const_iterator pos) { return c.erase(pos); }
  iterator erase(const_iterator first, const_iterator last) {
    return c.erase(first, last);
  }
  size_type erase(const key_type& key) { return c.erase(key); }
  node_type extract(const_iterator pos) { return c.extract(pos); }
  node_type extract(const key_type& key) { return c.extract(key); }
  void swap(map& other) { c.swap(other.c); }
  template <typename Tree,
            typename = typename ::std::enable_if<
              ::std::is_base_of<
                __container_base::_TreeBase<
                  value_type, key_compare, key_of_value, allocator_type>,
                Tree>::value &&
              !std::is_same<Tree, Container>::value>::type>
  void merge(const map<key_type, mapped_type,
             key_compare, allocator_type, Tree>& other) {
    c.merge_unique(other.c);
  }
  void merge(map& other) { c.merge_unique(::std::move(other.c)); }
  void merge(map&& other) { c.merge_unique(::std::move(other.c)); }
  template <typename Tree,
            typename = typename ::std::enable_if<
              ::std::is_base_of<
                __container_base::_TreeBase<
                  value_type, key_compare, key_of_value, allocator_type>,
                Tree>::value &&
              !std::is_same<Tree, Container>::value>::type>
  void merge(const multimap<key_type, mapped_type,
             key_compare, allocator_type, Tree>& other) {
    c.merge_unique(other.c);
  }
  void merge(multimap<key_type, mapped_type,
             key_compare, allocator_type, container_type>& other) {
    c.merge_unique(::std::move(other.c));
  }
  void merge(multimap<key_type, mapped_type,
             key_compare, allocator_type, container_type>&& other) {
    c.merge_unique(::std::move(other.c));
  }

  /////////////// find ///////////////
 public:
  size_type count(const key_type& key) const { return c.count(key); }
  template <typename K>
  size_type count(const K& key) const { return c.count(key); }
  iterator find(const key_type& key) { return c.find(key); }
  const_iterator find(const key_type& key) const { return c.find(key); }
  template <typename K>
  iterator find(const K& key) { return c.find(key); }
  template <typename K>
  const_iterator find(const K& key) const { return c.find(key); }
  bool contains(const key_type& key) const { return c.find(key) != c.end(); }
  template <typename K>
  bool contains(const K& key) const { return c.find(key) != c.end(); }
  ::std::pair<iterator, iterator> equal_range(const key_type& key) {
    return c.equal_range(key);
  }
  auto equal_range(const key_type& key) const
      -> ::std::pair<const_iterator, const_iterator> {
    return c.equal_range(key);
  }
  template <typename K>
  ::std::pair<iterator, iterator> equal_range(const K& key) {
    return c.equal_range(key);
  }
  template <typename K>
  auto equal_range(const K& key) const
      -> ::std::pair<const_iterator, const_iterator> {
    return c.equal_range(key);
  }
  iterator lower_bound(const key_type& key) { return c.lower_bound(key); }
  const_iterator lower_bound(const key_type& key) const {
    return c.lower_bound(key);
  }
  template <typename K>
  iterator lower_bound(const K& key) { return c.lower_bound(key); }
  template <typename K>
  const_iterator lower_bound(const K& key) const {
    return c.lower_bound(key);
  }
  iterator upper_bound(const key_type& key) { return c.upper_bound(key); }
  const_iterator upper_bound(const key_type& key) const {
    return c.upper_bound(key);
  }
  template <typename K>
  iterator upper_bound(const K& key) { return c.upper_bound(key); }
  template <typename K>
  const_iterator upper_bound(const K& key) const {
    return c.upper_bound(key);
  }
};



///////////////////////// java style iterator /////////////////////////
template <typename Key, typename T>
using MapIterator = typename map<Key, T>::iterator;
template <typename Key, typename T>
using MapConstIterator = typename map<Key, T>::const_iterator;



///////////////////////// map comparison operators /////////////////////////
template <typename Key, typename T,
          typename Compare, typename Allocator, typename Container>
constexpr bool operator==(
    const map<Key, T, Compare, Allocator, Container>& lhs,
    const map<Key, T, Compare, Allocator, Container>& rhs) {
  return lhs.get_container() == rhs.get_container();
}
template <typename Key, typename T,
          typename Compare, typename Allocator, typename Container>
constexpr bool operator!=(
    const map<Key, T, Compare, Allocator, Container>& lhs,
    const map<Key, T, Compare, Allocator, Container>& rhs) {
  return lhs.get_container() != rhs.get_container();
}
template <typename Key, typename T,
          typename Compare, typename Allocator, typename Container>
constexpr bool operator<(
    const map<Key, T, Compare, Allocator, Container>& lhs,
    const map<Key, T, Compare, Allocator, Container>& rhs) {
  return lhs.get_container() < rhs.get_container();
}
template <typename Key, typename T,
          typename Compare, typename Allocator, typename Container>
constexpr bool operator>(
    const map<Key, T, Compare, Allocator, Container>& lhs,
    const map<Key, T, Compare, Allocator, Container>& rhs) {
  return lhs.get_container() > rhs.get_container();
}
template <typename Key, typename T,
          typename Compare, typename Allocator, typename Container>
constexpr bool operator<=(
    const map<Key, T, Compare, Allocator, Container>& lhs,
    const map<Key, T, Compare, Allocator, Container>& rhs) {
  return !(rhs.get_container() < lhs.get_container());
}
template <typename Key, typename T,
          typename Compare, typename Allocator, typename Container>
constexpr bool operator>=(
    const map<Key, T, Compare, Allocator, Container>& lhs,
    const map<Key, T, Compare, Allocator, Container>& rhs) {
  return !(lhs.get_container() < rhs.get_container());
}



///////////////////////// specialization /////////////////////////
template <typename Key, typename T,
          typename Compare, typename Allocator, typename Container>
inline void swap(map<Key, T, Compare, Allocator, Container>& lhs,
                 map<Key, T, Compare, Allocator, Container>& rhs) {
  lhs.swap(rhs);
}

template <typename Key, typename T, typename Compare,
          typename Allocator, typename Container, typename Pred>
auto erase_if(map<Key, T, Compare, Allocator, Container>& c, Pred pred) ->
    typename map<Key, T, Compare, Allocator, Container>::size_type {
  return erase_if(c.get_container(), pred);
}

}  // namespace stl
}  // namespace ginshio





namespace std {
///////////////////////// specialization /////////////////////////
template <typename Key, typename T,
          typename Compare, typename Allocator, typename Container>
inline void swap(
    ::ginshio::stl::map<Key, T, Compare, Allocator, Container>& lhs,
    ::ginshio::stl::map<Key, T, Compare, Allocator, Container>& rhs) {
  lhs.swap(rhs);
}

template <typename Key, typename T, typename Compare,
          typename Allocator, typename Container, typename Pred>
auto erase_if(
    ::ginshio::stl::map<Key, T, Compare, Allocator, Container>& c,
    Pred pred) ->
    typename ::ginshio::stl::map<Key, T, Compare,
                                 Allocator, Container>::size_type {
  return ::ginshio::stl::erase_if(c.get_container(), pred);
}
}  // namespace std

#endif  // GINSHIO_STL__CONTAINER_STL_MAP_HH_
