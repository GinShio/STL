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

#ifndef GINSHIO_STL__STL_MAP_HH_
#define GINSHIO_STL__STL_MAP_HH_ 1

#include "base/stl_tree_algo.hh"
#include "rb_tree.hpp"

#include <initializer_list>
#include <memory>
#include <type_traits>
#include <utility>

#include "container/stl_multimap.hh"

namespace ginshio {
namespace stl {

///////////////////////// map /////////////////////////
template <typename Key, typename T, class Container =
          ginshio::stl::rb_tree<tree::associative_pair<const Key, T>>>
class map {
  /////////////// private type ///////////////
 private:
  template <typename _Alloc>
  using _UseAlloc = typename
      std::enable_if<std::uses_allocator<Container, _Alloc>::value>::type*;
  /////////////// member type ///////////////
 public:
  using key_type = Key;
  using mapped_type = T;
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
  // TODO: node_type && insert_return_type

  /////////////// data member ///////////////
 protected:
  container_type c;

  /////////////// constructor ///////////////
 public:
  map() = default;
  explicit map(const allocator_type& alloc) : c(alloc) {}
  template <typename InputIt, typename = typename
            std::enable_if<std::is_base_of<
                             std::input_iterator_tag, typename
                             std::iterator_traits<InputIt>::iterator_category>::
                           value>::type*>
  map(InputIt first, InputIt last,
      const allocator_type& alloc = allocator_type()) : c(first, last, alloc) {}
  map(const map& other) : c(other.c) {}
  map(const map& other, const allocator_type& alloc) : c(other.c, alloc) {}
  map(map&& other) noexcept = default;
  map(map&& other, const allocator_type& alloc) :
      c(std::move(other.c), alloc) {}
  map(std::initializer_list<value_type> ilist,
      const allocator_type& alloc = allocator_type()) : c(alloc) {
    for (auto& _val : ilist) {
      c.emplace_unique(_val);
    }
  }

  /////////////// destructor ///////////////
 public:
  ~map() noexcept = default;

  /////////////// TODO: member function ///////////////
 public:
  constexpr const container_type& get_container() const noexcept { return c; }
  constexpr const allocator_type get_allocator() const noexcept {
    return c.get_allocator();
  }

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
  std::pair<iterator, bool> insert(const value_type& value) {
    return c.emplace_unique(value);
  }
  std::pair<iterator, bool> insert(value_type&& value) {
    return c.emplace_unique(std::forward(value));
  }
  template <typename P>
  std::pair<iterator, bool> insert(P&& value) {
    return c.emplace_unique(std::forward<P>(value));
  }
  iterator insert(const_iterator hint, const value_type& value) {
    return c.emplace_hint_unique(hint, value).first;
  }
  iterator insert(const_iterator hint, value_type&& value) {
    return c.emplace_hint_unique(hint, std::forward(value)).first;
  }
  template <typename P>
  iterator insert(const_iterator hint, P&& value) {
    return c.emplace_hint_unique(hint, std::forward<P>(value)).first;
  }
  template <typename InputIt>
  void insert(InputIt first, InputIt last) {
    while (first != last) {
      c.emplace_unique(*first);
    }
  }
  void insert(std::initializer_list<value_type> ilist) {
    for (auto& value : ilist) {
      c.emplace_unique(std::forward(value));
    }
  }
  // TODO: insert_return_type insert(node_type&& nh);
  // TODO: iterator insert(const_iterator hint, node_type&& nh);
  template <typename M>
  std::pair<iterator, bool> insert_or_assign(const key_type& key, M&& obj) {
    iterator _pos = this->find(key);
    if (_pos == this->end()) {
      return {c.emplace_equal(key, std::forward<M>(obj)), true};
    } else {
      _pos->second = std::forward<M>(obj);
      return {_pos, false};
    }
  }
  template <typename M>
  std::pair<iterator, bool> insert_or_assign(key_type&& key, M&& obj) {
    iterator _pos = this->find(key);
    if (_pos == this->end()) {
      return {c.emplace_equal(std::move(key), std::forward<M>(obj)), true};
    } else {
      _pos->second = std::forward<M>(obj);
      return {_pos, false};
    }
  }
  template <typename M>
  iterator insert_or_assign(const_iterator hint, const key_type& key, M&& obj) {
    iterator _pos = this->find(key);
    if (_pos == this->end()) {
      return c.emplace_hint_equal(hint, key, std::forward<M>(obj));
    } else {
      _pos->second = std::forward<M>(obj);
      return _pos;
    }
  }
  template <typename M>
  iterator insert_or_assign(const_iterator hint, key_type&& key, M&& obj) {
    iterator _pos = this->find(key);
    if (_pos == this->end()) {
      return c.emplace_hint_equal(hint, std::move(key), std::forward<M>(obj));
    } else {
      _pos->second = std::forward<M>(obj);
      return _pos;
    }
  }
  template <typename... Args>
  std::pair<iterator, bool> emplace(Args&&... args) {
    return c.emplace_unique(std::forward<Args>(args)...);
  }
  template <typename... Args>
  iterator emplace_hint(const_iterator hint, Args&&... args) {
    return c.emplace_hint_unique(hint, std::forward<Args>(args)...).first;
  }
  template <typename... Args>
  std::pair<iterator, bool> try_emplace(const key_type& key, Args&&... args) {
    iterator _pos = this->find(key);
    if (_pos == this->end()) {
      return {c.emplace_equal(key, std::forward<Args>(args)...), true};
    }
    return {_pos, false};
  }
  template <typename... Args>
  std::pair<iterator, bool> try_emplace(key_type&& key, Args&&... args) {
    iterator _pos = this->find(key);
    if (_pos == this->end()) {
      return {c.emplace_equal(std::forward<key_type>(key),
                              std::forward<Args>(args)...), true};
    }
    return {_pos, false};
  }
  template <typename... Args>
  iterator try_emplace(const_iterator hint,
                       const key_type& key, Args&&... args) {
    iterator _pos = this->find(key);
    if (_pos == this->end()) {
      return c.emplace_hint_equal(hint, key, std::forward<Args>(args)...);
    }
    return _pos;
  }
  template <typename... Args>
  iterator try_emplace(const_iterator hint, key_type&& key, Args&&... args) {
    iterator _pos = this->find(key);
    if (_pos == this->end()) {
      return c.emplace_hint_equal(hint, std::move(key),
                                  std::forward<Args>(args)...);
    }
    return _pos;
  }
  iterator erase(const_iterator pos) { return c.erase(pos); }
  iterator erase(const_iterator first, const_iterator last) {
    return c.erase(first, last);
  }
  size_type erase(const key_type& key) { return c.erase(key); }
  // TODO: extract
  void swap(map& other) { c.swap(other.c); }
  void merge(map& other) { c.merge_unique(std::move(other.c)); }
  void merge(map&& other) { c.merge_unique(std::move(other.c)); }
  void merge(multimap<key_type, mapped_type, container_type>& other) {
    c.merge_unique(std::move(other.c));
  }
  void merge(multimap<key_type, mapped_type, container_type>&& other) {
    c.merge_unique(std::move(other.c));
  }
  // TODO: merge difference container(e.g. avl_tree)

