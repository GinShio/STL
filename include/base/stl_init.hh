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


#ifndef GINSHIO_STL__STL_INITIALIZED_HH_
#define GINSHIO_STL__STL_INITIALIZED_HH_ 1

#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>

#include <cstddef>
#include <cstring>

namespace ginshio {
namespace stl {

///////////////////////// construct /////////////////////////
namespace __implement {
template <typename _T, typename... _Args>
inline void __construct(_T* _ptr, _Args&&... _args) {
  ::new(static_cast<void*>(_ptr)) _T(std::forward<_Args>(_args)...);
}
} // namespace __implement

template <typename T, typename... Args>
inline void construct(T* ptr, Args&&... args) {
  __implement::__construct(ptr, std::forward<Args>(args)...);
}

template <typename ForwardIt, typename... Args>
void construct(ForwardIt first, ForwardIt last, Args&&... args) {
  for (; first != last; ++first) {
    __implement::__construct(std::addressof(*first),
                             std::forward<Args>(args)...);
  }
}

template <typename ForwardIt, typename... Args>
void construct_n(ForwardIt first, std::size_t count, Args&&... args) {
  for (std::size_t n = 0; n < count; ++n) {
    __implement::__construct(std::addressof(*first),
                             std::forward<Args>(args)...);
    ++first;
  }
}



///////////////////////// destroy /////////////////////////
namespace __implement {
template <typename _T, bool _IS_TRIVIAL>
struct _Destroy {
  _Destroy() = delete;
  static inline void __destroy(_T* _ptr) {
    _ptr->~_T();
  }
};
template <typename _T>
struct _Destroy<_T, true> {
  _Destroy() = delete;
  static inline void __destroy(_T*) {}
};

template <typename _ForwardIt, typename _Category, bool _IS_TRIVIAL>
struct _DestroyRange {
  _DestroyRange() = delete;
  static void __destroy(_ForwardIt _first, _ForwardIt _last) {
    using _Type = typename std::iterator_traits<_ForwardIt>::value_type;
    for (; _first != _last; ++_first) {
      _Destroy<_Type, false>::__destroy(std::addressof(*_first));
    }
  }
};
template <typename _ForwardIt>
struct _DestroyRange<_ForwardIt, std::random_access_iterator_tag, false> {
  _DestroyRange() = delete;
  static void __destroy(_ForwardIt _first, _ForwardIt _last) {
    using _Type = typename std::iterator_traits<_ForwardIt>::value_type;
    using _Dist = typename std::iterator_traits<_ForwardIt>::difference_type;
    for (_Dist _cnt = _last - _first; _cnt > 0; --_cnt) {
      _Destroy<_Type, false>::__destroy(std::addressof(*_first));
      ++_first;
    }
  }
};
template <typename _ForwardIt, typename _Category>
struct _DestroyRange<_ForwardIt, _Category, true> {
  _DestroyRange() = delete;
  static inline void __destroy(_ForwardIt, _ForwardIt) {}
};

template <typename _ForwardIt, bool _IS_TRIVIAL>
struct _DestroyN {
  _DestroyN() = delete;
  static _ForwardIt __destroy(_ForwardIt _first, std::size_t _cnt) {
    using _Type = typename std::iterator_traits<_ForwardIt>::value_type;
    for (; _cnt > 0; --_cnt) {
      _Destroy<_Type, false>::__destroy(std::addressof(*_first));
      ++_first;
    }
    return _first;
  }
};
template <typename _ForwardIt>
struct _DestroyN<_ForwardIt, true> {
  _DestroyN() = delete;
  static inline _ForwardIt __destroy(_ForwardIt _first, std::size_t _cnt) {
    std::advance(_first, _cnt);
    return _first;
  }
};

template <typename _T, bool _IsArray>
struct _DestroyAt {
  _DestroyAt() = delete;
  static void __destroy(_T* _ptr) {
    for (auto& _elm : *_ptr) {
      _DestroyAt<_T, std::is_array<_T>::value>::
          __destroy(std::addressof(_elm));
    }
  }
};
template <typename _T>
struct _DestroyAt<_T, false> {
  _DestroyAt() = delete;
  static inline void __destroy(_T* _ptr) {
    using _ValueType = typename std::iterator_traits<_T*>::value_type;
    _Destroy<_ValueType,
             !std::is_destructible<_ValueType>::value ||
             std::is_trivially_destructible<_ValueType>::value>::
        __destroy(std::addressof(*_ptr));
  }
};
} // namespace __implement

template <typename T>
inline void destroy(T* ptr) {
  using _ValueType = typename std::iterator_traits<T*>::value_type;
  static_assert(std::is_destructible<_ValueType>::value,
                "destroy: value type must be destructible");
  __implement::
      _Destroy<_ValueType, std::is_trivially_destructible<_ValueType>::value>::
      __destroy(ptr);
}

template <typename ForwardIt>
inline void destroy(ForwardIt first, ForwardIt last) {
  using _ValueType = typename std::iterator_traits<ForwardIt>::value_type;
  static_assert(std::is_destructible<_ValueType>::value,
                "destroy: value type must be destructible");
  using _Category = typename std::iterator_traits<ForwardIt>::iterator_category;
  __implement::
      _DestroyRange<ForwardIt, _Category,
                    std::is_trivially_destructible<_ValueType>::value>::
      __destroy(first, last);
}

template <typename ForwardIt>
inline ForwardIt destroy_n(ForwardIt first, std::size_t n) {
  using _ValueType = typename std::iterator_traits<ForwardIt>::value_type;
  static_assert(std::is_destructible<_ValueType>::value,
                "destroy_n: value type must be destructible");
  return __implement::
      _DestroyN<ForwardIt, std::is_trivially_destructible<_ValueType>::value>::
      __destroy(first, n);
}

template <typename T>
inline void destroy_at(T* ptr) {
  __implement::_DestroyAt<T, std::is_array<T>::value>::__destroy(ptr);
}



///////////////////////// initialized function /////////////////////////
namespace __implement {
template <typename _T, typename _U>
inline _U* __trivial_copy(_T* _first, _U* _result, const std::size_t& _cnt) {
  return static_cast<_U*>(std::memmove(_result, _first, sizeof(_U) * _cnt));
}

template <typename _T, typename _U>
inline _T* __trivial_fill(_T* _first, const _U& _value,
                          const std::size_t& _cnt) {
  return static_cast<_T*>(memset(_first, _value, _cnt));
}

template <typename _InputIt, typename _OutputIt,
          typename _Category, bool _IS_MOVE, bool _IS_TRIVIAL>
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
struct _Copy<_InputIt, _OutputIt,
             std::random_access_iterator_tag, false, false> {
  _Copy() = delete;
  static _OutputIt __copy(_InputIt _first, _InputIt _last, _OutputIt _result) {
    using _Dist = typename std::iterator_traits<_InputIt>::difference_type;
    for (_Dist _cnt = _last - _first; _cnt > 0; --_cnt) {
      *_result = *_first;
      ++_first, ++_result;
    }
    return _result;
  }
};
template <typename _InputIt, typename _OutputIt, typename _Category>
struct _Copy<_InputIt, _OutputIt, _Category, true, false> {
  _Copy() = delete;
  static _OutputIt __copy(_InputIt _first, _InputIt _last, _OutputIt _result) {
    for (; _first != _last; ++_first, ++_result) {
      *_result = std::move(*_first);
    }
    return _result;
  }
};
template <typename _InputIt, typename _OutputIt>
struct _Copy<_InputIt, _OutputIt,
             std::random_access_iterator_tag, true, false> {
  _Copy() = delete;
  static _OutputIt __copy(_InputIt _first, _InputIt _last, _OutputIt _result) {
    using _Dist = typename std::iterator_traits<_InputIt>::difference_type;
    for (_Dist _cnt = _last - _first; _cnt > 0; --_cnt) {
      *_result = std::move(*_first);
      ++_first, ++_result;
    }
    return _result;
  }
};
template <typename _InputIt, typename _OutputIt,
          typename _Category, bool _IS_MOVE>
struct _Copy<_InputIt, _OutputIt, _Category, _IS_MOVE, true> {
  _Copy() = delete;
  static inline _OutputIt __copy(_InputIt _first, _InputIt _last,
                                 _OutputIt _result) {
    const std::size_t _dist = _last - _first;
    return __trivial_copy(_first, _result, _dist) + _dist;
  }
};

template <typename _BidirIt1, typename _BidirIt2,
          typename _Category, bool _IS_MOVE, bool _IS_TRIVIAL>
struct _CopyBackward {
  _CopyBackward() = delete;
  static _BidirIt2 __copy(_BidirIt1 _first, _BidirIt1 _last,
                          _BidirIt2 _d_last) {
    while (_first != _last) {
      *--_d_last = *--_last;
    }
    return _d_last;
  }
};
template <typename _BidirIt1, typename _BidirIt2, typename _Category>
struct _CopyBackward<_BidirIt1, _BidirIt2, _Category, true, false> {
  _CopyBackward() = delete;
  static _BidirIt2 __copy(_BidirIt1 _first, _BidirIt1 _last,
                          _BidirIt2 _d_last) {
    while (_first != _last) {
      *--_d_last = std::move(*--_last);
    }
    return _d_last;
  }
};
template <typename _BidirIt1, typename _BidirIt2>
struct _CopyBackward<_BidirIt1, _BidirIt2,
                     std::random_access_iterator_tag, false, false> {
  _CopyBackward() = delete;
  static _BidirIt2 __copy(_BidirIt1 _first, _BidirIt1 _last,
                          _BidirIt2 _d_last) {
    using _Dist = typename std::iterator_traits<_BidirIt1>::difference_type;
    for (_Dist _n = _last - _first; _n > 0; --_n) {
      *--_d_last = *--_last;
    }
    return _d_last;
  }
};
template <typename _BidirIt1, typename _BidirIt2>
struct _CopyBackward<_BidirIt1, _BidirIt2,
                     std::random_access_iterator_tag, true, false> {
  _CopyBackward() = delete;
  static _BidirIt2 __copy(_BidirIt1 _first, _BidirIt1 _last,
                          _BidirIt2 _d_last) {
    using _Dist = typename std::iterator_traits<_BidirIt1>::difference_type;
    for (_Dist _n = _last - _first; _n > 0; --_n) {
      *--_d_last = std::move(*--_last);
    }
    return _d_last;
  }
};
template <typename _BidirIt1, typename _BidirIt2,
          typename _Category, bool _IS_MOVE>
struct _CopyBackward<_BidirIt1, _BidirIt2, _Category, _IS_MOVE, true> {
  _CopyBackward() = delete;
  static inline _BidirIt2 __copy(_BidirIt1 _first, _BidirIt1 _last,
                                 _BidirIt2 _d_last) {
    const std::size_t _dist = _last - _first;
    return __trivial_copy(_first, _d_last - _dist, _dist);
  }
};

template <typename _InputIt, typename _OutputIt,
          bool _IS_MOVE, bool _IS_TRIVIAL>
struct _CopyN {
  _CopyN() = delete;
  static std::pair<_InputIt, _OutputIt>
  __copy(_InputIt _first, std::size_t _cnt, _OutputIt _result) {
    for (std::size_t _n = 0; _n < _cnt; ++_n) {
      *_result = *_first;
      ++_first, ++_result;
    }
    return {_first, _result};
  }
};
template <typename _InputIt, typename _OutputIt>
struct _CopyN<_InputIt, _OutputIt, true, false> {
  _CopyN() = delete;
  static std::pair<_InputIt, _OutputIt>
  __copy(_InputIt _first, std::size_t _cnt, _OutputIt _result) {
    for (std::size_t _n = 0; _n < _cnt; ++_n) {
      *_result = std::move(*_first);
      ++_first, ++_result;
    }
    return {_first, _result};
  }
};
template <typename _InputIt, typename _OutputIt, bool _IS_MOVE>
struct _CopyN<_InputIt, _OutputIt, _IS_MOVE, true> {
  _CopyN() = delete;
  static inline std::pair<_InputIt, _OutputIt>
  __copy(_InputIt _first, std::size_t _cnt, _OutputIt _result) {
    return {_first + _cnt, __trivial_copy(_first, _result, _cnt) + _cnt};
  }
};

template <typename _ForwardIt, typename _T, typename _Category, bool _IS_BYTE>
struct _Fill {
  _Fill() = delete;
  static void __fill(_ForwardIt _first, _ForwardIt _last, const _T& _value) {
    for (; _first != _last; ++_first) {
      *_first = _value;
    }
  }
};
template <typename _ForwardIt, typename _T>
struct _Fill<_ForwardIt, _T, std::random_access_iterator_tag, false> {
  _Fill() = delete;
  static void __fill(_ForwardIt _first, _ForwardIt _last, const _T& _value) {
    using _Dist = typename std::iterator_traits<_ForwardIt>::difference_type;
    for (_Dist _cnt = _last - _first; _cnt > 0; --_cnt) {
      *_first = _value;
      ++_first;
    }
  }
};
template <typename _ForwardIt, typename _T, typename _Category>
struct _Fill<_ForwardIt, _T, _Category, true> {
  _Fill() = delete;
  static inline void __fill(_ForwardIt _first, _ForwardIt _last,
                            const _T& _value) {
    static_cast<void>(__trivial_fill(_first, _value, _last - _first));
  }
};

template <typename _OutputIt, typename _T, bool _IS_BYTE>
struct _FillN {
  _FillN() = delete;
  static _OutputIt __fill(_OutputIt _first, std::size_t _cnt,
                          const _T& _value) {
    for (std::size_t _n = 0; _n < _cnt; ++_n) {
      *_first = _value;
      ++_first;
    }
    return _first;
  }
};
template <typename _OutputIt, typename _T>
struct _FillN<_OutputIt, _T, true> {
  _FillN() = delete;
  static inline _OutputIt
  __fill(_OutputIt _first, std::size_t _cnt, const _T& _value) {
    return __trivial_fill(_first, _value, _cnt) + _cnt;
  }
};
} // namespace __implement

template <typename InputIt, typename OutputIt>
inline OutputIt copy(InputIt first, InputIt last, OutputIt result) {
  using _Category = typename std::iterator_traits<InputIt>::iterator_category;
  using _InputType = typename std::iterator_traits<InputIt>::value_type;
  using _OutputType = typename std::iterator_traits<OutputIt>::value_type;
  const bool _IS_TRIVIAL = std::is_same<_InputType, _OutputType>::value &&
      std::is_trivially_copyable<_InputType>::value &&
      std::is_pointer<InputIt>::value && std::is_pointer<OutputIt>::value;
  return __implement::_Copy<InputIt, OutputIt, _Category, false, _IS_TRIVIAL>::
      __copy(first, last, result);
}

template <typename BidirIt1, typename BidirIt2>
inline BidirIt2 copy_backward(BidirIt1 first, BidirIt1 last, BidirIt2 result) {
  using _Category = typename std::iterator_traits<BidirIt1>::iterator_category;
  static_assert(
      std::is_base_of<std::bidirectional_iterator_tag, _Category>::value &&
      std::is_base_of<std::bidirectional_iterator_tag,
      typename std::iterator_traits<BidirIt2>::iterator_category>::value,
      "Iterator must meet the requirements of the Bidirectional Iterator");
  using _InputType = typename std::iterator_traits<BidirIt1>::value_type;
  using _OutputType = typename std::iterator_traits<BidirIt2>::value_type;
  const bool _IS_TRIVIAL = std::is_same<_InputType, _OutputType>::value &&
      std::is_trivially_copyable<_InputType>::value &&
      std::is_pointer<BidirIt1>::value && std::is_pointer<BidirIt2>::value;
  return __implement::
      _CopyBackward<BidirIt1, BidirIt2, _Category, false, _IS_TRIVIAL>::
      __copy(first, last, result);
}

template <typename InputIt, typename OutputIt>
inline std::pair<InputIt, OutputIt>
copy_n(InputIt first, std::size_t count, OutputIt result) {
  using _InputType = typename std::iterator_traits<InputIt>::value_type;
  using _OutputType = typename std::iterator_traits<OutputIt>::value_type;
  const bool _IS_TRIVIAL = std::is_same<_InputType, _OutputType>::value &&
      std::is_trivially_copyable<_InputType>::value &&
      std::is_pointer<InputIt>::value && std::is_pointer<OutputIt>::value;
  return __implement::_CopyN<InputIt, OutputIt, false, _IS_TRIVIAL>::
      __copy(first, count, result);
}

template <typename InputIt, typename OutputIt>
inline OutputIt move(InputIt first, InputIt last, OutputIt result) {
  using _Category = typename std::iterator_traits<InputIt>::iterator_category;
  using _InputType = typename std::iterator_traits<InputIt>::value_type;
  using _OutputType = typename std::iterator_traits<OutputIt>::value_type;
  const bool _IS_TRIVIAL = std::is_same<_InputType, _OutputType>::value &&
      std::is_trivially_copyable<_InputType>::value &&
      std::is_pointer<InputIt>::value && std::is_pointer<OutputIt>::value;
  return __implement::_Copy<InputIt, OutputIt, _Category, true, _IS_TRIVIAL>::
      __copy(first, last, result);
}

template <typename BidirIt1, typename BidirIt2>
inline BidirIt2 move_backward(BidirIt1 first, BidirIt1 last, BidirIt2 result) {
  using _Category = typename std::iterator_traits<BidirIt1>::iterator_category;
  static_assert(
      std::is_base_of<std::bidirectional_iterator_tag, _Category>::value &&
      std::is_base_of<std::bidirectional_iterator_tag,
      typename std::iterator_traits<BidirIt2>::iterator_category>::value,
      "Iterator must meet the requirements of the Bidirectional Iterator");
  using _InputType = typename std::iterator_traits<BidirIt1>::value_type;
  using _OutputType = typename std::iterator_traits<BidirIt2>::value_type;
  const bool _IS_TRIVIAL = std::is_same<_InputType, _OutputType>::value &&
      std::is_trivially_copyable<_InputType>::value &&
      std::is_pointer<BidirIt1>::value && std::is_pointer<BidirIt2>::value;
  return __implement::
      _CopyBackward<BidirIt1, BidirIt2, _Category, true, _IS_TRIVIAL>::
      __copy(first, last, result);
}

template <typename InputIt, typename OutputIt>
inline std::pair<InputIt, OutputIt>
move_n(InputIt first, std::size_t count, OutputIt result) {
  using _InputType = typename std::iterator_traits<InputIt>::value_type;
  using _OutputType = typename std::iterator_traits<OutputIt>::value_type;
  const bool _IS_TRIVIAL = std::is_same<_InputType, _OutputType>::value &&
      std::is_trivially_copyable<_InputType>::value &&
      std::is_pointer<InputIt>::value && std::is_pointer<OutputIt>::value;
  return __implement::_CopyN<InputIt, OutputIt, true, _IS_TRIVIAL>::
      __copy(first, count, result);
}

template <typename ForwardIt, typename T>
inline void fill(ForwardIt first, ForwardIt last, const T& value) {
  using _Category = typename std::iterator_traits<ForwardIt>::iterator_category;
  using _IterValType = typename std::iterator_traits<ForwardIt>::reference;
  const bool _IS_TRIVIAL = std::is_trivially_copyable<_IterValType>::value &&
      std::is_pointer<ForwardIt>::value && sizeof(_IterValType) == 1 &&
      std::is_arithmetic<T>::value;
  __implement::_Fill<ForwardIt, T, _Category, _IS_TRIVIAL>::
      __fill(first, last, value);
}

template <typename OutputIt, typename T>
inline OutputIt fill_n(OutputIt first, std::size_t count, const T& value) {
  using _IterValType = typename std::iterator_traits<OutputIt>::value_type;
  const bool _IS_BYTE = std::is_trivially_copyable<_IterValType>::value &&
      std::is_pointer<OutputIt>::value && sizeof(_IterValType) == 1 &&
      std::is_arithmetic<T>::value;
  return __implement::
      _FillN<OutputIt, T, _IS_BYTE>::__fill(first, count, value);
}



///////////////////////// uninitialized function /////////////////////////
namespace __implement {
template <typename _InputIt, typename _ForwardIt,
          typename _Category, bool _IS_MOVE, bool _IS_TRIVIAL>
struct _UninitCopy {
  _UninitCopy() = delete;
  static _ForwardIt __copy(_InputIt _first, _InputIt _last,
                           _ForwardIt _result) {
    _ForwardIt _cur = _result;
    try {
      for (; _first != _last; ++_first, ++_cur) {
        __construct(std::addressof(*_cur), *_first);
      }
    } catch (...) {
      using _Type = typename std::iterator_traits<_ForwardIt>::value_type;
      _DestroyRange<_ForwardIt, _Category,
                    std::is_trivially_destructible<_Type>::value>::
          __destroy(_result, _cur);
      throw;
    }
    return _cur;
  }
};
template <typename _InputIt, typename _ForwardIt, typename _Category>
struct _UninitCopy<_InputIt, _ForwardIt, _Category, true, false> {
  _UninitCopy() = delete;
  static _ForwardIt __copy(_InputIt _first, _InputIt _last,
                           _ForwardIt _result) {
    _ForwardIt _cur = _result;
    try {
      for (; _first != _last; ++_first, ++_cur) {
        __construct(std::addressof(*_cur), std::move(*_first));
      }
    } catch (...) {
      using _Type = typename std::iterator_traits<_ForwardIt>::value_type;
      _DestroyRange<_ForwardIt, _Category,
                    std::is_trivially_destructible<_Type>::value>::
          __destroy(_result, _cur);
      throw;
    }
    return _cur;
  }
};
template <typename _InputIt, typename _ForwardIt>
struct _UninitCopy<_InputIt, _ForwardIt,
                   std::random_access_iterator_tag, false, false> {
  _UninitCopy() = delete;
  static _ForwardIt __copy(_InputIt _first, _InputIt _last,
                           _ForwardIt _result) {
    using _Dist = typename std::iterator_traits<_InputIt>::difference_type;
    _ForwardIt _cur = _result;
    try {
      for (_Dist _n = _last - _first; _n > 0; --_n) {
        __construct(std::addressof(*_cur), *_first);
        ++_first, ++_cur;
      }
    } catch (...) {
      using _Type = typename std::iterator_traits<_ForwardIt>::value_type;
      _DestroyRange<_ForwardIt, std::random_access_iterator_tag,
                    std::is_trivially_destructible<_Type>::value>::
          __destroy(_result, _cur);
      throw;
    }
    return _cur;
  }
};
template <typename _InputIt, typename _ForwardIt>
struct _UninitCopy<_InputIt, _ForwardIt,
                   std::random_access_iterator_tag, true, false> {
  _UninitCopy() = delete;
  static _ForwardIt __copy(_InputIt _first, _InputIt _last,
                           _ForwardIt _result) {
    using _Dist = typename std::iterator_traits<_InputIt>::difference_type;
    _ForwardIt _cur = _result;
    try {
      for (_Dist _n = _last - _first; _n > 0; --_n) {
        __construct(std::addressof(*_cur), std::move(*_first));
        ++_first, ++_cur;
      }
    } catch (...) {
      using _Type = typename std::iterator_traits<_ForwardIt>::value_type;
      _DestroyRange<_ForwardIt, std::random_access_iterator_tag,
                    std::is_trivially_destructible<_Type>::value>::
          __destroy(_result, _cur);
      throw;
    }
    return _cur;
  }
};
template <typename _InputIt, typename _ForwardIt,
          typename _Category, bool _IS_MOVE>
struct _UninitCopy<_InputIt, _ForwardIt, _Category, _IS_MOVE, true> {
  _UninitCopy() = delete;
  static inline _ForwardIt __copy(_InputIt _first, _InputIt _last,
                                  _ForwardIt _result) {
    using _InputType = typename std::iterator_traits<_InputIt>::value_type;
    using _ForwardType = typename std::iterator_traits<_ForwardIt>::value_type;
    const bool _IS_TRIVIAL = std::is_same<_InputType, _ForwardType>::value &&
        std::is_pointer<_InputIt>::value && std::is_pointer<_ForwardIt>::value;
    return _Copy<_InputIt, _ForwardIt, _Category, false, _IS_TRIVIAL>::
        __copy(_first, _last, _result);
  }
};

template <typename _InputIt, typename _ForwardIt,
          bool _IS_MOVE, bool _IS_TRIVIAL>
struct _UninitCopyN {
  _UninitCopyN() = delete;
  static std::pair<_InputIt, _ForwardIt>
  __copy(_InputIt _first, std::size_t _cnt, _ForwardIt _result) {
    _ForwardIt _cur = _result;
    try {
      for (std::size_t _n = 0; _n < _cnt; ++_n) {
        __construct(std::addressof(*_cur), *_first);
        ++_first, ++_cur;
      }
    } catch (...) {
      using _Type = typename std::iterator_traits<_ForwardIt>::value_type;
      using _Category = typename
          std::iterator_traits<_ForwardIt>::iterator_category;
      _DestroyRange<_ForwardIt, _Category,
                    std::is_trivially_destructible<_Type>::value>::
          __destroy(_result, _cur);
      throw;
    }
    return {_first, _cur};
  }
};
template <typename _InputIt, typename _ForwardIt>
struct _UninitCopyN<_InputIt, _ForwardIt, true, false> {
  _UninitCopyN() = delete;
  static std::pair<_InputIt, _ForwardIt>
  __copy(_InputIt _first, std::size_t _cnt, _ForwardIt _result) {
    _ForwardIt _cur = _result;
    try {
      for (std::size_t _n = 0; _n < _cnt; ++_n) {
        __construct(std::addressof(*_cur), std::move(*_first));
        ++_first, ++_cur;
      }
    } catch (...) {
      using _Type = typename std::iterator_traits<_ForwardIt>::value_type;
      using _Category = typename
          std::iterator_traits<_ForwardIt>::iterator_category;
      _DestroyRange<_ForwardIt, _Category,
                    std::is_trivially_destructible<_Type>::value>::
          __destroy(_result, _cur);
      throw;
    }
    return {_first, _cur};
  }
};
template <typename _InputIt, typename _ForwardIt, bool _IS_MOVE>
struct _UninitCopyN<_InputIt, _ForwardIt, _IS_MOVE, true> {
  _UninitCopyN() = delete;
  static inline std::pair<_InputIt, _ForwardIt>
  __copy(_InputIt _first, std::size_t _cnt, _ForwardIt _result) {
    using _InputType = typename std::iterator_traits<_InputIt>::value_type;
    using _ForwardType = typename std::iterator_traits<_ForwardIt>::value_type;
    const bool _IS_TRIVIAL = std::is_same<_InputType, _ForwardType>::value &&
        std::is_pointer<_InputIt>::value && std::is_pointer<_ForwardIt>::value;
    return _CopyN<_InputIt, _ForwardIt, _IS_MOVE, _IS_TRIVIAL>::
        __copy(_first, _cnt, _result);
  }
};

template <typename _ForwardIt, typename _T,
          typename _Category, bool _IS_TRIVIAL>
struct _UninitFill {
  _UninitFill() = delete;
  static void __fill(_ForwardIt _first, _ForwardIt _last, const _T& _value) {
    _ForwardIt _cur = _first;
    try {
      for (; _cur != _last; ++_cur) {
        __construct(std::addressof(*_cur), _value);
      }
    } catch (...) {
      using _Type = typename std::iterator_traits<_ForwardIt>::value_type;
      _DestroyRange<_ForwardIt, _Category,
                    std::is_trivially_destructible<_Type>::value>::
          __destroy(_first, _cur);
      throw;
    }
  }
};
template <typename _ForwardIt, typename _T>
struct _UninitFill<_ForwardIt, _T, std::random_access_iterator_tag, false> {
  _UninitFill() = delete;
  static void __fill(_ForwardIt _first, _ForwardIt _last, const _T& _value) {
    _ForwardIt _cur = _first;
    using _Dist = typename std::iterator_traits<_ForwardIt>::difference_type;
    try {
      for (_Dist _cnt = _last - _first; _cnt > 0; --_cnt) {
        __construct(std::addressof(*_cur), _value);
        ++_cur;
      }
    } catch (...) {
      using _Type = typename std::iterator_traits<_ForwardIt>::value_type;
      _DestroyRange<_ForwardIt, std::random_access_iterator_tag,
                    std::is_trivially_destructible<_Type>::value>::
          __destroy(_first, _cur);
      throw;
    }
  }
};
template <typename _ForwardIt, typename _T, typename _Category>
struct _UninitFill<_ForwardIt, _T, _Category, true> {
  _UninitFill() = delete;
  static inline void __fill(_ForwardIt _first, _ForwardIt _last,
                            const _T& _value) {
    using _ForwardType = typename std::iterator_traits<_ForwardIt>::value_type;
    const bool _IS_BYTE = std::is_pointer<_ForwardIt>::value &&
        sizeof(_ForwardType) == 1 && std::is_arithmetic<_T>::value;
    _Fill<_ForwardIt, _T, _Category, _IS_BYTE>::__fill(_first, _last, _value);
  }
};

template <typename _ForwardIt, typename _T, bool _IS_BYTE>
struct _UninitFillN {
  _UninitFillN() = delete;
  static _ForwardIt __fill(_ForwardIt _first, std::size_t _cnt,
                           const _T& _value) {
    _ForwardIt _cur = _first;
    try {
      for (std::size_t _n = 0; _n < _cnt; ++_n) {
        __construct(std::addressof(*_cur), _value);
        ++_cur;
      }
    } catch (...) {
      using _Type = typename std::iterator_traits<_ForwardIt>::value_type;
      using _Category = typename
          std::iterator_traits<_ForwardIt>::iterator_category;
      _DestroyRange<_ForwardIt, _Category,
                    std::is_trivially_destructible<_Type>::value>::
          __destroy(_first, _cur);
      throw;
    }
    return _cur;
  }
};
template <typename _ForwardIt, typename _T>
struct _UninitFillN<_ForwardIt, _T, true> {
  _UninitFillN() = delete;
  static inline _ForwardIt __fill(_ForwardIt _first, std::size_t _cnt,
                                  const _T& _value) {
    using _ForwardType = typename std::iterator_traits<_ForwardIt>::value_type;
    const bool _IS_BYTE = std::is_pointer<_ForwardIt>::value &&
        sizeof(_ForwardType) == 1 && std::is_arithmetic<_T>::value;
    return _FillN<_ForwardIt, _T, _IS_BYTE>::__fill(_first, _cnt, _value);
  }
};
} // namespace __implement

template <typename InputIt, typename ForwardIt>
inline ForwardIt uninitialized_copy(InputIt first, InputIt last,
                                    ForwardIt result) {
  using _InputType = typename std::iterator_traits<InputIt>::value_type;
  using _ForwardType = typename std::iterator_traits<ForwardIt>::value_type;
  static_assert(std::is_constructible<_ForwardType, const _InputType&>::value,
                "uninitialized_copy: is not constructible from input type");
  using _Category = typename std::iterator_traits<InputIt>::iterator_category;
  const bool _IS_TRIVIAL = std::is_assignable<
    typename std::iterator_traits<ForwardIt>::reference,
    typename std::iterator_traits<InputIt>::reference>::value &&
      std::is_trivially_copyable<_InputType>::value &&
      std::is_trivially_copyable<_ForwardType>::value;
  return __implement::_UninitCopy<InputIt, ForwardIt,
                                  _Category, false, _IS_TRIVIAL>::
      __copy(first, last, result);
}

template <typename InputIt, typename ForwardIt>
inline std::pair<InputIt, ForwardIt>
uninitialized_copy_n(InputIt first, std::size_t count,
                                      ForwardIt result) {
  using _InputType = typename std::iterator_traits<InputIt>::value_type;
  using _ForwardType = typename std::iterator_traits<ForwardIt>::value_type;
  const bool _IS_TRIVIAL = std::is_assignable<
    typename std::iterator_traits<ForwardIt>::reference,
    typename std::iterator_traits<InputIt>::reference>::value &&
      std::is_trivially_copyable<_InputType>::value &&
      std::is_trivially_copyable<_ForwardType>::value;
  return __implement::_UninitCopyN<InputIt, ForwardIt, false, _IS_TRIVIAL>::
      __copy(first, count, result);
}

template <typename InputIt, typename ForwardIt>
inline ForwardIt uninitialized_move(InputIt first, InputIt last,
                                    ForwardIt result) {
  using _InputType = typename std::iterator_traits<InputIt>::value_type;
  using _ForwardType = typename std::iterator_traits<ForwardIt>::value_type;
  using _Category = typename std::iterator_traits<InputIt>::iterator_category;
  const bool _IS_TRIVIAL = std::is_assignable<
    typename std::iterator_traits<ForwardIt>::reference,
    typename std::iterator_traits<InputIt>::reference>::value &&
      std::is_trivially_copyable<_InputType>::value &&
      std::is_trivially_copyable<_ForwardType>::value;
  return __implement::_UninitCopy<InputIt, ForwardIt,
                                  _Category, true, _IS_TRIVIAL>::
      __copy(first, last, result);
}

template <typename InputIt, typename ForwardIt>
inline std::pair<InputIt, ForwardIt>
uninitialized_move_n(InputIt first, std::size_t count, ForwardIt result) {
  using _InputType = typename std::iterator_traits<InputIt>::value_type;
  using _ForwardType = typename std::iterator_traits<ForwardIt>::value_type;
  const bool _IS_TRIVIAL = std::is_assignable<
    typename std::iterator_traits<ForwardIt>::reference,
    typename std::iterator_traits<InputIt>::reference>::value &&
      std::is_trivially_copyable<_InputType>::value &&
      std::is_trivially_copyable<_ForwardType>::value;
  return __implement::_UninitCopyN<InputIt, ForwardIt, true, _IS_TRIVIAL>::
      __copy(first, count, result);
}

template <typename ForwardIt, typename T>
void uninitialized_fill(ForwardIt first, ForwardIt last, const T& value) {
  using _ForwardType = typename std::iterator_traits<ForwardIt>::value_type;
  static_assert(std::is_constructible<_ForwardType, const T&>::value,
                "uninitialized_fill: is not constructible from fill type");
  using _Category = typename std::iterator_traits<ForwardIt>::iterator_category;
  const bool _IS_TRIVIAL = std::is_assignable<
    typename std::iterator_traits<ForwardIt>::reference,
    typename std::add_lvalue_reference<T>::type>::value &&
      std::is_trivially_copyable<_ForwardType>::value &&
      std::is_trivially_copyable<T>::value;
  __implement::_UninitFill<ForwardIt, T, _Category, _IS_TRIVIAL>::
      __fill(first, last, value);
}

template <typename ForwardIt, typename T>
ForwardIt uninitialized_fill_n(ForwardIt first,
                               std::size_t count, const T& value) {
  using _ForwardType = typename std::iterator_traits<ForwardIt>::value_type;
  static_assert(std::is_constructible<_ForwardType, const T&>::value,
                "uninitialized_fill_n: is not constructible from fill type");
  const bool _IS_TRIVIAL = std::is_assignable<
    typename std::iterator_traits<ForwardIt>::reference,
    typename std::add_lvalue_reference<T>::type>::value &&
      std::is_trivially_copyable<_ForwardType>::value &&
      std::is_trivially_copyable<T>::value;
  return __implement::_UninitFillN<ForwardIt, T, _IS_TRIVIAL>::
      __fill(first, count, value);
}

} // namespace stl
} // namespace ginshio

#endif // GINSHIO_STL__STL_INITIALIZED_HH_
