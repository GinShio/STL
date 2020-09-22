#ifndef GINSHIO_STL__ARRAY_HPP_
#define GINSHIO_STL__ARRAY_HPP_ 1

#include "base/stl_check.hh"
#include "base/stl_init.hh"

#include <cstddef>
#include <iterator>
#include <limits>
#include <utility>

namespace ginshio {
namespace stl {

///////////////////////// array /////////////////////////
template <typename T, std::size_t N>
class array {
  /////////////// member type ///////////////
 private:
  using _ArrayType = T[N];

 public:
  using value_type = T;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using iterator = value_type*;
  using const_iterator = const value_type*;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  /////////////// data member ///////////////
 public:
  _ArrayType data_;

  /////////////// element access ///////////////
 public:
  reference at(size_type pos) {
    __check_out_of_range("array::at", pos, N);
    return data_[pos];
  }
  const_reference at(size_type pos) const {
    __check_out_of_range("array::at", pos, N);
    return data_[pos];
  }
  reference operator[](size_type pos) { return data_[pos]; }
  constexpr const_reference operator[](size_type pos) const {
    return data_[pos];
  }
  reference front() { return *data_; }
  constexpr const_reference front() const { return *data_; }
  reference back() { return *(data_ + N - 1); }
  constexpr const_reference back() const { return *(data_ + N - 1); }
  pointer data() noexcept { return data_; }
  constexpr const_pointer data() const noexcept { return data_; }

  /////////////// iterator ///////////////
 public:
  iterator begin() noexcept { return data_; }
  constexpr const_iterator begin() const noexcept { return data_; }
  constexpr const_iterator cbegin() const noexcept { return data_; }
  iterator end() noexcept { return data_ + N; }
  constexpr const_iterator end() const noexcept { return data_ + N; }
  constexpr const_iterator cend() const noexcept { return data_ + N; }
  reverse_iterator rbegin() noexcept { return reverse_iterator(data_ + N); }
  constexpr const_reverse_iterator rbegin() const noexcept {
    return const_reverse_iterator(data_ + N);
  }
  constexpr const_reverse_iterator crbegin() const noexcept {
    return const_reverse_iterator(data_ + N);
  }
  reverse_iterator rend() noexcept { return reverse_iterator(data_); }
  constexpr const_reverse_iterator rend() const noexcept {
    return const_reverse_iterator(data_);
  }
  constexpr const_reverse_iterator crend() const noexcept {
    return const_reverse_iterator(data_);
  }

  /////////////// capacity ///////////////
 public:
  constexpr bool empty() const noexcept { return N == 0; }
  constexpr size_type size() const noexcept { return N; }
  constexpr size_type max_size() const noexcept { return N; }