  /////////////// find ///////////////
  /////////////// TODO: template overload K in C++14 ///////////////
 public:
  size_type count(const key_type& key) const {
    return c.count(key);
  }
  iterator find(const key_type& key) { return c.find(key); }
  const_iterator find(const key_type& key) const { return c.find(key); }
  bool contains(const key_type& key) const { return c.find(key) != c.end(); }
  std::pair<iterator, iterator> equal_range(const key_type& key) {
    return c.equal_range(key);
  }
  auto equal_range(const key_type& key) const
      -> std::pair<const_iterator, const_iterator> {
    return c.equal_range(key);
  }
  iterator lower_bound(const key_type& key) { return c.lower_bound(key); }
  const_iterator lower_bound(const key_type& key) const {
    return c.lower_bound(key);
  }
  iterator upper_bound(const key_type& key) { return c.upper_bound(key); }
  const_iterator upper_bound(const key_type& key) const {
    return c.upper_bound(key);
  }
};



///////////////////////// java style iterator /////////////////////////
template <typename Key, typename T>
using MapIterator = typename map<Key, T>::iterator;
template <typename Key, typename T>
using MapConstIterator = typename map<Key, T>::const_iterator;



///////////////////////// map comparison operators /////////////////////////
template <typename Key, typename T, typename Container>
constexpr bool operator==(const map<Key, T, Container>& lhs,
                          const map<Key, T, Container>& rhs) {
  return lhs.get_container() == rhs.get_container();
}
template <typename Key, typename T, typename Container>
constexpr bool operator!=(const map<Key, T, Container>& lhs,
                          const map<Key, T, Container>& rhs) {
  return lhs.get_container() != rhs.get_container();
}
template <typename Key, typename T, typename Container>
constexpr bool operator<(const map<Key, T, Container>& lhs,
                         const map<Key, T, Container>& rhs) {
  return lhs.get_container() < rhs.get_container();
}
template <typename Key, typename T, typename Container>
constexpr bool operator>(const map<Key, T, Container>& lhs,
                         const map<Key, T, Container>& rhs) {
  return lhs.get_container() > rhs.get_container();
}
template <typename Key, typename T, typename Container>
constexpr bool operator<=(const map<Key, T, Container>& lhs,
                          const map<Key, T, Container>& rhs) {
  return !(rhs.get_container() < lhs.get_container());
}
template <typename Key, typename T, typename Container>
constexpr bool operator>=(const map<Key, T, Container>& lhs,
                          const map<Key, T, Container>& rhs) {
  return !(lhs.get_container() < rhs.get_container());
}



///////////////////////// specialization /////////////////////////
template <typename Key, typename T, typename Container>
inline void swap(map<Key, T, Container>& lhs, map<Key, T, Container>& rhs) {
  lhs.swap(rhs);
}

template <typename Key, typename T, typename Container, typename Pred>
auto erase_if(map<Key, T, Container>& c, Pred pred)
    -> typename map<Key, T, Container>::size_type {
  return erase_if(c.get_container(), pred);
}

} // namespace stl
} // namespace ginshio



namespace std {
///////////////////////// specialization /////////////////////////
template <typename Key, typename T, typename Container>
inline void swap(ginshio::stl::map<Key, T, Container>& lhs,
                 ginshio::stl::map<Key, T, Container>& rhs) {
  lhs.swap(rhs);
}

template <typename Key, typename T, typename Container, typename Pred>
auto erase_if(ginshio::stl::map<Key, T, Container>& c, Pred pred)
    -> typename ginshio::stl::map<Key, T, Container>::size_type {
  return ginshio::stl::erase_if(c.get_container(), pred);
}
} // namespace std

#endif // GINSHIO_STL__STL_MAP_HH_
