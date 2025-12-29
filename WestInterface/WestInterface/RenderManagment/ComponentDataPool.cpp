#include "ComponentDataPool.h"

#include <cassert>
#include <format>
#include <mutex>

ComponentDataPool::ComponentDataPool()
{
  _poolingData.reserve(2048);
  _poolingData.assign(2048, nullptr);
  _slotUsed.reserve(2048);
  _slotUsed.assign(2048, false);
  _positionUpdateCallback = nullptr;
  _dirty                  = false;
}

ComponentDataPool::~ComponentDataPool()
{
  std::lock_guard<std::mutex> lock(_poolMutex);
  for (auto* data : _poolingData)
  {
    delete data;
  }
  _poolingData.clear();
  _slotUsed.clear();
#ifdef DEBUG
  _logger.log(Level::Info, "@@@ ComponentDataPool destroyed, cleaned up Domponent Data\n");
#endif
}

std::uint32_t ComponentDataPool::reserveNew(std::uint32_t size)
{
  std::lock_guard<std::mutex> lock(_poolMutex);

  if (size == 0)
  {
    _logger.log(Level::Error, "@@@ ComponentDataPool::reserveNew called with size 0\n");
    return _poolingData.size();
  }

  std::uint32_t result = findFit(size);
  if (result != -1)
  {
    _dirty = true;
    for (std::size_t i = result; i < result + size; i++)
    {
      assert(_poolingData[i] == nullptr && _slotUsed[i] == false);
      _poolingData[i] = new ComponentData();
      _slotUsed[i]    = true;
    }
  }
  else
  {
    _logger.log(Level::Error,
                std::format("@@@ ComponentDataPool::reserveNew "
                            "could not find space for size {}\n",
                            size));
  }

  return result;
}

bool ComponentDataPool::deleteRange(std::uint32_t start, std::uint32_t end)
{
  std::lock_guard<std::mutex> lock(_poolMutex);
  if (start >= _poolingData.size() || end >= _poolingData.size() || start > end)
  {
    _logger.log(Level::Error,
                std::format("@@@ ComponentDataPool::deleteRange invalid range [{}, "
                            "{}], pool size: {}\n",
                            start,
                            end,
                            _poolingData.size()));
    return false;
  }

  for (std::size_t i = start; i <= end; i++)
  {
    if (_poolingData[i] != nullptr)
    {
      delete _poolingData[i];
      _poolingData[i] = nullptr;
    }
    _slotUsed[i] = false;
  }

  return true;
}

ComponentData* ComponentDataPool::getDataAtLocation(std::uint32_t location)
{
  std::lock_guard<std::mutex> lock(_poolMutex);

  if (location > _poolingData.size())
  {
    _logger.log(Level::Error,
                std::format("@@@ ComponentDataPool::getDataAtLocation location {} "
                            "out of bounds, pool size: {}\n",
                            location,
                            _poolingData.size()));
    return nullptr;
  }

  if (_poolingData[location] == nullptr)
  {
    _logger.log(Level::Error,
                std::format("ComponentDataPool::getDataAtLocation found "
                            "nullptr at location {}\n",
                            location));
    return nullptr;
  }
  assert(_poolingData[location] != nullptr);
  return _poolingData[location];
}

std::uint32_t ComponentDataPool::findFit(std::uint32_t size)
{
  const std::size_t N = _slotUsed.size();
  std::size_t run     = 0;
  for (std::size_t i = 0; i < N; i++)
  {
    run = _slotUsed[i] ? 0 : (run + 1);
    if (run == size)
    {
      return (i + 1) - run;
    }
  }

  if (run < size && _dirty)
  {
    defragPool();
    return findFit(size);
  }
  else
  {
    return -1;
  }
}

void ComponentDataPool::defragPool()
{
  std::uint32_t writePos = 0;

  std::uint32_t lastUsedSlot = 0;
  for (std::uint32_t i = 0; i < _slotUsed.size(); ++i)
  {
    if (_slotUsed[i])
    {
      lastUsedSlot = i;
    }
  }

  for (std::uint32_t readPos = 0; readPos <= lastUsedSlot; ++readPos)
  {
    if (_slotUsed[readPos])
    {
      if (writePos != readPos)
      {
        _poolingData[writePos] = _poolingData[readPos];
        _slotUsed[writePos]    = true;
        _poolingData[readPos]  = nullptr;
        _slotUsed[readPos]     = false;
        assert(_positionUpdateCallback != nullptr);
        _positionUpdateCallback(readPos, writePos);

#ifdef DEBUG
        _logger.log(Level::Info, std::format("@@@ Defrag: moved element from position {} to {}\n", readPos, writePos));
#endif
      }
      writePos++;
    }
  }

#ifdef DEBUG
  _logger.log(Level::Info,
              std::format("@@@ Defrag complete: compacted pool from {} to {} elements\n", lastUsedSlot + 1, writePos));
#endif

  _dirty = false;
}
