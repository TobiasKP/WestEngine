#include "../../CoreHeaders/Utils/WestString.h"

#include <cstdarg>
#include <cstdio>

WestString::WestString() {}

void WestString::format(const char *format, ...) {
  va_list args;
  va_start(args, format);
  vsnprintf(_buffer, BUFFER_SIZE, format, args);
  va_end(args);
}

