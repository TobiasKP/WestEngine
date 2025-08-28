#include "InterfaceBuilder.h"

InterfaceBuilder::InterfaceBuilder() {}

InterfaceBuilder::~InterfaceBuilder() {
  if (_current != nullptr) {
    // TODO log info
    delete _current;
  }
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
  // Todo log info
}

void InterfaceBuilder::addElement(ElementProxy *e) {
  IElement *newElement = transform(e);
  _current->addChild(newElement, e->column, e->row);
}

ContainerElement *InterfaceBuilder::build() {
  assert(_current != nullptr);
  ContainerElement *result = _current;
  _current = nullptr;
  return result;
}

IElement *InterfaceBuilder::transform(ElementProxy *e) {
  assert(e != nullptr);
  return nullptr;
}
