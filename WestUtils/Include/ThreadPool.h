// Thread pool implementation based on the standard C++ producer-consumer pattern
// using std::condition_variable and std::mutex for task scheduling.
// Reference: https://en.cppreference.com/w/cpp/thread/condition_variable
// Extended with a fire-and-forget queue and hardware-aware thread count sizing.

#pragma once

#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <WestLogger.h>

class ThreadPool
{
public:
  static std::size_t getDefaultThreadCount()
  {
    static const std::size_t hwThreads = std::thread::hardware_concurrency();
    return hwThreads > 3 ? hwThreads - 2 : 1;
  }

  ThreadPool();
  ~ThreadPool();

  void fireAndForget(std::function<void()> task);
  std::future<void> enqueue(std::function<void()> task);
  std::uint8_t getPoolSize()
  {
    return _threads.size();
  }

private:
  void addFireAndForgetThread();

  WestLogger* _logger = &WestLogger::getLoggerInstance();
  std::vector<std::thread> _threads;
  std::queue<std::function<void()>> _tasks;
  std::queue<std::function<void()>> _forgetTasks;
  std::mutex _mutex, _smutex;
  std::condition_variable _cv, _forgetCv;
  bool _stop = false;
  std::atomic_bool _finishedDetachedWorker{false};
};
