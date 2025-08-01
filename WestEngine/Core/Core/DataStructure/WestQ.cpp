#include "../../CoreHeaders/DataStructure/WestQ.h"

#include <format>

WestQ::WestQ() {
  _logger = nullptr;
  _size = 0;
  _capacity = 0;
  _array = new IManager *[_capacity];
  _rear = -1;
  _front = -1;
}

WestQ::WestQ(std::uint8_t maxCapacity, WestLogger *logger) {
  assert(logger && maxCapacity > 0);
  _logger = logger;
  _capacity = maxCapacity;
  _front = _size = 0;
  _array = new IManager *[_capacity];
  _rear = _capacity - 1;
#ifdef DEBUG
  _logger->writeInfo(
      std::format("Initialized Manager Queue with capacity: {}\n", _capacity));
#endif
}

WestQ::~WestQ() {
#ifdef DEBUG 
    _logger->writeInfo(std::format("Deleting Manager Queue."));
#endif
  delete _array;
}

void WestQ::enqueue(IManager *item) {
  assert(item);
  if (isFull()) {
    _logger->writeError(std::format(
        "West Queue is full! stopped trying to add: {}", item->getName()));
    return;
  }

  _rear = (_rear + 1) % _capacity;
  _array[_rear] = item;
  _size++;
}

IManager *WestQ::dequeue() {
  if (isEmpty()) {
    _logger->writeError("West Queue is empty!");
    return nullptr;
  }

  IManager *item = _array[_front];
  _front = (_front + 1) % _capacity;
  _size--;

  assert(item);
  return item;
}

IManager *WestQ::front() {
  if (isEmpty()) {
    return nullptr;
  }
  assert(_array[_front]);
  return _array[_front];
}

IManager *WestQ::rear() {
  if (isEmpty()) {
    return nullptr;
  }
  assert(_array[_rear]);
  return _array[_rear];
}

bool WestQ::isEmpty() { return _size == 0; }

bool WestQ::isFull() { return _size == _capacity; }
