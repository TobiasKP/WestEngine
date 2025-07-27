#include "../WestInterfaceFacade.h"

WestInterfaceFacade::WestInterfaceFacade() {
  count = 0;
  filled = false;
  for (std::uint8_t i = 0; i < 32; i++) {
    _interfaces.at(i) = nullptr;
  }
}

WestInterfaceFacade::~WestInterfaceFacade() {
  while (count > 0) {
    delete _interfaces.at(count);
    count--;
  }
}

ContainerElement *WestInterfaceFacade::findInterfaceById(std::uint8_t id) {
  for (std::uint8_t i = 0; i < count; i++) {
    if (_interfaces.at(i)->id == id) {
      return _interfaces.at(i);
    }
  }

  return nullptr;
}

std::uint8_t WestInterfaceFacade::createNewInterface(
    std::uint16_t xScreenPosition, std::uint16_t yScreenPosition, float scale,
    std::uint8_t gridLayout, std::vector<ElementProxy *> elements) {

  _builder.createNewInterface(xScreenPosition, yScreenPosition, scale,
                              gridLayout);
  for (auto *element : elements) {
    _builder.addElement(element);
  }
  ContainerElement *interface = _builder.build();
  _interfaces.at(count) = interface;
  count++;
  return interface->id;
}

bool WestInterfaceFacade::destroyInterface(std::uint8_t interfaceId) {
  std::int8_t location = -1;
  for (std::uint8_t i = 0; i < count; i++) {
    if (_interfaces.at(i)->id == interfaceId) {
      location = i;
      break;
    }
  }

  if (location == -1)
    return false;

  ContainerElement *e = _interfaces.at(count);
  _interfaces.at(count) = nullptr;
  _interfaces.at(location) = e;
  count--;
  return true;
}

void WestInterfaceFacade::addElement(std::uint8_t interfaceId,
                                     ElementProxy *element) {
  ContainerElement *interface = findInterfaceById(interfaceId);
  if (interface == nullptr)
    return;

  IElement *e = _builder.transform(element);
  interface->addChild(e);
};

bool WestInterfaceFacade::removeElement(std::uint8_t interfaceId,
                                        std::uint32_t elementId) {
  ContainerElement *interface = findInterfaceById(interfaceId);
  if (interface == nullptr)
    return false;

  return interface->deleteChildById(elementId);
};

void WestInterfaceFacade::updateRenderData() {};

const RenderData WestInterfaceFacade::getRenderData() { return _instance; };

///////////////////////////

bool WestInterfaceFacade::notify(std::uint8_t event, std::uint16_t mouseX,
                                 std::uint16_t mouseY) {
  return false;
};

bool WestInterfaceFacade::notify(std::uint8_t event, std::string value) {
  return false;
};

bool WestInterfaceFacade::resize(std::uint8_t interfaceId, std::uint16_t width,
                                 std::uint16_t height) {
  return false;
};

bool WestInterfaceFacade::reposition(std::uint8_t interfaceId,
                                     std::uint16_t xScreenPosition,
                                     std::uint16_t yScreenPosition) {
  return false;
};

const char *WestInterfaceFacade::getResource(std::string resource) {

  return NULL;
}
