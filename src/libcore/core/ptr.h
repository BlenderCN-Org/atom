#pragma once

#include <memory>

namespace atom {

#ifdef __linux__

template<typename T, typename D = std::default_delete<T>>
using uptr = std::unique_ptr<T, D>;

template<typename T>
using sptr = std::shared_ptr<T>;

#else
#  define uptr std::unique_ptr
#  define sptr std::shared_ptr
#endif

}
