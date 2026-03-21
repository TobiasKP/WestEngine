#pragma once

#include <mutex>
#include <optional>
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

  std::optional<T> tryPop()
  {
    std::lock_guard lock(_m);
    if (_data.size() == 0)
    {
      return {};
    }
    T result = _data.back();
    _data.pop_back();
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
