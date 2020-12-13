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

#ifndef GINSHIO_STL__CONTAINER_STL_MULTISET_HH_
#define GINSHIO_STL__CONTAINER_STL_MULTISET_HH_ 1

namespace ginshio {
namespace stl {

template <typename Key, typename Compare, typename Allocator,
          typename Container>
class set;

///////////////////////// multiset /////////////////////////
template <typename Key, typename Compare = ::std::less<Key>,
          typename Allocator = ::std::allocator<Key>,
          typename Container =
          ::ginshio::stl::rb_tree<Key, Compare,
                                  ::ginshio::stl::tree::_KeyOfValue<Key>,
                                  Allocator>>
class multiset {
  /////////////// private type ///////////////
 private:
  template <typename _Alloc>
  using _UseAlloc = typename ::std::enable_if<
      ::std::uses_allocator<Container, _Alloc>::value>::type;
  /////////////// member type ///////////////
 public:
  using key_type = Key;
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
  multiset() = default;
  explicit multiset(const allocator_type& alloc) : c(alloc) {}
  template <typename InputIt,
            typename = typename ::std::enable_if<
              ::ginshio::stl::is_input_iterator<InputIt>::value>::type>
  multiset(InputIt first, InputIt last,
           const allocator_type& alloc = allocator_type())
      : c(first, last, alloc) {}
  multiset(const multiset& other) : c(other.c) {}
  multiset(const multiset& other, const allocator_type& alloc)
      : c(other.c, alloc) {}
  multiset(multiset&& other) noexcept = default;
  multiset(multiset&& other, const allocator_type& alloc)
      : c(::std::move(other.c), alloc) {}
  multiset(::std::initializer_list<value_type> ilist,
           const allocator_type& alloc = allocator_type()) : c(ilist, alloc) {}

  /////////////// destructor ///////////////
 public:
  ~multiset() noexcept = default;

