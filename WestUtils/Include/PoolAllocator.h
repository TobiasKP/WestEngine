#pragma once

#include "Config.h"

#include <WestLogger.h>

#include <mutex>

struct Chunk
{
  Chunk* next;
};

class PoolAllocator
{
public:
  PoolAllocator() {};
  ~PoolAllocator() {};

  void* allocate(size_t size);
  void deallocate(void* ptr, size_t size);

private:
  std::mutex _mutex;
  size_t _numberOfChunks    = Config::GeneralConfig.CHUNK_SIZE;
  Chunk* _allocationPointer = nullptr;
  WestLogger* _logger       = &WestLogger::getLoggerInstance();

  Chunk* allocateBlock(size_t size);
};
