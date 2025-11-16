#pragma once

#if defined(_WIN32) || defined(_WIN64)
#if defined(WESTUTILS_BUILDING_DLL)
#define WESTUTILS __declspec(dllexport)
#else
#define WESTUTILS __declspec(dllimport)
#endif
#else
#define WESTUTILS __attribute__((visibility("default")))
#endif

#include "Config.h"

#include <WestLogger.h>

struct Chunk
{
  Chunk* next;
};

class WESTUTILS PoolAllocator
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
