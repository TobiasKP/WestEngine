#pragma once

#include <mutex>
#include <queue>
#include <thread>
#include <functional>
#include <future>

#include <WestLogger.h>

class ThreadPool {

public:
  ThreadPool(size_t numThreads = std::thread::hardware_concurrency());
  ~ThreadPool();
  
  std::future<void> enqueue(std::function<void()> task);
  void setLogger(WestLogger *logger) { _logger = logger; }

private:
  WestLogger *_logger = &WestLogger::getLoggerInstance();
  std::vector<std::thread> _threads;
  std::queue<std::function<void()> > _tasks;
  std::mutex _mutex;
  std::condition_variable _cv;

  bool _stop = false;
};
