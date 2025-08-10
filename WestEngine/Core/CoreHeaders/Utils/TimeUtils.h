#pragma once

#include <string>

namespace TimeUtils {
double getCurrentTimeAsTime();
double getCurrentTimeAsHz();
double getDuration(double start, double end);
double getNanoseconds();
std::string getCurrentTimeAsDate();
}; // namespace TimeUtils