  /////////////// member function ///////////////
 public:
  template <typename InputIt,
            typename = typename ::std::enable_if<
              ::ginshio::stl::is_input_iterator<InputIt>::value>::type>
  void assign(InputIt first, InputIt last) {
    c.assign_equal(first, last);
  }
  void assign(::std::initializer_list<value_type> ilist) {
    c.assign_equal(ilist);
  }
  template <typename Tree,
            typename = typename ::std::enable_if<::std::is_base_of<
              __container_base::_TreeBase<
                value_type, key_compare, key_of_value, allocator_type>,
              Tree>::value>::type>
  multiset& operator=(const set<key_type, key_compare,
                      allocator_type, Tree>& other) {
    c.assign_equal_copy_allocator(other.c);
    return *this;
  }
  template <typename Tree,
            typename = typename ::std::enable_if<::std::is_base_of<
              __container_base::_TreeBase<
                value_type, key_compare, key_of_value, allocator_type>,
              Tree>::value>::type>
  multiset& operator=(const multiset<key_type, key_compare,
                      allocator_type, Tree>& other) {
    c.assign_equal_copy_allocator(other.c);
    return *this;
  }
  multiset& operator=(multiset&& other) {
    c.assign_equal_move_allocator(::std::move(other.c));
    return *this;
  }
  multiset& operator=(::std::initializer_list<value_type> ilist) {
    c.assign_equal(ilist);
    return *this;
  }
  constexpr const container_type& get_container() const noexcept { return c; }
  constexpr const allocator_type get_allocator() const noexcept {
    return c.get_allocator();
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
  iterator insert(const value_type& value) { return c.emplace_equal(value); }
  iterator insert(value_type&& value) {
    return c.emplace_equal(::std::forward<value_type>(value));
  }
  template <typename P>
  iterator insert(P&& value) {
    return c.emplace_equal(::std::forward<P>(value));
  }
  iterator insert(const_iterator hint, const value_type& value) {
    return c.emplace_hint_equal(hint, value);
  }
  iterator insert(const_iterator hint, value_type&& value) {
    return c.emplace_hint_equal(hint, ::std::forward<value_type>(value));
  }
  template <typename P>
  iterator insert(const_iterator hint, P&& value) {
    return c.emplace_hint_equal(hint, ::std::forward<P>(value));
  }
  template <typename InputIt,
            typename = typename ::std::enable_if<
              ::ginshio::stl::is_input_iterator<InputIt>::value>::type>
  void insert(InputIt first, InputIt last) {
    c.insert_equal(first, last);
  }
  void insert(::std::initializer_list<value_type> ilist) {
    for (auto& value : ilist) {
      c.emplace_equal(value);
    }
  }
  // TODO: insert_return_type insert(node_type&& nh);
  // TODO: iterator insert(const_iterator hint, node_type&& nh);
  template <typename... Args>
  iterator emplace(Args&&... args) {
    return c.emplace_equal(::std::forward<Args>(args)...);
  }
  template <typename... Args>
  iterator emplace_hint(const_iterator hint, Args&&... args) {
    return c.emplace_hint_equal(hint, std::forward<Args>(args)...);
  }
  iterator erase(const_iterator pos) { return c.erase(pos); }
  iterator erase(const_iterator first, const_iterator last) {
    return c.erase(first, last);
  }
  size_type erase(const key_type& key) { return c.erase(key); }
  node_type extract(const_iterator pos) { return c.extract(pos); }
  node_type extract(const key_type& key) { return c.extract(key); }
  void swap(multiset& other) { c.swap(other.c); }
  template <typename Tree,
            typename = typename ::std::enable_if<
              ::std::is_base_of<
                __container_base::_TreeBase<
                  value_type, key_compare, key_of_value, allocator_type>,
                Tree>::value &&
              !std::is_same<Tree, Container>::value>::type>
  void merge(const multiset<key_type, key_compare,
             allocator_type, Tree>& other) {
    c.merge_equal(other.c);
  }
  void merge(multiset& other) { c.merge_equal(::std::move(other.c)); }
  void merge(multiset&& other) { c.merge_equal(::std::move(other.c)); }
  template <typename Tree,
            typename = typename ::std::enable_if<
              ::std::is_base_of<
                __container_base::_TreeBase<
                  value_type, key_compare, key_of_value, allocator_type>,
                Tree>::value &&
              !std::is_same<Tree, Container>::value>::type>
  void merge(const set<key_type, key_compare, allocator_type, Tree>& other) {
    c.merge_equal(other.c);
  }
  void merge(set<key_type, key_compare,
             allocator_type, container_type>& other) {
    c.merge_equal(::std::move(other.c));
  }
  void merge(set<key_type, key_compare,
             allocator_type, container_type>&& other) {
    c.merge_equal(::std::move(other.c));
  }

  /////////////// find ///////////////
 public:
  size_type count(const key_type& key) const { return c.count(key); }
  template <typename K>
  size_type count(const K& x) const { return c.count(x); }
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
template <typename Key>
using MultiSetIterator = typename multiset<Key>::iterator;
template <typename Key>
using MultiSetConstIterator = typename multiset<Key>::const_iterator;



///////////////////////// comparison operators ////////////////////////////
template <typename Key, typename Compare,
          typename Allocator, typename Container>
constexpr bool operator==(
    const multiset<Key, Compare, Allocator, Container>& lhs,
    const multiset<Key, Compare, Allocator, Container>& rhs) {
  return lhs.get_container() == rhs.get_container();

}
template <typename Key, typename Compare,
          typename Allocator, typename Container>
constexpr bool operator!=(
    const multiset<Key, Compare, Allocator, Container>& lhs,
    const multiset<Key, Compare, Allocator, Container>& rhs) {
  return lhs.get_container() != rhs.get_container();
}
template <typename Key, typename Compare,
          typename Allocator, typename Container>
constexpr bool operator<(
    const multiset<Key, Compare, Allocator, Container>& lhs,
    const multiset<Key, Compare, Allocator, Container>& rhs) {
  return lhs.get_container() < rhs.get_container();
}
template <typename Key, typename Compare,
          typename Allocator, typename Container>
constexpr bool operator>(
    const multiset<Key, Compare, Allocator, Container>& lhs,
    const multiset<Key, Compare, Allocator, Container>& rhs) {
  return lhs.get_container() > rhs.get_container();
}
template <typename Key, typename Compare,
          typename Allocator, typename Container>
constexpr bool operator<=(
    const multiset<Key, Compare, Allocator, Container>& lhs,
    const multiset<Key, Compare, Allocator, Container>& rhs) {
  return !(rhs.get_container() < lhs.get_container());
}
template <typename Key, typename Compare,
          typename Allocator, typename Container>
constexpr bool operator>=(
    const multiset<Key, Compare, Allocator, Container>& lhs,
    const multiset<Key, Compare, Allocator, Container>& rhs) {
  return !(lhs.get_container() < rhs.get_container());
}



///////////////////////// specialization /////////////////////////
template <typename Key, typename Compare,
          typename Allocator, typename Container>
inline void swap(multiset<Key, Compare, Allocator, Container>& lhs,
                 multiset<Key, Compare, Allocator, Container>& rhs) {
  lhs.swap(rhs);
}

template <typename Key, typename Compare,
          typename Allocator, typename Container, typename Pred>
auto erase_if(multiset<Key, Compare, Allocator, Container>& c, Pred pred) ->
    typename multiset<Key, Compare, Allocator, Container>::size_type {
  return erase_if(c.get_container(), pred);
}

}  // namespace stl
}  // namespace ginshio





namespace std {
///////////////////////// specialization /////////////////////////
template <typename Key, typename Compare,
          typename Allocator, typename Container>
inline void swap(
    ::ginshio::stl::multiset<Key, Compare, Allocator, Container>& lhs,
    ::ginshio::stl::multiset<Key, Compare, Allocator, Container>& rhs) {
  lhs.swap(rhs);
}

template <typename Key, typename Compare,
          typename Allocator, typename Container, typename Pred>
auto erase_if(
    ::ginshio::stl::multiset<Key, Compare, Allocator, Container>& c,
    Pred pred) ->
    typename ::ginshio::stl::multiset<Key, Compare,
                                      Allocator, Container>::size_type {
  return ::ginshio::stl::erase_if(c.get_container(), pred);
}
}  // namespace std

#endif  // GINSHIO_STL__CONTAINER_STL_MULTISET_HH_
