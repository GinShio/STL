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

#ifndef GINSHIO_STL__MULTIMAP_HH_
#define GINSHIO_STL__MULTIMAP_HH_ 1

#include "container/map.hh"

namespace ginshio {
namespace stl {

///////////////////////// multimap /////////////////////////
template <typename Key, typename T, class Container =
          ginshio::stl::rb_tree<tree::tree_pair<const Key, T>>>
class multimap {
  /////////////// private type ///////////////
 private:
  template <typename _Alloc>
  using _UseAlloc = typename
      std::enable_if<std::uses_allocator<Container, _Alloc>::value>::type*;
  /////////////// member type ///////////////
 public:
  using key_type = Key;
  using mappred_type = T;
  using container_type = Container;
  using value_type = typename Container::value_type;
  using allocator_type = typename Container::allocator_type;
  using size_type = typename Container::size_type;
  using difference_type = typename Container::difference_type;
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

  /////////////// TODO: constructor ///////////////
 public:
  multimap() = default;

  /////////////// destructor ///////////////
 public:
  ~multimap() noexcept = default;

  /////////////// TODO: member function ///////////////
 public:
  constexpr const container_type& get_container() const noexcept { return c; }

  /////////////// TODO: element access ///////////////
 public:

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
  iterator insert(const value_type& value) { return c.emplace_equal(value); }
  iterator insert(value_type&& value) {
    return c.emplace_equal(std::forward(value));
  }
  template <typename P>
  iterator insert(P&& value) { return c.emplace_equal(std::forward<P>(value)); }
  iterator insert(const_iterator hint, const value_type& value) {
    return c.emplace_hint_equal(hint, value);
  }
  iterator insert(const_iterator hint, value_type&& value) {
    return c.emplace_hint_equal(hint, std::forward(value));
  }
  template <typename P>
  iterator insert(const_iterator hint, P&& value) {
    return c.emplace_hint_equal(hint, std::forward<P>(value));
  }
  template <typename InputIt>
  void insert(InputIt first, InputIt last) {
    while (first != last) {
      c.emplace_equal(std::move(*first));
    }
  }
  void insert(std::initializer_list<value_type> ilist) {
    for (auto& value : ilist) {
      c.emplace_equal(std::forward(value));
    }
  }
  // TODO: insert_return_type insert(node_type&& nh);
  // TODO: iterator insert(const_iterator hint, node_type&& nh);
  template <typename... Args>
  iterator emplace(Args&&... args) {
    return c.emplace_equal(std::forward<Args>(args)...);
  }
  template <typename... Args>
  iterator emplace_hint(const_iterator hint, Args&&... args) {
    return c.emplace_hint_equal(hint, std::forward<Args>(args)...);
  }
  // TODO: try_emplace
  // TODO: erase
  // TODO: extract
  void swap(multimap& other) { c.swap(other.c); }
  void merge(multimap& other) { c.merge_equal(std::move(other.c)); }
  void merge(multimap&& other) { c.merge_equal(std::forward(other.c)); }
  // TODO: merge map && difference container(e.g. avl_tree)

  /////////////// find ///////////////
  /////////////// TODO: template <typename K> ///////////////
 public:
  size_type count(const Key& key) const {
    return c.count(key);
  }
  iterator find(const Key& key) { return c.find(key); }
  const_iterator find(const Key& key) const { return c.find(key); }
  // TODO: contains
  std::pair<iterator, iterator> equal_range(const Key& key) {
    return c.equal_range(key);
  }
  std::pair<const_iterator, const_iterator> equal_range(const Key& key) const {
    return c.equal_range(key);
  }
  iterator lower_bound(const Key& key) { return c.lower_bound(key); }
  const_iterator lower_bound(const Key& key) const {
    return c.lower_bound(key);
  }
  iterator upper_bound(const Key& key) { return c.upper_bound(key); }
  const_iterator upper_bound(const Key& key) const {
    return c.upper_bound(key);
  }
};



///////////////////////// java style iterator /////////////////////////
template <typename Key, typename T>
using MultiMapIterator = multimap<Key, T>::iterator;
template <typename Key, typename T>
using MultiMapConstIterator = multimap<Key, T>::const_iterator;



///////////////////////// multimap comparison operators /////////////////////////
template <typename Key, typename T, typename Container>
constexpr bool operator==(const multimap<Key, T, Container>& lhs,
                          const multimap<Key, T, Container>& rhs) {
  return lhs.get_container() == rhs.get_container();
}
template <typename Key, typename T, typename Container>
constexpr bool operator!=(const multimap<Key, T, Container>& lhs,
                          const multimap<Key, T, Container>& rhs) {
  return lhs.get_container() != rhs.get_container();
}
template <typename Key, typename T, typename Container>
constexpr bool operator<(const multimap<Key, T, Container>& lhs,
                         const multimap<Key, T, Container>& rhs) {
  return lhs.get_container() < rhs.get_container();
}
template <typename Key, typename T, typename Container>
constexpr bool operator>(const multimap<Key, T, Container>& lhs,
                         const multimap<Key, T, Container>& rhs) {
  return lhs.get_container() > rhs.get_container();
}
template <typename Key, typename T, typename Container>
constexpr bool operator<=(const multimap<Key, T, Container>& lhs,
                          const multimap<Key, T, Container>& rhs) {
  return !(rhs.get_container() < lhs.get_container());
}
template <typename Key, typename T, typename Container>
constexpr bool operator>=(const multimap<Key, T, Container>& lhs,
                          const multimap<Key, T, Container>& rhs) {
  return !(lhs.get_container() < rhs.get_container());
}



///////////////////////// specialization /////////////////////////
template <typename Key, typename T, typename Container>
inline void swap(multimap<Key, T, Container>& lhs,
                 multimap<Key, T, Container>& rhs) {
  lhs.swap(rhs);
}

// TODO: erase && erase_if

} // namespace stl
} // namespace ginshio



namespace std {
///////////////////////// specialization /////////////////////////
template <typename Key, typename T, typename Container>
inline void swap(ginshio::stl::multimap<Key, T, Container>& lhs,
                 ginshio::stl::multimap<Key, T, Container>& rhs) {
  lhs.swap(rhs);
}

// TODO: erase && erase_if
} // namespace std

#endif // GINSHIO_STL__MULTIMAP_HH_
