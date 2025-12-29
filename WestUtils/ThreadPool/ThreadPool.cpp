#include "../Include/ThreadPool.h"

ThreadPool::ThreadPool(size_t numThreads)
{
  _logger->log(Level::Info, "--- Creating worker threads for engine\n");

  for (size_t i = 0; i < numThreads; ++i)
  {
    _threads.emplace_back(
      [this]
      {
        while (true)
        {
          std::function<void()> task;
          {
            std::unique_lock<std::mutex> lock(_mutex);
            _cv.wait(lock, [this] { return !_tasks.empty() || _stop; });

            if (_stop && _tasks.empty())
            {
              return;
            }
            task = std::move(_tasks.front());
            _tasks.pop();
          }
          task();
        }
      });
  }
}

ThreadPool::~ThreadPool()
{
  {
    std::unique_lock<std::mutex> lock(_mutex);
    _stop = true;
  }

  _cv.notify_all();

  for (auto& thread : _threads)
  {
    thread.join();
  }
}

std::future<void> ThreadPool::enqueue(std::function<void()> task)
{
  auto promise = std::make_shared<std::promise<void>>();
  auto future  = promise->get_future();
  {
    std::unique_lock<std::mutex> lock(_mutex);
    _tasks.emplace(
      [task, promise]
      {
        task();
        promise->set_value();
      });
  }
  _cv.notify_one();
  return future;
}
