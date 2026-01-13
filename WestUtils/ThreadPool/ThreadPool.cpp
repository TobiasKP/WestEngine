#include "../Include/ThreadPool.h"

ThreadPool::ThreadPool()
{
  _logger->log(Level::Info, "--- Creating worker threads for engine\n");

  std::size_t numThreads = getDefaultThreadCount();
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
  addFireAndForgetThread();
}

ThreadPool::~ThreadPool()
{
  {
    std::unique_lock<std::mutex> lock(_mutex);
    _stop = true;
  }

  _cv.notify_all();
  _forgetCv.notify_all();

  while (!_finishedDetachedWorker)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  for (auto& thread : _threads)
  {
    if (thread.joinable())
    {
      thread.join();
    }
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

void ThreadPool::fireAndForget(std::function<void()> task)
{
  std::unique_lock<std::mutex> lock(_smutex);
  _forgetTasks.emplace(task);
  _forgetCv.notify_one();
}

void ThreadPool::addFireAndForgetThread()
{
  _threads
    .emplace_back(
      [this]
      {
        while (true)
        {
          std::function<void()> task;
          {
            std::unique_lock<std::mutex> lock(_smutex);
            _forgetCv.wait(lock, [this] { return !_forgetTasks.empty() || _stop; });

            if (_stop && _forgetTasks.empty())
            {
              _finishedDetachedWorker = true;
              return;
            }
            task = std::move(_forgetTasks.front());
            _forgetTasks.pop();
          }
          task();
        }
      })
    .detach();
}
