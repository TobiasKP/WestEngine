#pragma once

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <fstream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

constexpr std::string_view INFO_FILE_NAME  = "WestLog_";
constexpr std::string_view ERROR_FILE_NAME = "WestError_";
constexpr std::string_view CYCLE_FILE_NAME = "WestCyclingLog_";
constexpr std::uint8_t CYCLE_LENGTH   = 200;
constexpr std::uint8_t BULK_SIZE      = 50;

enum class Level : uint8_t { Info, Error, Cycle };

struct Message
{
  Level mode;
  std::string payload;
};

class WestLogger
{
public:
  static WestLogger& getLoggerInstance();

  WestLogger(WestLogger const&)     = delete;
  void operator=(WestLogger const&) = delete;

  void log(const Level level, const std::string message);
  void closeFileStreams();

  std::uint8_t getCycleLength();

private:
  WestLogger();
  ~WestLogger();

  void workerThread();
  void joinThreads();

  std::ofstream _logFile;
  std::ofstream _errorFile;
  std::ofstream _cycleFile;

  std::atomic<size_t> _cycleLength{CYCLE_LENGTH};
  std::atomic<bool> _stopWorker{false};
  std::queue<Message> _q;
  std::mutex _mutex;
  std::condition_variable _cv;
  std::thread _th;
};
