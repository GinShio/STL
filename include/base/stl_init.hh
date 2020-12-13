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

#ifndef GINSHIO_STL__BASE_STL_INIT_HH_
#define GINSHIO_STL__BASE_STL_INIT_HH_ 1

#include "base/stl_iterator_traits.hh"

#include <cstddef>
#include <cstring>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>

namespace ginshio {
namespace stl {

namespace __implement {
///////////////////////// construct /////////////////////////
template <typename _T, typename... _Args>
inline void __construct(_T* _ptr, _Args&&... _args) {
  ::new (static_cast<void*>(_ptr)) _T(::std::forward<_Args>(_args)...);
}


///////////////////////// destroy /////////////////////////
template <typename _T, bool _TRIVIAL>
struct _Destroy {
  _Destroy() = delete;
  static inline void __destroy(_T* _ptr) { _ptr->~_T(); }
};
template <typename _T>
struct _Destroy<_T, true> {
  _Destroy() = delete;
  static inline void __destroy(_T*) {}
};

template <typename _ForwardIt, bool _TRIVIAL,
          typename = typename ::std::enable_if<
            ::ginshio::stl::is_forward_iterator<_ForwardIt>::value>::type>
struct _DestroyRange {
  _DestroyRange() = delete;
  static void __destroy(_ForwardIt _first, _ForwardIt _last) {
    using _Type = typename ::std::iterator_traits<_ForwardIt>::value_type;
    for (; _first != _last; ++_first) {
      _Destroy<_Type, false>::__destroy(::std::addressof(*_first));
    }
  }
};
template <typename _ForwardIt>
struct _DestroyRange<_ForwardIt, true> {
  _DestroyRange() = delete;
  static inline void __destroy(_ForwardIt, _ForwardIt) {}
};

template <typename _ForwardIt, bool _TRIVIAL,
          typename = typename ::std::enable_if<
            ::ginshio::stl::is_forward_iterator<_ForwardIt>::value>::type>
struct _DestroyN {
  _DestroyN() = delete;
  static _ForwardIt __destroy(_ForwardIt _first, ::std::size_t _cnt) {
    using _Type = typename ::std::iterator_traits<_ForwardIt>::value_type;
    for (; _cnt > 0; --_cnt) {
      _Destroy<_Type, false>::__destroy(::std::addressof(*_first));
      ++_first;
    }
    return _first;
  }
};
template <typename _ForwardIt>
struct _DestroyN<_ForwardIt, true> {
  _DestroyN() = delete;
  static inline _ForwardIt __destroy(_ForwardIt _first, ::std::size_t _cnt) {
    std::advance(_first, _cnt);
    return _first;
  }
};

template <typename _T, bool _IsArray>
struct _DestroyAt {
  _DestroyAt() = delete;
  static inline void __destroy(_T* _ptr) {
    using _ValueType = typename ::std::iterator_traits<_T*>::value_type;
    static_assert(::std::is_destructible<_ValueType>::value,
                  "destroy: value type must be destructible");
    _Destroy<_ValueType,
             ::std::is_trivially_destructible<_ValueType>::value>::
        __destroy(std::addressof(*_ptr));
  }
};
template <typename _T>
struct _DestroyAt<_T, true> {
  _DestroyAt() = delete;
  static void __destroy(_T* _ptr) {
    for (auto& _elm : *_ptr) {
      _DestroyAt<_T, ::std::is_array<_T>::value>::
          __destroy(::std::addressof(_elm));
    }
  }
};
}  // namespace __implement





///////////////////////// construct /////////////////////////
template <typename T, typename... Args>
inline void construct(T* ptr, Args&&... args) {
  __implement::__construct(ptr, ::std::forward<Args>(args)...);
}

template <typename ForwardIt, typename... Args>
void construct(ForwardIt first, typename
               ::std::enable_if<::ginshio::stl::is_forward_iterator<
               ForwardIt>::value, ForwardIt>::type last,
               Args&&... args) {
  ForwardIt _cur = first;
  try {
    for (; _cur != last; ++_cur) {
      __implement::__construct(::std::addressof(*_cur),
                               ::std::forward<Args>(args)...);
    }
  } catch (...) {
    using _ValType = typename ::std::iterator_traits<ForwardIt>::value_type;
    __implement::_DestroyRange<
      ForwardIt, ::std::is_trivially_destructible<_ValType>::value>::
        __destroy(first, _cur);
    throw;
  }
}

template <typename RandomIt, typename... Args>
void construct(RandomIt first, typename
               ::std::enable_if<::ginshio::stl::is_random_access_iterator<
               RandomIt>::value, RandomIt>::type last,
               Args&&... args) {
  using _Dist = typename ::std::iterator_traits<RandomIt>::difference_type;
  RandomIt _cur = first;
  try {
    for (_Dist n = last - first; n > 0; --n) {
      __implement::__construct(::std::addressof(*_cur),
                               ::std::forward<Args>(args)...);
      ++_cur;
    }
  } catch (...) {
    using _ValType = typename ::std::iterator_traits<RandomIt>::value_type;
    __implement::_DestroyRange<
      RandomIt, ::std::is_trivially_destructible<_ValType>::value>::
        __destroy(first, _cur);
    throw;
  }
}

template <typename ForwardIt, typename... Args>
void construct_n(typename ::std::enable_if<::ginshio::stl::is_forward_iterator<
                 ForwardIt>::value, ForwardIt>::type first,
                 ::std::size_t count, Args&&... args) {
  ForwardIt _cur = first;
  try {
    for (; count > 0; --count) {
      __implement::__construct(::std::addressof(*_cur),
                               ::std::forward<Args>(args)...);
      ++_cur;
    }
  } catch (...) {
    using _ValType = typename ::std::iterator_traits<ForwardIt>::value_type;
    __implement::_DestroyRange<
      ForwardIt, ::std::is_trivially_destructible<_ValType>::value>::
        __destroy(first, _cur);
    throw;
  }
}



///////////////////////// destroy /////////////////////////
template <typename T>
inline void destroy(T* ptr) {
  using _ValueType = typename ::std::iterator_traits<T*>::value_type;
  static_assert(::std::is_destructible<_ValueType>::value,
                "destroy: value type must be destructible");
  __implement::_Destroy<
    _ValueType, ::std::is_trivially_destructible<_ValueType>::value>::
      __destroy(ptr);
}

template <typename ForwardIt>
inline void destroy(ForwardIt first, ForwardIt last) {
  using _ValueType = typename ::std::iterator_traits<ForwardIt>::value_type;
  static_assert(::std::is_destructible<_ValueType>::value,
                "destroy: value type must be destructible");
  __implement::_DestroyRange<
    ForwardIt, ::std::is_trivially_destructible<_ValueType>::value>::
      __destroy(first, last);
}

template <typename ForwardIt>
inline ForwardIt destroy_n(ForwardIt first, std::size_t n) {
  using _ValueType = typename ::std::iterator_traits<ForwardIt>::value_type;
  static_assert(::std::is_destructible<_ValueType>::value,
                "destroy: value type must be destructible");
  return __implement::_DestroyN<
    ForwardIt, ::std::is_trivially_destructible<_ValueType>::value>::
      __destroy(first, n);
}

template <typename T>
inline void destroy_at(T* ptr) {
  __implement::_DestroyAt<T, ::std::is_array<T>::value>::__destroy(ptr);
}



///////////////////////// initialized function /////////////////////////
namespace __implement {
template <typename _T, typename _U>
inline _U* __trivial_copy(_T* _first, _U* _result, const ::std::size_t& _cnt) {
  return static_cast<_U*>(::std::memmove(_result, _first, sizeof(_U) * _cnt));
}

template <typename _T, typename _U>
inline _T* __trivial_fill(_T* _first, const _U& _value,
                          const ::std::size_t& _cnt) {
  _T* _cur = _first;
  for (::std::size_t _n = _cnt; _n > 0; --_n) {
    memmove(_cur, &_value, sizeof(_U));
    ++_cur;
  }
  return _first;
}

template <typename _InputIt, typename _OutputIt, bool _MOVE, bool _TRIVIAL,
          typename = typename ::std::enable_if<
            ::ginshio::stl::is_input_iterator<_InputIt>::value>::type,
          typename = typename ::std::enable_if<
            ::ginshio::stl::is_output_iterator<_OutputIt>::value>::type>
struct _Copy {
  _Copy() = delete;
  static _OutputIt __copy(_InputIt _first, _InputIt _last, _OutputIt _result) {
    for (; _first != _last; ++_first, ++_result) {
      *_result = *_first;
    }
    return _result;
  }
};
template <typename _InputIt, typename _OutputIt>
struct _Copy<_InputIt, _OutputIt, true, false> {
  _Copy() = delete;
  static _OutputIt __copy(_InputIt _first, _InputIt _last, _OutputIt _result) {
    for (; _first != _last; ++_first, ++_result) {
      *_result = ::std::move(*_first);
    }
    return _result;
  }
};
template <typename _ContiIt1, typename _ContiIt2, bool _MOVE>
struct _Copy<
  _ContiIt1, _ContiIt2, _MOVE, true,
  typename ::std::enable_if<
    ::ginshio::stl::is_contiguous_iterator<_ContiIt1>::value>::type,
  typename ::std::enable_if<
    ::ginshio::stl::is_contiguous_iterator<_ContiIt2>::value>::type> {
  _Copy() = delete;
  static inline _ContiIt2 __copy(_ContiIt1 _first, _ContiIt1 _last,
                                 _ContiIt2 _result) {
    using _Dist = typename ::std::iterator_traits<_ContiIt2>::difference_type;
    const _Dist _dist = _last - _first;
    return __trivial_copy(_first, _result, _dist) + _dist;
  }
};

template <typename _BidirIt1, typename _BidirIt2, bool _MOVE, bool _TRIVIAL,
          typename = typename ::std::enable_if<
            ::ginshio::stl::is_bidirectional_iterator<_BidirIt1>::value>::type,
          typename = typename ::std::enable_if<
            ::ginshio::stl::is_bidirectional_iterator<_BidirIt2>::value>::type>
struct _CopyBackward {
  _CopyBackward() = delete;
  static _BidirIt2 __copy(_BidirIt1 _first, _BidirIt1 _last,
                          _BidirIt2 _result) {
    while (_first != _last) {
      *--_result = *--_last;
    }
    return _result;
  }
};
template <typename _BidirIt1, typename _BidirIt2>
struct _CopyBackward<_BidirIt1, _BidirIt2, true, false> {
  _CopyBackward() = delete;
  static _BidirIt2 __copy(_BidirIt1 _first, _BidirIt1 _last,
                          _BidirIt2 _result) {
    while (_first != _last) {
      *--_result = std::move(*--_last);
    }
    return _result;
  }
};
template <typename _ContiIt1, typename _ContiIt2, bool _MOVE>
struct _CopyBackward<
  _ContiIt1, _ContiIt2, _MOVE, true,
  typename ::std::enable_if<
    ::ginshio::stl::is_contiguous_iterator<_ContiIt1>::value>::type,
  typename ::std::enable_if<
    ::ginshio::stl::is_contiguous_iterator<_ContiIt2>::value>::type> {
  _CopyBackward() = delete;
  static inline _ContiIt2 __copy(_ContiIt1 _first, _ContiIt1 _last,
                                 _ContiIt2 _result) {
    using _Dist = typename ::std::iterator_traits<_ContiIt2>::difference_type;
    const _Dist _dist = _last - _first;
    return __trivial_copy(_first, _result - _dist, _dist);
  }
};

template <typename _InputIt, typename _OutputIt, bool _MOVE, bool _TRIVIAL,
          typename = typename ::std::enable_if<
            ::ginshio::stl::is_bidirectional_iterator<_InputIt>::value>::type,
          typename = typename ::std::enable_if<
            ::ginshio::stl::is_bidirectional_iterator<_OutputIt>::value>::type>
struct _CopyN {
  _CopyN() = delete;
  static std::pair<_InputIt, _OutputIt> __copy(
      _InputIt _first, ::std::size_t _cnt, _OutputIt _result) {
    for (; _cnt > 0; --_cnt) {
      *_result = *_first;
      ++_first, ++_result;
    }
    return {_first, _result};
  }
};
template <typename _InputIt, typename _OutputIt>
struct _CopyN<_InputIt, _OutputIt, true, false> {
  _CopyN() = delete;
  static std::pair<_InputIt, _OutputIt> __copy(
      _InputIt _first, ::std::size_t _cnt, _OutputIt _result) {
    for (; _cnt > 0; --_cnt) {
      *_result = std::move(*_first);
      ++_first, ++_result;
    }
    return {_first, _result};
  }
};
template <typename _ContiIt1, typename _ContiIt2, bool _MOVE>
struct _CopyN<
  _ContiIt1, _ContiIt2, _MOVE, true,
  typename ::std::enable_if<
    ::ginshio::stl::is_contiguous_iterator<_ContiIt1>::value>::type,
  typename ::std::enable_if<
    ::ginshio::stl::is_contiguous_iterator<_ContiIt2>::value>::type> {
  _CopyN() = delete;
  static inline std::pair<_ContiIt1, _ContiIt2> __copy(
      _ContiIt1 _first, ::std::size_t _cnt, _ContiIt2 _result) {
    return {_first + _cnt, __trivial_copy(_first, _result, _cnt) + _cnt};
  }
};

template <typename _ForwardIt, typename _T, bool _TRIVIAL,
          typename = typename ::std::enable_if<
            ::ginshio::stl::is_forward_iterator<_ForwardIt>::value>::type>
struct _Fill {
  _Fill() = delete;
  static void __fill(_ForwardIt _first, _ForwardIt _last, const _T& _value) {
    for (; _first != _last; ++_first) {
      *_first = _value;
    }
  }
};
template <typename _ContiIt, typename _T>
struct _Fill<_ContiIt, _T, true, typename
             ::std::enable_if<::ginshio::stl::is_contiguous_iterator<
               _ContiIt>::value>::type> {
  _Fill() = delete;
  static inline void __fill(_ContiIt _first, _ContiIt _last, const _T& _value) {
    static_cast<void>(__trivial_fill(_first, _value, _last - _first));
  }
};

template <typename _OutputIt, typename _T, bool _TRIVIAL,
          typename = typename ::std::enable_if<
            ::ginshio::stl::is_output_iterator<_OutputIt>::value>::type>
struct _FillN {
  _FillN() = delete;
  static _OutputIt __fill(_OutputIt _first, ::std::size_t _cnt,
                          const _T& _value) {
    for (; _cnt > 0; --_cnt) {
      *_first = _value;
      ++_first;
    }
    return _first;
  }
};
template <typename _ContiIt, typename _T>
struct _FillN<_ContiIt, _T, true, typename
              ::std::enable_if<::ginshio::stl::is_contiguous_iterator<
                _ContiIt>::value>::type> {
  _FillN() = delete;
  static inline _ContiIt __fill(_ContiIt _first, ::std::size_t _cnt,
                                const _T& _value) {
    return __trivial_fill(_first, _value, _cnt) + _cnt;
  }
};
}  // namespace __implement





template <typename InputIt, typename OutputIt>
inline OutputIt copy(InputIt first, InputIt last, OutputIt result) {
  using _InputType = typename ::std::iterator_traits<InputIt>::value_type;
  using _OutputType = typename ::std::iterator_traits<OutputIt>::value_type;
  constexpr bool _IS_TRIVIAL = ::std::is_same<_InputType, _OutputType>::value &&
      ::std::is_trivially_copyable<_InputType>::value &&
      ::std::is_pointer<InputIt>::value &&
      ::std::is_pointer<OutputIt>::value;
  return __implement::_Copy<InputIt, OutputIt, false, _IS_TRIVIAL>::
      __copy(first, last, result);
}

template <typename BidirIt1, typename BidirIt2>
inline BidirIt2 copy_backward(BidirIt1 first, BidirIt1 last, BidirIt2 result) {
  static_assert(::ginshio::stl::is_bidirectional_iterator<BidirIt1>::value &&
                ::ginshio::stl::is_bidirectional_iterator<BidirIt2>::value,
                "Iterator must meet the requirements of " \
                "the Bidirectional Iterator");
  using _InputType = typename ::std::iterator_traits<BidirIt1>::value_type;
  using _OutputType = typename ::std::iterator_traits<BidirIt2>::value_type;
  constexpr bool _IS_TRIVIAL = ::std::is_same<_InputType, _OutputType>::value &&
      ::std::is_trivially_copyable<_InputType>::value &&
      ::std::is_pointer<BidirIt1>::value &&
      ::std::is_pointer<BidirIt2>::value;
  return __implement::_CopyBackward<BidirIt1, BidirIt2, false, _IS_TRIVIAL>::
      __copy(first, last, result);
}

template <typename InputIt, typename OutputIt>
inline std::pair<InputIt, OutputIt> copy_n(InputIt first, ::std::size_t count,
                                           OutputIt result) {
  using _InputType = typename ::std::iterator_traits<InputIt>::value_type;
  using _OutputType = typename ::std::iterator_traits<OutputIt>::value_type;
  constexpr bool _IS_TRIVIAL = ::std::is_same<_InputType, _OutputType>::value &&
      ::std::is_trivially_copyable<_InputType>::value &&
      ::std::is_pointer<InputIt>::value &&
      ::std::is_pointer<OutputIt>::value;
  return __implement::_CopyN<InputIt, OutputIt, false, _IS_TRIVIAL>::
      __copy(first, count, result);
}

template <typename InputIt, typename OutputIt>
inline OutputIt move(InputIt first, InputIt last, OutputIt result) {
  using _InputType = typename ::std::iterator_traits<InputIt>::value_type;
  using _OutputType = typename ::std::iterator_traits<OutputIt>::value_type;
  constexpr bool _IS_TRIVIAL = ::std::is_same<_InputType, _OutputType>::value &&
      ::std::is_trivially_copyable<_InputType>::value &&
      ::std::is_pointer<InputIt>::value &&
      ::std::is_pointer<OutputIt>::value;
  return __implement::_Copy<InputIt, OutputIt, true, _IS_TRIVIAL>::
      __copy(first, last, result);
}

template <typename BidirIt1, typename BidirIt2>
inline BidirIt2 move_backward(BidirIt1 first, BidirIt1 last, BidirIt2 result) {
  static_assert(::ginshio::stl::is_bidirectional_iterator<BidirIt1>::value &&
                ::ginshio::stl::is_bidirectional_iterator<BidirIt2>::value,
                "Iterator must meet the requirements of " \
                "the Bidirectional Iterator");
  using _InputType = typename ::std::iterator_traits<BidirIt1>::value_type;
  using _OutputType = typename ::std::iterator_traits<BidirIt2>::value_type;
  constexpr bool _IS_TRIVIAL = ::std::is_same<_InputType, _OutputType>::value &&
      ::std::is_trivially_copyable<_InputType>::value &&
      ::std::is_pointer<BidirIt1>::value &&
      ::std::is_pointer<BidirIt2>::value;
  return __implement::_CopyBackward<BidirIt1, BidirIt2, true, _IS_TRIVIAL>::
      __copy(first, last, result);
}

template <typename InputIt, typename OutputIt>
inline std::pair<InputIt, OutputIt> move_n(InputIt first, ::std::size_t count,
                                           OutputIt result) {
  using _InputType = typename ::std::iterator_traits<InputIt>::value_type;
  using _OutputType = typename ::std::iterator_traits<OutputIt>::value_type;
  constexpr bool _IS_TRIVIAL = ::std::is_same<_InputType, _OutputType>::value &&
      ::std::is_trivially_copyable<_InputType>::value &&
      ::std::is_pointer<InputIt>::value &&
      ::std::is_pointer<OutputIt>::value;
  return __implement::_CopyN<InputIt, OutputIt, true, _IS_TRIVIAL>::
      __copy(first, count, result);
}

template <typename ForwardIt, typename T>
inline void fill(ForwardIt first, ForwardIt last, const T& value) {
  using _IterValType = typename ::std::iterator_traits<ForwardIt>::value_type;
  constexpr bool _IS_TRIVIAL = ::std::is_same<_IterValType, T>::value &&
      ::std::is_trivially_copyable<_IterValType>::value &&
      ::std::is_pointer<ForwardIt>::value;
  __implement::_Fill<ForwardIt, T, _IS_TRIVIAL>::__fill(first, last, value);
}

template <typename OutputIt, typename T>
inline OutputIt fill_n(OutputIt first, ::std::size_t count, const T& value) {
  using _IterValType = typename std::iterator_traits<OutputIt>::value_type;
  constexpr bool _IS_TRIVIAL = ::std::is_same<_IterValType, T>::value &&
      ::std::is_trivially_copyable<_IterValType>::value &&
      ::std::is_pointer<OutputIt>::value;
  return __implement::
      _FillN<OutputIt, T, _IS_TRIVIAL>::__fill(first, count, value);
}





///////////////////////// uninitialized function /////////////////////////
namespace __implement {
template <typename _InputIt, typename _ForwardIt, bool _MOVE, bool _TRIVIAL,
          typename = typename ::std::enable_if<
            ::ginshio::stl::is_input_iterator<_InputIt>::value>::type,
          typename = typename ::std::enable_if<
            ::ginshio::stl::is_forward_iterator<_ForwardIt>::value>::type>
struct _UninitCopy {
  _UninitCopy() = delete;
  static _ForwardIt __copy(_InputIt _first, _InputIt _last,
                           _ForwardIt _result) {
    _ForwardIt _cur = _result;
    try {
      for (; _first != _last; ++_first, ++_cur) {
        __construct(::std::addressof(*_cur), *_first);
      }
    } catch (...) {
      using _Type = typename ::std::iterator_traits<_ForwardIt>::value_type;
      _DestroyRange<_ForwardIt, ::std::is_trivially_destructible<_Type>::value>
          ::__destroy(_result, _cur);
      throw;
    }
    return _cur;
  }
};
template <typename _InputIt, typename _ForwardIt>
struct _UninitCopy<_InputIt, _ForwardIt, true, false> {
  _UninitCopy() = delete;
  static _ForwardIt __copy(_InputIt _first, _InputIt _last,
                           _ForwardIt _result) {
    _ForwardIt _cur = _result;
    try {
      for (; _first != _last; ++_first, ++_cur) {
        __construct(::std::addressof(*_cur), std::move(*_first));
      }
    } catch (...) {
      using _Type = typename ::std::iterator_traits<_ForwardIt>::value_type;
      _DestroyRange<_ForwardIt, ::std::is_trivially_destructible<_Type>::value>
          ::__destroy(_result, _cur);
      throw;
    }
    return _cur;
  }
};
template <typename _InputIt, typename _ForwardIt, bool _MOVE>
struct _UninitCopy<_InputIt, _ForwardIt, _MOVE, true> {
  _UninitCopy() = delete;
  static inline _ForwardIt __copy(_InputIt _first, _InputIt _last,
                                  _ForwardIt _result) {
    using _InputType = typename ::std::iterator_traits<_InputIt>::value_type;
    using _ForwardType =
        typename ::std::iterator_traits<_ForwardIt>::value_type;
    const bool _IS_TRIVIAL = ::std::is_same<_InputType, _ForwardType>::value &&
                             ::std::is_pointer<_InputIt>::value &&
                             ::std::is_pointer<_ForwardIt>::value;
    return _Copy<_InputIt, _ForwardIt, _MOVE, _IS_TRIVIAL>::
        __copy(_first, _last, _result);
  }
};

template <typename _InputIt, typename _ForwardIt, bool _MOVE, bool _TRIVIAL,
          typename = typename ::std::enable_if<
            ::ginshio::stl::is_input_iterator<_InputIt>::value>::type,
          typename = typename ::std::enable_if<
            ::ginshio::stl::is_forward_iterator<_ForwardIt>::value>::type>
struct _UninitCopyN {
  _UninitCopyN() = delete;
  static std::pair<_InputIt, _ForwardIt> __copy(
      _InputIt _first, ::std::size_t _cnt, _ForwardIt _result) {
    _ForwardIt _cur = _result;
    try {
      for (; _cnt > 0; --_cnt) {
        __construct(::std::addressof(*_cur), *_first);
        ++_first, ++_cur;
      }
    } catch (...) {
      using _Type = typename ::std::iterator_traits<_ForwardIt>::value_type;
      _DestroyRange<
        _ForwardIt, ::std::is_trivially_destructible<_Type>::value>::
          __destroy(_result, _cur);
      throw;
    }
    return {_first, _cur};
  }
};
template <typename _InputIt, typename _ForwardIt>
struct _UninitCopyN<_InputIt, _ForwardIt, true, false> {
  _UninitCopyN() = delete;
  static std::pair<_InputIt, _ForwardIt> __copy(
      _InputIt _first, ::std::size_t _cnt, _ForwardIt _result) {
    for (; _cnt > 0; --_cnt) {
      __construct(::std::addressof(*_result), ::std::move(*_first));
      ++_first, ++_result;
    }
    return {_first, _result};
  }
};
template <typename _InputIt, typename _ForwardIt, bool _MOVE>
struct _UninitCopyN<_InputIt, _ForwardIt, _MOVE, true> {
  _UninitCopyN() = delete;
  static inline std::pair<_InputIt, _ForwardIt> __copy(
      _InputIt _first, ::std::size_t _cnt, _ForwardIt _result) {
    using _InputType = typename ::std::iterator_traits<_InputIt>::value_type;
    using _ForwardType =
        typename ::std::iterator_traits<_ForwardIt>::value_type;
    constexpr bool _IS_TRIVIAL =
        ::std::is_same<_InputType, _ForwardType>::value &&
        ::std::is_pointer<_InputIt>::value &&
        ::std::is_pointer<_ForwardIt>::value;
    return _CopyN<_InputIt, _ForwardIt, _MOVE, _IS_TRIVIAL>::
        __copy(_first, _cnt, _result);
  }
};

template <typename _ForwardIt, typename _T, bool _TRIVIAL,
          typename = typename ::std::enable_if<
            ::ginshio::stl::is_forward_iterator<_ForwardIt>::value>::type>
struct _UninitFill {
  _UninitFill() = delete;
  static void __fill(_ForwardIt _first, _ForwardIt _last, const _T& _value) {
    _ForwardIt _cur = _first;
    try {
      for (; _cur != _last; ++_cur) {
        __construct(::std::addressof(*_cur), _value);
      }
    } catch (...) {
      using _Type = typename ::std::iterator_traits<_ForwardIt>::value_type;
      _DestroyRange<
        _ForwardIt, ::std::is_trivially_destructible<_Type>::value>::
          __destroy(_first, _cur);
      throw;
    }
  }
};
template <typename _RandomIt, typename _T>
struct _UninitFill<_RandomIt, _T, false, typename
                   ::std::enable_if<::ginshio::stl::is_forward_iterator<
                     _RandomIt>::value>::type> {
  _UninitFill() = delete;
  static void __fill(_RandomIt _first, _RandomIt _last, const _T& _value) {
    _RandomIt _cur = _first;
    using _Dist = typename ::std::iterator_traits<_RandomIt>::difference_type;
    try {
      for (_Dist _cnt = _last - _first; _cnt > 0; --_cnt) {
        __construct(::std::addressof(*_cur), _value);
        ++_cur;
      }
    } catch (...) {
      using _Type = typename ::std::iterator_traits<_RandomIt>::value_type;
      _DestroyRange<_RandomIt, ::std::is_trivially_destructible<_Type>::value>::
          __destroy(_first, _cur);
      throw;
    }
  }
};
template <typename _ForwardIt, typename _T>
struct _UninitFill<_ForwardIt, _T, true> {
  _UninitFill() = delete;
  static inline void __fill(_ForwardIt _first, _ForwardIt _last,
                            const _T& _value) {
    using _ForwardType =
        typename ::std::iterator_traits<_ForwardIt>::value_type;
    const bool _IS_TRIVIAL = ::std::is_same<_ForwardType, _T>::value &&
        ::std::is_trivially_copyable<_ForwardType>::value;
    _Fill<_ForwardIt, _T, _IS_TRIVIAL>::__fill(_first, _last, _value);
  }
};

template <typename _ForwardIt, typename _T, bool _MOVE,
          typename = typename ::std::enable_if<
            ::ginshio::stl::is_forward_iterator<_ForwardIt>::value>::type>
struct _UninitFillN {
  _UninitFillN() = delete;
  static _ForwardIt __fill(_ForwardIt _first, ::std::size_t _cnt,
                           const _T& _value) {
    _ForwardIt _cur = _first;
    try {
      for (; _cnt > 0; --_cnt) {
        __construct(::std::addressof(*_cur), _value);
        ++_cur;
      }
    } catch (...) {
      using _Type = typename ::std::iterator_traits<_ForwardIt>::value_type;
      _DestroyRange<_ForwardIt, ::std::is_trivially_destructible<_Type>::value>
          ::__destroy(_first, _cur);
      throw;
    }
    return _cur;
  }
};
template <typename _ForwardIt, typename _T>
struct _UninitFillN<_ForwardIt, _T, true> {
  _UninitFillN() = delete;
  static inline _ForwardIt __fill(_ForwardIt _first, ::std::size_t _cnt,
                                  const _T& _value) {
    using _ForwardType =
        typename ::std::iterator_traits<_ForwardIt>::value_type;
    const bool _IS_TRIVIAL = ::std::is_pointer<_ForwardIt>::value &&
        ::std::is_same<_ForwardType, _T>::value &&
        ::std::is_trivially_copyable<_ForwardType>::value;
    return _FillN<_ForwardIt, _T, true>::__fill(_first, _cnt, _value);
  }
};
}  // namespace __implement

template <typename InputIt, typename ForwardIt>
inline ForwardIt uninitialized_copy(InputIt first, InputIt last,
                                    ForwardIt result) {
  using _InputType = typename ::std::iterator_traits<InputIt>::value_type;
  using _ForwardType = typename ::std::iterator_traits<ForwardIt>::value_type;
  static_assert(::std::is_constructible<_ForwardType, const _InputType&>::value,
                "uninitialized_copy: is not constructible from input type");
  constexpr bool _IS_TRIVIAL = ::std::is_assignable<
    typename ::std::iterator_traits<ForwardIt>::reference,
    typename ::std::iterator_traits<InputIt>::reference>::value &&
      ::std::is_trivially_copyable<_InputType>::value &&
      ::std::is_trivially_copyable<_ForwardType>::value;
  return __implement::_UninitCopy<InputIt, ForwardIt, false, _IS_TRIVIAL>::
      __copy(first, last, result);
}

template <typename InputIt, typename ForwardIt>
inline ::std::pair<InputIt, ForwardIt> uninitialized_copy_n(
    InputIt first, ::std::size_t count, ForwardIt result) {
  using _InputType = typename ::std::iterator_traits<InputIt>::value_type;
  using _ForwardType = typename ::std::iterator_traits<ForwardIt>::value_type;
  constexpr bool _IS_TRIVIAL = ::std::is_assignable<
    typename ::std::iterator_traits<ForwardIt>::reference,
    typename ::std::iterator_traits<InputIt>::reference>::value &&
      ::std::is_trivially_copyable<_InputType>::value &&
      ::std::is_trivially_copyable<_ForwardType>::value;
  return __implement::_UninitCopyN<InputIt, ForwardIt, false, _IS_TRIVIAL>::
      __copy(first, count, result);
}

template <typename InputIt, typename ForwardIt>
inline ForwardIt uninitialized_move(InputIt first, InputIt last,
                                    ForwardIt result) {
  using _InputType = typename ::std::iterator_traits<InputIt>::value_type;
  using _ForwardType = typename ::std::iterator_traits<ForwardIt>::value_type;
  constexpr bool _IS_TRIVIAL = ::std::is_assignable<
    typename ::std::iterator_traits<ForwardIt>::reference,
    typename ::std::iterator_traits<InputIt>::reference>::value &&
      ::std::is_trivially_copyable<_InputType>::value &&
      ::std::is_trivially_copyable<_ForwardType>::value;
  return __implement::_UninitCopy<InputIt, ForwardIt, true, _IS_TRIVIAL>::
      __copy(first, last, result);
}

template <typename InputIt, typename ForwardIt>
inline ::std::pair<InputIt, ForwardIt> uninitialized_move_n(
    InputIt first, ::std::size_t count, ForwardIt result) {
  using _InputType = typename ::std::iterator_traits<InputIt>::value_type;
  using _ForwardType = typename ::std::iterator_traits<ForwardIt>::value_type;
  constexpr bool _IS_TRIVIAL = ::std::is_assignable<
    typename ::std::iterator_traits<ForwardIt>::reference,
    typename ::std::iterator_traits<InputIt>::reference>::value &&
      ::std::is_trivially_copyable<_InputType>::value &&
      ::std::is_trivially_copyable<_ForwardType>::value;
  return __implement::_UninitCopyN<InputIt, ForwardIt, true, _IS_TRIVIAL>::
      __copy(first, count, result);
}

template <typename ForwardIt, typename T>
void uninitialized_fill(ForwardIt first, ForwardIt last, const T& value) {
  using _ForwardType = typename ::std::iterator_traits<ForwardIt>::value_type;
  static_assert(::std::is_constructible<_ForwardType, const T&>::value,
                "uninitialized_fill: is not constructible from fill type");
  constexpr bool _IS_TRIVIAL = ::std::is_assignable<
    typename ::std::iterator_traits<ForwardIt>::reference,
    typename ::std::add_lvalue_reference<T>::type>::value &&
      ::std::is_trivially_copyable<_ForwardType>::value &&
      ::std::is_trivially_copyable<T>::value;
  __implement::_UninitFill<ForwardIt, T, _IS_TRIVIAL>::
      __fill(first, last, value);
}

template <typename ForwardIt, typename T>
ForwardIt uninitialized_fill_n(ForwardIt first, ::std::size_t count,
                               const T& value) {
  using _ForwardType = typename ::std::iterator_traits<ForwardIt>::value_type;
  static_assert(::std::is_constructible<_ForwardType, const T&>::value,
                "uninitialized_fill_n: is not constructible from fill type");
  constexpr bool _IS_TRIVIAL = ::std::is_assignable<
    typename ::std::iterator_traits<ForwardIt>::reference,
    typename ::std::add_lvalue_reference<T>::type>::value &&
      ::std::is_trivially_copyable<_ForwardType>::value &&
      ::std::is_trivially_copyable<T>::value;
  return __implement::_UninitFillN<ForwardIt, T, _IS_TRIVIAL>::
      __fill(first, count, value);
}

}  // namespace stl
}  // namespace ginshio

#endif  // GINSHIO_STL__BASE_STL_INIT_HH_
