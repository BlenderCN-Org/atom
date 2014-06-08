#pragma once

namespace atom {

namespace noncopyable {

class NonCopyable {
protected:
  NonCopyable() {}
  ~NonCopyable() {}

private:
#ifndef _MSC_VER
  NonCopyable(const NonCopyable &) = delete;
  NonCopyable& operator=(const NonCopyable &) = delete;
#else
  NonCopyable(const NonCopyable &);
  NonCopyable& operator=(const NonCopyable &);
#endif
};

}

typedef noncopyable::NonCopyable NonCopyable;

}
