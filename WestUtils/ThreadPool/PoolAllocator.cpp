#include "../Include/PoolAllocator.h"

#include <stdlib.h>

Chunk* PoolAllocator::allocateBlock(size_t chunkSize)
{
#ifdef DEBUG
  _logger->log(Level::Info, "---Allocating new chunks\n");
#endif
  size_t blockSize  = _numberOfChunks * chunkSize;
  Chunk* blockBegin = reinterpret_cast<Chunk*>(malloc(blockSize));
  Chunk* chunk      = blockBegin;

  for (int i = 0; i < _numberOfChunks - 1; ++i)
  {
    chunk->next = reinterpret_cast<Chunk*>(reinterpret_cast<char*>(chunk) + chunkSize);
    chunk       = chunk->next;
  }

  chunk->next = nullptr;
  return blockBegin;
}

void* PoolAllocator::allocate(size_t size)
{
  std::lock_guard<std::mutex> lock(_mutex);

  if (_allocationPointer == nullptr)
  {
    _allocationPointer = allocateBlock(size);
  }

  Chunk* freeChunk   = _allocationPointer;
  _allocationPointer = _allocationPointer->next;

#ifdef DEBUG
  _logger->log(Level::Info, "---Returning next free Chunk\n");
#endif
  return freeChunk;
}

void PoolAllocator::deallocate(void* chunk, size_t size)
{
  std::lock_guard<std::mutex> lock(_mutex);
  reinterpret_cast<Chunk*>(chunk)->next = _allocationPointer;
  _allocationPointer                    = reinterpret_cast<Chunk*>(chunk);
}
