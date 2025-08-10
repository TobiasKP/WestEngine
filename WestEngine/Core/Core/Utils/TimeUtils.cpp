#include "../../CoreHeaders/Utils/TimeUtils.h"

#include <chrono>

namespace TimeUtils {

double getCurrentTimeAsTime() {
  auto long_time = std::chrono::high_resolution_clock::now();
  auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
      long_time.time_since_epoch());
  return milliseconds.count();
}

double getDuration(double start, double end) { return (end - start); }

double getNanoseconds() {
  auto now = std::chrono::high_resolution_clock::now();
  auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(
      now.time_since_epoch());
  return nanoseconds.count();
}

std::string getCurrentTimeAsDate() {
  const std::chrono::time_point now{std::chrono::system_clock::now()};
  const std::chrono::year_month_day ymd{
      std::chrono::floor<std::chrono::days>(now)};
  return std::to_string(static_cast<unsigned>(ymd.day())) + "_" +
         std::to_string(static_cast<unsigned>(ymd.month())) + "_" +
         std::to_string(static_cast<int>(ymd.year()));
}

} // namespace TimeUtils
