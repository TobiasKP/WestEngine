#pragma once

#include <mutex>
#include <vector>

template <typename T>
class tQueue
{
public:
  tQueue() {};
  ~tQueue() {};

  void push(T obj)
  {
    std::lock_guard lock(_m);
    _data.push_back(std::move(obj));
  }

  std::vector<T> drain()
  {
    std::lock_guard lock(_m);
    std::vector<T> result;
    result.swap(_data);
    return result;
  }

  size_t size()
  {
    std::lock_guard lock(_m);
    return _data.size();
  }

private:
  std::mutex _m;
  std::vector<T> _data;
};
