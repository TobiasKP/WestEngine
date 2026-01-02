#pragma once

#include <chrono>
#include <string>

namespace TimeUtils
{
static double getCurrentTimeAsTime()
{
  auto long_time    = std::chrono::high_resolution_clock::now();
  auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(long_time.time_since_epoch());
  return milliseconds.count();
}

static double getCurrentTimeAsHz()
{
  // TODO fill when used
  return 0;
}

static double getDuration(double start, double end)
{
  return (end - start);
}

static double getNanoseconds()
{
  auto now         = std::chrono::high_resolution_clock::now();
  auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch());
  return nanoseconds.count();
}

static std::string getCurrentTimeAsDate()
{
  const std::chrono::time_point now{std::chrono::system_clock::now()};
  const std::chrono::year_month_day ymd{std::chrono::floor<std::chrono::days>(now)};
  return std::to_string(static_cast<unsigned>(ymd.day())) + "_" + std::to_string(static_cast<unsigned>(ymd.month()))
         + "_" + std::to_string(static_cast<int>(ymd.year()));
}

};  // namespace TimeUtils
