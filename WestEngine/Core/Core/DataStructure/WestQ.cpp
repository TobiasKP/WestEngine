#include "../../CoreHeaders/DataStructure/WestQ.h"

#include "../../CoreHeaders/Utils/WestString.h"

WestQ::WestQ() {
  _logger = nullptr;
  _string = nullptr;
  _size = 0;
  _capacity = 0;
  _array = new IManager *[_capacity];
  _rear = -1;
  _front = -1;
}

WestQ::WestQ(std::uint8_t maxCapacity, WestLogger *logger) {
  assert(logger && maxCapacity > 0);
  _logger = logger;
  _string = new WestString();
  _capacity = maxCapacity;
  _front = _size = 0;
  _array = new IManager *[_capacity];
  _rear = _capacity - 1;
#ifdef DEBUG
  _string->format("Initialized Manager Queue with capacity: %d\n",_capacity);
  _logger->writeInfo(_string->getBuffer());
#endif
}

WestQ::~WestQ() {
  for (int i = 0; i < _capacity; i++) {
    assert(_array[i] != nullptr);
    delete _array[i];
  }
  delete _array;
}

void WestQ::enqueue(IManager *item) {
  assert(item);
  if (isFull()) {
    _string->format("West Queue is full! stopped trying to add: %s", item->getName());
    _logger->writeError(_string->getBuffer());
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
