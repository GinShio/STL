#ifndef GINSHIO_STL__BASE_STL_CHECK_HH_
#define GINSHIO_STL__BASE_STL_CHECK_HH_ 1

#include <cstdio>
#include <iterator>
#include <memory>
#include <stdexcept>

namespace ginshio {
namespace stl {

template <typename _Iterator,
          typename = typename std::enable_if<
            std::is_base_of<std::input_iterator_tag,
                            typename std::iterator_traits<
                              _Iterator>::iterator_category>::value>::type*>
inline void __check_out_of_range(const char* _func,
                                 _Iterator _pos, _Iterator _end) {
  if (_pos == _end) {
    const char* _fmt = "function \"%s\": pos out of container range";
    int _len = std::snprintf(nullptr, 0, _fmt, _func) + 1;
    std::unique_ptr<char[]> _buf(new char[_len]);
    snprintf(_buf.get(), _len, _fmt, _func);
    throw std::out_of_range(_buf.get());
  }
}

inline void __check_out_of_range(const char* _func, const std::size_t& _pos,
                                 const std::size_t& _sz) {
  if (_sz <= _pos) {
    const char* _fmt = "function \"%s\": pos(%zu) out of container range(%zu)";
    int _len = std::snprintf(nullptr, 0, _fmt, _func, _pos, _sz) + 1;
    std::unique_ptr<char[]> _buf(new char[_len]);
    snprintf(_buf.get(), _len, _fmt, _func, _pos, _sz);
    throw std::out_of_range(_buf.get());
  }
}

inline void __check_length_error(const char* _func, const std::size_t& _need,
                                 const std::size_t& _max_sz) {
  if (_max_sz < _need) {
    const char* _fmt =
        "function \"%s\": need cap(%zu) greater than or equal max size(%zu)";
    int _len = std::snprintf(nullptr, 0, _fmt, _func, _need, _max_sz) + 1;
    std::unique_ptr<char[]> _buf(new char[_len]);
    snprintf(_buf.get(), _len, _fmt, _func, _need, _max_sz);
    throw std::length_error(_buf.get());
  }
}

}  // namespace stl
}  // namespace ginshio

#endif  // GINSHIO_STL__BASE_STL_CHECK_HH_
