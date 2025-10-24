#include "InterfaceBuilder.h"

InterfaceBuilder::InterfaceBuilder() {
  _factory = nullptr;
  _dataPool = nullptr;
}

InterfaceBuilder::InterfaceBuilder(EventObserver *eo, ValueObserver *vo) {
  _vObserver = vo;
  _eObserver = eo;
  _factory = new ElementFactory(eo, vo);
  _dataPool = new ComponentDataPool();
}

InterfaceBuilder::~InterfaceBuilder() {
  if (_current != nullptr) {
    _logger.log(
        Level::Error,
        "@@@ Interface still in build process but Builder was destroyed.\n");
    delete _current;
  }
  delete _factory;
  delete _dataPool;
}

void InterfaceBuilder::createNewInterface(std::uint16_t xScreenPosition,
                                          std::uint16_t yScreenPosition,
                                          float stretchX, float stretchY,
                                          std::uint16_t rows,
                                          std::uint16_t columns,
                                          bool hiddenContainer) {

  _current = new ContainerElement();
  assert(_current != nullptr);
  _current->poolPosition = _dataPool->reserveNew(rows * columns);
  _dataPool->setPositionUpdateCallback(
      [this](std::uint32_t oldPos, std::uint32_t newPos) {
        _current->updatePositions(oldPos, newPos);
      });
  _current->stretchX = stretchX;
  _current->stretchY = stretchY;
  _current->xLL = xScreenPosition;
  _current->yLL = yScreenPosition;
  _current->rowElements = rows;
  _current->columnElements = columns;
  _current->id = CURRENT_ID;
  _current->dataPool = _dataPool;
  assert(_current->xLL >= 0 && _current->yLL >= 0);
  CURRENT_ID++;

#ifdef DEBUG
  _logger.log(Level::Info,
              std::format("@@@ Creating new Interface -> {} : {}\n",
                          xScreenPosition, yScreenPosition));
#endif
}

void InterfaceBuilder::addElement(ElementProxy *e) {
  assert(e != nullptr);
  IElement *newElement = transform(e);
  assert(newElement != nullptr);

  newElement->poolPosition = _dataPool->reserveNew(newElement->rowElements *
                                                   newElement->columnElements);
  if (newElement->poolPosition == -1) {
    _logger.log(
        Level::Error,
        "@@@ Error reserving size for new Element do not add Element.\n");
    return;
  }

  _current->addChild(newElement, e->column, e->row);
}

ContainerElement *InterfaceBuilder::build() {
  assert(_current != nullptr);
  ContainerElement *result = _current;
  _current = nullptr;
#ifdef DEBUG
  _logger.log(Level::Info,
              "@@@ Interface build complete returning Parent container\n");
#endif
  return result;
}

IElement *InterfaceBuilder::transform(ElementProxy *e) {
  assert(e != nullptr);
  IElement *result = _factory->createElement(e);
  assert(result != nullptr);
  return result;
}
