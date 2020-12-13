#ifndef GINSHIO_STL__BASE_STL_ITERATOR_TRAITS_HH_
#define GINSHIO_STL__BASE_STL_ITERATOR_TRAITS_HH_ 1

#include <type_traits>
#include <iterator>

namespace ginshio {
namespace stl {

struct contiguous_iterator_tag : public ::std::random_access_iterator_tag {};

template <typename T, typename = void>
struct is_iterator : public ::std::false_type {};
template <typename T>
struct is_iterator<
  T, typename ::std::enable_if<
       ::std::is_base_of<::std::input_iterator_tag,
                          typename ::std::iterator_traits<T>::iterator_category
                          >::value ||
       ::std::is_same<::std::output_iterator_tag,
                       typename ::std::iterator_traits<T>::iterator_category
                       >::value>::type>
    : public ::std::true_type {};

template <typename T, typename = void>
struct is_input_iterator : public ::std::false_type {};
template <typename T>
struct is_input_iterator<
  T, typename ::std::enable_if<
       ::std::is_base_of<::std::input_iterator_tag,
                          typename ::std::iterator_traits<T>::iterator_category
                          >::value>::type>
    : public ::std::true_type {};

template <typename T, typename = void>
struct is_forward_iterator : public ::std::false_type {};
template <typename T>
struct is_forward_iterator<
  T, typename ::std::enable_if<
       ::std::is_base_of<::std::forward_iterator_tag,
                          typename ::std::iterator_traits<T>::iterator_category
                          >::value>::type>
    : public ::std::true_type {};

template <typename T, typename = void>
struct is_bidirectional_iterator : public ::std::false_type {};
template <typename T>
struct is_bidirectional_iterator<
  T, typename ::std::enable_if<
       ::std::is_base_of<::std::bidirectional_iterator_tag,
                          typename ::std::iterator_traits<T>::iterator_category
                          >::value>::type>
    : public ::std::true_type {};

template <typename T, typename = void>
struct is_random_access_iterator : public ::std::false_type {};
template <typename T>
struct is_random_access_iterator<
  T, typename ::std::enable_if<
       ::std::is_base_of<::std::random_access_iterator_tag,
                          typename ::std::iterator_traits<T>::iterator_category
                          >::value>::type>
    : public ::std::true_type {};

template <typename T, typename = void>
struct is_contiguous_iterator : public ::std::false_type {};
#if __cplusplus > 202001L
template <typename T>
struct is_contiguous_iterator<
  T, typename ::std::enable_if<
       ::std::is_base_of<::ginshio::stl::contiguous_iterator_tag,
                          typename ::std::iterator_traits<T>::iterator_category
                          >::value ||
       ::std::is_base_of<::std::contiguous_iterator_tag,
                          typename ::std::iterator_traits<T>::iterator_category
                          >::value ||
       ::std::is_pointer<T>::value>::type*>
    : public ::std::true_type {};
#else // C++11
template <typename T>
struct is_contiguous_iterator<
  T, typename ::std::enable_if<
       ::std::is_base_of<::ginshio::stl::contiguous_iterator_tag,
                          typename ::std::iterator_traits<T>::iterator_category
                          >::value ||
       ::std::is_pointer<T>::value>::type>
    : public ::std::true_type {};
#endif

template <typename T, typename = void>
struct is_output_iterator : public ::std::false_type {};
template <typename T>
struct is_output_iterator<
  T, typename ::std::enable_if<
       ::std::is_same<::std::output_iterator_tag,
                       typename ::std::iterator_traits<T>::iterator_category
                       >::value ||
       ::ginshio::stl::is_forward_iterator<T>::value>::type>
    : public ::std::true_type {};

} // namespace stl
} // namespace ginshio

#endif // GINSHIO_STL__BASE_STL_ITERATOR_TRAITS_HH_
