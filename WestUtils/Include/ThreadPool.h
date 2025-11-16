#pragma once

#if defined(_WIN32) || defined(_WIN64)
#if defined(WESTUTILS_BUILDING_DLL)
#define WESTUTILS __declspec(dllexport)
#else
#define WESTUTILS __declspec(dllimport)
#endif
#else
#define WESTUTILS __attribute__((visibility("default")))
#endif

#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <WestLogger.h>

class WESTUTILS ThreadPool
{
public:
  ThreadPool(size_t numThreads = std::thread::hardware_concurrency());
  ~ThreadPool();

  std::future<void> enqueue(std::function<void()> task);
  std::uint8_t getPoolSize()
  {
    return _threads.size();
  }

private:
  WestLogger* _logger = &WestLogger::getLoggerInstance();
  std::vector<std::thread> _threads;
  std::queue<std::function<void()>> _tasks;
  std::mutex _mutex;
  std::condition_variable _cv;

  bool _stop = false;
};
