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
  ThreadPool(size_t numThreads = std::thread::hardware_concurrency()
                                 - 2);  // -1 for main running thread and -1 for available fireAndForget Thread
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
