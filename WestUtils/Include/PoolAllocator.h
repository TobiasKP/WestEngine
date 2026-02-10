#pragma once

#include "Config.h"

#include <WestLogger.h>

struct Chunk
{
  Chunk* next;
};

// TODO: allocate/deallocate are not thread-safe, add mutex or use thread-local allocators
class PoolAllocator
{
public:
  PoolAllocator() {};
  ~PoolAllocator() {};

  void* allocate(size_t size);
  void deallocate(void* ptr, size_t size);

private:
  size_t _numberOfChunks    = Config::GeneralConfig.CHUNK_SIZE;
  Chunk* _allocationPointer = nullptr;
  WestLogger* _logger       = &WestLogger::getLoggerInstance();

  Chunk* allocateBlock(size_t size);
};
