#pragma once

#if defined(__GNUC__) || defined(__clang__)
#  define MM_FUNC_NAME __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
#  define MM_FUNC_NAME __FUNCSIG__
#else
#error Unsupported compiler
#endif

namespace atom {

}
