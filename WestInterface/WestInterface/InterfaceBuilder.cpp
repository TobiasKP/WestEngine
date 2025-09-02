#include "InterfaceBuilder.h"

#include <format>

InterfaceBuilder::InterfaceBuilder() { _factory = new ElementFactory(); }

InterfaceBuilder::~InterfaceBuilder() {
  if (_current != nullptr) {
    _logger.log(
        Level::Error,
        "@@@ Interface still in build process but Builder was destroyed.");
    delete _current;
  }
  _factory->~ElementFactory();
}

void InterfaceBuilder::createNewInterface(std::uint16_t xScreenPosition,
                                          std::uint16_t yScreenPosition,
                                          float scale, std::uint8_t gridCells) {

  _current = new ContainerElement();
  _current->scale = scale;
  _current->xLL = xScreenPosition;
  _current->yLL = yScreenPosition;
  _current->gridCells = gridCells;
  assert(_current != nullptr && _current->xLL >= 0 && _current->yLL >= 0);
#ifdef DEBUG
  _logger.log(Level::Info, std::format("@@@ Creating new Interface -> {} : {}",
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
              "@@@ Interface build complete returning Parent container");
#endif
  return result;
}

IElement *InterfaceBuilder::transform(ElementProxy *e) {
  assert(e != nullptr);
  IElement *result = _factory->createElement(e);
  assert(result != nullptr);
  return result;
}
