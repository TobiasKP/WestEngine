#include "InterfaceBuilder.h"

InterfaceBuilder::InterfaceBuilder() { _factory = new ElementFactory(); }

InterfaceBuilder::~InterfaceBuilder() {
  if (_current != nullptr) {
    _logger.log(
        Level::Error,
        "@@@ Interface still in build process but Builder was destroyed.\n");
    delete _current;
  }
  _factory->~ElementFactory();
}

void InterfaceBuilder::createNewInterface(std::uint16_t xScreenPosition,
                                          std::uint16_t yScreenPosition,
                                          float scale, std::uint8_t rows,
                                          std::uint8_t columns,
                                          bool hiddenContainer) {

  _current = new ContainerElement();
  _current->scale = scale;
  _current->xLL = xScreenPosition;
  _current->yLL = yScreenPosition;
  _current->rowElements = rows;
  _current->columnElements = columns;
  _current->id = CURRENT_ID;
  if (hiddenContainer) {
    _current->flags = 0x08;
  }
  assert(_current != nullptr && _current->xLL >= 0 && _current->yLL >= 0);
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
