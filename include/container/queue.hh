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


#ifndef GINSHIO_STL__QUEUE_HH_
#define GINSHIO_STL__QUEUE_HH_ 1

namespace ginshio {
namespace stl {

/////////////// queue ///////////////
template <typename T, class Container = ginshio::stl::deque<T>>
class queue {
  /////////////// private type ///////////////
 private:
  template <typename _Alloc>
  using _UseAlloc = typename
      std::enable_if<std::uses_allocator<Container, _Alloc>::value>::type*;
  /////////////// define type ///////////////
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
  queue() = default;
  explicit queue(const Container& cont) : c(cont) {}
  explicit queue(Container&& cont) : c(std::move(cont)) {}
  queue(const queue& other) : c(other.c) {}
  queue(queue&& other) noexcept : c(std::move(other.c)) {}
  template <typename Alloc, typename = _UseAlloc<Alloc>>
  explicit queue(const Alloc& alloc) : c(alloc) {}
  template <typename Alloc, typename = _UseAlloc<Alloc>>
  explicit queue(const Container& cont, const Alloc& alloc) : c(cont, alloc) {}
  template <typename Alloc, typename = _UseAlloc<Alloc>>
  explicit queue(Container&& cont, const Alloc& alloc) :
      c(std::move(cont), alloc) {}
  template <typename Alloc, typename = _UseAlloc<Alloc>>
  explicit queue(const queue& cont, const Alloc& alloc) : c(cont.c, alloc) {}
  template <typename Alloc, typename = _UseAlloc<Alloc>>
  explicit queue(queue&& cont, const Alloc& alloc) : c(cont.c, alloc) {}

  /////////////// destructor function ///////////////
 public:
  ~queue() noexcept = default;

  /////////////// member function ///////////////
 public:
  queue& operator=(const queue& other) {
    if (this != &other) {
      static_cast<void>(this->c.operator=(other.c));
    }
    return *this;
  }
  queue& operator=(queue&& other) {
    static_cast<void>(this->c.operator=(std::move(other.c)));
    return *this;
  }
  constexpr const container_type& get_container() const noexcept { return c; }

  /////////////// element access ///////////////
 public:
  reference front() { return c.front(); }
  constexpr const_reference front() const { return c.front(); }
  reference back() { return c.back(); }
  constexpr const_reference back() const { return c.back(); }

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
  void pop() { c.pop_front(); }
  void swap(queue& other) noexcept { c.swap(other.c); }
};

///////////////////////// queue comparison operators /////////////////////////
template <typename T, typename Container>
constexpr bool operator==(const queue<T, Container>& lhs,
                          const queue<T, Container>& rhs) {
  return lhs.get_container() == rhs.get_container();
}
template <typename T, typename Container>
constexpr bool operator!=(const queue<T, Container>& lhs,
                          const queue<T, Container>& rhs) {
  return lhs.get_container() != rhs.get_container();
}
template <typename T, typename Container>
constexpr bool operator<(const queue<T, Container>& lhs,
                         const queue<T, Container>& rhs) {
  return lhs.get_container() < rhs.get_container();
}
template <typename T, typename Container>
constexpr bool operator>(const queue<T, Container>& lhs,
                         const queue<T, Container>& rhs) {
  return lhs.get_container() > rhs.get_container();
}
template <typename T, typename Container>
constexpr bool operator<=(const queue<T, Container>& lhs,
                          const queue<T, Container>& rhs) {
  return !(rhs.get_container() < lhs.get_container());
}
template <typename T, typename Container>
constexpr bool operator>=(const queue<T, Container>& lhs,
                          const queue<T, Container>& rhs) {
  return !(lhs.get_container() < rhs.get_container());
}



///////////////////////// speecialization /////////////////////////
template <typename T, typename Container>
inline void swap(const queue<T, Container>& lhs,
                 const queue<T, Container>& rhs) {
  lhs.swap(rhs);
}

template <typename T, typename Container, typename Allocator>
struct uses_allocator<queue<T, Container>, Allocator> :
      public typename std::uses_allocator<Container, Allocator>::type {};

} // namespace stl
} // namespace ginshio



namespace std {
///////////////////////// speecialization /////////////////////////
template <typename T, typename Container>
inline void swap(const ginshio::stl::queue<T, Container>& lhs,
                 const ginshio::stl::queue<T, Container>& rhs) {
  lhs.swap(rhs);
}

template <typename T, typename Container, typename Allocator>
struct uses_allocator<ginshio::stl::queue<T, Container>, Allocator> :
      public typename std::uses_allocator<Container, Allocator>::type {};
} // namespace std

#endif // GINSHIO_STL__QUEUE_HH_
