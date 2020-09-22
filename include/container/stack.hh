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

#ifndef GINSHIO_STL__CONTAINER_STACK_HH_
#define GINSHIO_STL__CONTAINER_STACK_HH_ 1

#include "deque.hpp"

#include <memory>
#include <type_traits>

namespace ginshio {
namespace stl {

///////////////////////// stack /////////////////////////
template <typename T, class Container = ginshio::stl::deque<T>>
class stack {
  /////////////// private type ///////////////
 private:
  template <typename _Alloc>
  using _UseAlloc = typename std::enable_if<
      std::uses_allocator<Container, _Alloc>::value>::type*;
  /////////////// member type ///////////////
 public:
  using container_type = Container;
  using value_type = typename Container::value_type;
  using size_type = typename Container::size_type;
  using reference = typename Container::reference;
  using const_reference = typename Container::const_reference;

  /////////////// data member ///////////////
 protected:
  container_type c;

  /////////////// constructor function ///////////////
 public:
  template <typename _Cont = container_type, typename = typename
            std::enable_if<std::is_default_constructible<_Cont>::value>::type*>
  stack() : c() {}
  explicit stack(const Container& cont) : c(cont) {}
  explicit stack(Container&& cont) : c(std::move(cont)) {}
  stack(const stack& other) : c(other.c) {}
  stack(stack&& other) noexcept : c(std::move(other.c)) {}
  template <typename Alloc, typename = _UseAlloc<Alloc>>
  explicit stack(const Alloc& alloc) : c(alloc) {}
  template <typename Alloc, typename = _UseAlloc<Alloc>>
  explicit stack(const Container& cont, const Alloc& alloc) : c(cont, alloc) {}
  template <typename Alloc, typename = _UseAlloc<Alloc>>
  explicit stack(Container&& cont, const Alloc& alloc)
      : c(std::move(cont), alloc) {}
  template <typename Alloc, typename = _UseAlloc<Alloc>>
  explicit stack(const stack& cont, const Alloc& alloc) : c(cont.c, alloc) {}
  template <typename Alloc, typename = _UseAlloc<Alloc>>
  explicit stack(stack&& cont, const Alloc& alloc) : c(cont.c, alloc) {}

  /////////////// destructor function ///////////////
 public:
  ~stack() noexcept = default;

  /////////////// member function ///////////////
 public:
  stack& operator=(const stack& other) {
    if (this != &other) {
      static_cast<void>(this->c.operator=(other.c));
    }
    return *this;
  }
  stack& operator=(stack&& other) {
    static_cast<void>(this->c.operator=(std::move(other.c)));
    return *this;
  }
  constexpr const container_type& get_container() const noexcept { return c; }

  /////////////// element access ///////////////
 public:
  reference top() { return c.back(); }
  constexpr const_reference top() const { return c.back(); }

  /////////////// capacity ///////////////
 public:
  constexpr bool empty() const noexcept { return c.empty(); }
  constexpr size_type size() const noexcept { return c.size(); }
  constexpr size_type max_size() const noexcept { return c.max_size(); }
  void shrink_to_fit() { c.shrink_to_fit(); }
  constexpr size_type capacity() const noexcept { return c.capacity(); }

  /////////////// modifier function ///////////////
 public:
  void push(const value_type& value) { c.emplace_back(value); }
  void push(value_type&& value) {
    c.emplace_back(std::forward<value_type>(value));
  }
  template <typename... Args>
  void emplace(Args... args) {
    c.emplace_back(std::forward<Args>(args)...);
  }
  void pop() { c.pop_back(); }
  void swap(stack& other) noexcept { c.swap(other.c); }
};

///////////////////////// stack comparison operators /////////////////////////
template <typename T, typename Container>
constexpr bool operator==(const stack<T, Container>& lhs,
                          const stack<T, Container>& rhs) {
  return lhs.get_container() == rhs.get_container();
}
template <typename T, typename Container>
constexpr bool operator!=(const stack<T, Container>& lhs,
                          const stack<T, Container>& rhs) {
  return lhs.get_container() != rhs.get_container();
}
template <typename T, typename Container>
constexpr bool operator<(const stack<T, Container>& lhs,
                         const stack<T, Container>& rhs) {
  return lhs.get_container() < rhs.get_container();
}
template <typename T, typename Container>
constexpr bool operator>(const stack<T, Container>& lhs,
                         const stack<T, Container>& rhs) {
  return lhs.get_container() > rhs.get_container();
}
template <typename T, typename Container>
constexpr bool operator<=(const stack<T, Container>& lhs,
                          const stack<T, Container>& rhs) {
  return !(rhs.get_container() < lhs.get_container());
}
template <typename T, typename Container>
constexpr bool operator>=(const stack<T, Container>& lhs,
                          const stack<T, Container>& rhs) {
  return !(lhs.get_container() < rhs.get_container());
}



///////////////////////// specialization /////////////////////////
template <typename T, typename Container>
inline void swap(stack<T, Container>& lhs, stack<T, Container>& rhs) {
  lhs.swap(rhs);
}

}  // namespace stl
}  // namespace ginshio





namespace std {
///////////////////////// specialization /////////////////////////
template <typename T, typename Container>
inline void swap(ginshio::stl::stack<T, Container>& lhs,
                 ginshio::stl::stack<T, Container>& rhs) {
  lhs.swap(rhs);
}

template <typename T, typename Container, typename Allocator>
struct uses_allocator<ginshio::stl::stack<T, Container>, Allocator>
    : public std::uses_allocator<Container, Allocator>::type {};
}  // namespace std

#endif  // GINSHIO_STL__CONTAINER_STACK_HH_
