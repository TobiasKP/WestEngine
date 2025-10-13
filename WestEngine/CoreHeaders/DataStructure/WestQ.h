#pragma once

#include <cstdint>
#include <cassert>
#include <WestLogger.h>

#include "../Interfaces/IManager.h"


class WestQ {

public:
  WestQ();
  WestQ(std::uint8_t maxCapacity, WestLogger *logger);
  ~WestQ();

  void enqueue( IManager* item);
  IManager* dequeue();
  IManager* front();
  IManager* rear();
  bool isEmpty();
  bool isFull();
  inline std::int32_t getSize() { return _size; }
  inline std::int32_t getFront() { return _front; }
  inline std::int32_t getRear() { return _rear; }
  inline std::uint8_t getCapacity() { return _capacity; }

private:
  WestLogger *_logger;
  std::int32_t _front, _rear,  _size;
  std::uint8_t _capacity;
  IManager **_array;
};
