#pragma once

#include <WestLogger.h>
#include "../../../Constants/CoreConstants.h"

struct Chunk {
  Chunk *next;
};

class PoolAllocator {
public:
  PoolAllocator() {};
  ~PoolAllocator() {};

  void *allocate(size_t size);
  void deallocate(void *ptr, size_t size);

private:
  size_t _numberOfChunks = CoreConstants::CHUNK_SIZE;
  Chunk *_allocationPointer = nullptr;
  WestLogger *_logger = &WestLogger::getLoggerInstance();

  Chunk *allocateBlock(size_t size);
};
