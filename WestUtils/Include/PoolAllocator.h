// Pool allocator using a free-list of fixed-size chunks.
// Based on the standard pool allocation pattern described in:
// - Dmitry Soshnikov, "Writing a Pool Allocator" (https://dmitrysoshnikov.com/compilers/writing-a-pool-allocator/)
// - GameDev.net, "C++ Custom Memory Allocation" (https://www.gamedev.net/articles/programming/general-and-gameplay-programming/c-custom-memory-allocation-r3010/)
// Adapted with thread-safe allocation via std::mutex and configurable chunk count.

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
