#include "../../CoreHeaders/Utils/TimeUtils.h"

#include <chrono>

namespace TimeUtils {

double getCurrentTimeAsTime() {
  auto long_time = std::chrono::high_resolution_clock::now();
  auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
      long_time.time_since_epoch());
  return milliseconds.count();
}

double getDuration(double start, double end) {
  return (end - start);
}

double getNanoseconds() {
  auto now = std::chrono::high_resolution_clock::now();
  auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(
      now.time_since_epoch());
  return nanoseconds.count();
}

} // namespace TimeUtils