  /////////////// modifier ///////////////
 public:
  void fill(const value_type& value) {
    static_cast<void>(fill_n(data_, N, value));
  }
  void swap(array& other) noexcept(noexcept(
      std::swap(std::declval<value_type&>(), std::declval<value_type&>()))) {
    std::swap_ranges(data_, data_ + N, other.data_);
  }
};



/////////////// relational operators ///////////////
template <typename T, std::size_t N>
constexpr bool operator==(const array<T, N>& lhs, const array<T, N>& rhs) {
  return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename T, std::size_t N>
inline bool operator!=(const array<T, N>& lhs, const array<T, N>& rhs) {
  return !(operator==(lhs, rhs));
}

template <typename T, std::size_t N>
inline bool operator<(const array<T, N>& lhs, const array<T, N>& rhs) {
  return std::lexicographical_compare(lhs.begin(), lhs.end(),
                                      rhs.begin(), rhs.end());
}

template <typename T, std::size_t N>
inline bool operator>(const array<T, N>& lhs, const array<T, N>& rhs) {
  return std::lexicographical_compare(rhs.begin(), rhs.end(),
                                      lhs.begin(), lhs.end());
}

template <typename T, std::size_t N>
inline bool operator<=(const array<T, N>& lhs, const array<T, N>& rhs) {
  return !(operator>(lhs, rhs));
}

template <typename T, std::size_t N>
inline bool operator>=(const array<T, N>& lhs, const array<T, N>& rhs) {
  return !(operator<(lhs, rhs));
}



///////////////////////// specialization /////////////////////////
template <typename T, std::size_t N>
inline void swap(array<T, N>& lhs, array<T, N>& rhs) {
  lhs.swap(rhs);
}

template <std::size_t I, typename T, std::size_t N>
constexpr T& get(array<T, N>& arr) noexcept {
  static_assert(I < N, "Index must be within the range of the array");
  return arr.data()[I];
}

template <std::size_t I, typename T, std::size_t N>
constexpr const T& get(const array<T, N>& arr) noexcept {
  static_assert(I < N, "Index must be within the range of the array");
  return arr.data()[I];
}

template <std::size_t I, typename T, std::size_t N>
constexpr T&& get(array<T, N>&& arr) noexcept {
  static_assert(I < N, "Index must be within the range of the array");
  return std::move(arr.data()[I]);
}

template <std::size_t I, typename T, std::size_t N>
constexpr const T&& get(const array<T, N>&& arr) noexcept {
  static_assert(I < N, "Index must be within the range of the array");
  return std::move(arr.data()[I]);
}



#if __cplusplus > 201103L
namespace __implement {
template <typename T, std::size_t N, std::size_t... I>
constexpr auto to_array(T (&a)[N], std::index_sequence<I...>) ->
    array<typename std::remove_cv<T>::type, N> {
  return {{a[I]...}};
}
template <typename T, std::size_t N, std::size_t... I>
constexpr auto to_array(T(&&a)[N], std::index_sequence<I...>) ->
    array<typename std::remove_cv<T>::type, N> {
  return {{std::move(a[I])...}};
}
}  // namespace __implement
template <typename T, std::size_t N>
constexpr auto to_array(T (&a)[N]) ->
    array<typename std::remove_cv<T>::type, N> {
  return __implement::to_array(a, std::make_index_sequence<N>());
}
template <typename T, std::size_t N>
constexpr auto to_array(T(&&a)[N]) ->
    array<typename std::remove_cv<T>::type, N> {
  return __implement::to_array(std::move(a), std::make_index_sequence<N>());
}
#else   // __cplusplus <= 201402L
template <typename T, std::size_t N>
auto to_array(T (&a)[N]) -> array<typename std::remove_cv<T>::type, N> {
  array<typename std::remove_cv<T>::type, N> arr;
  for (std::size_t i = 0; i < N; ++i) {
    arr[i] = a[i];
  }
  return arr;
}
template <typename T, std::size_t N>
auto to_array(T(&&a)[N]) -> array<typename std::remove_cv<T>::type, N> {
  array<typename std::remove_cv<T>::type, N> arr;
  for (std::size_t i = 0; i < N; ++i) {
    arr[i] = std::move(a[i]);
  }
  return arr;
}
#endif  // __cplusplus > 201402L

///////////////////////// helper class /////////////////////////
template <std::size_t I, class T>
struct tuple_element;
template <std::size_t I, typename T, std::size_t N>
struct tuple_element<I, array<T, N>> {
  static_assert(I < N, "Index must be within the range of the array");
  using type = T;
};

template <typename T>
struct tuple_size;
template <typename T, std::size_t N>
struct tuple_size<array<T, N>> : public std::integral_constant<std::size_t, N> {
};

}  // namespace stl
}  // namespace ginshio





namespace std {
///////////////////////// specialization /////////////////////////
template <typename T, size_t N>
inline void swap(ginshio::stl::array<T, N>& lhs,
                 ginshio::stl::array<T, N>& rhs) {
  lhs.swap(rhs);
}

///////////////////////// helper class /////////////////////////
template <std::size_t I, class T>
struct tuple_element;
template <std::size_t I, typename T, std::size_t N>
struct tuple_element<I, ginshio::stl::array<T, N>> {
  static_assert(I < N, "Index must be within the range of the array");
  using type = T;
};

template <typename T>
struct tuple_size;
template <typename T, size_t N>
struct tuple_size<ginshio::stl::array<T, N>>
    : public integral_constant<size_t, N> {};
}  // namespace std

#endif  // GINSHIO_STL__ARRAY_HPP_
