#pragma once

#include <cstring>

struct CStrCmp {
  bool operator()(const char *lhs, const char *rhs) const {
    return std::strcmp(lhs, rhs) < 0;
  }
};

class WestString {
public:
  WestString();
  void format(const char *format, ...);
  inline const char *getBuffer() { return _buffer; }

private:
  static constexpr size_t BUFFER_SIZE = 2048;
  char _buffer[BUFFER_SIZE];
};
