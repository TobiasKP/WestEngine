#include "../WestInterfaceFacade.h"

#include <format>

WestInterfaceFacade &WestInterfaceFacade::getInterfaceInstance() {
  static WestInterfaceFacade instance;
  return instance;
}

WestInterfaceFacade::WestInterfaceFacade() {
  count = 0;
  _renderManager = new UIRenderManager();
  _builder = new InterfaceBuilder();
  for (std::uint8_t i = 0; i < 32; i++) {
    _interfaces.at(i) = nullptr;
  }
}

WestInterfaceFacade::~WestInterfaceFacade() {}

void WestInterfaceFacade::shutdown() {
  while (count > 0) {
    delete _interfaces.at(count);
    count--;
  }
  delete _builder;
}

ContainerElement *WestInterfaceFacade::findInterfaceById(std::uint8_t id) {
  if (id <= 0)
    return nullptr;

  for (std::uint8_t i = 0; i < count; i++) {
    assert(_interfaces.at(i) != nullptr);
    if (_interfaces.at(i)->id == id)
      return _interfaces.at(i);
  }

  return nullptr;
}

std::uint8_t WestInterfaceFacade::createNewInterface(
    std::uint16_t xScreenPosition, std::uint16_t yScreenPosition, float scale,
    std::uint8_t rows, std::uint8_t columns, bool hiddenContainer,
    std::vector<ElementProxy *> elements) {
#ifdef DEBUG
  _logger.log(Level::Info, "@@@ Creating new Interface\n");
#endif

  _builder->createNewInterface(xScreenPosition, yScreenPosition, scale, rows,
                               columns, hiddenContainer);
  for (auto *element : elements) {
#ifdef DEBUG
    _logger.log(Level::Cycle,
                std::format("@@@ Addding new element to interface: {}\n",
                            (int)element->type));
#endif
    _builder->addElement(element);
  }
  ContainerElement *interface = _builder->build();
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

  _logger.log(Level::Info,
              std::format("@@@ Destroying interface: {}\n", interfaceId));
  ContainerElement *elementToDelete = _interfaces.at(location);
 
  if (location != count - 1) {
    _interfaces.at(location) = _interfaces.at(count - 1);
  }
  _interfaces.at(count - 1) = nullptr;
  count--;

  delete elementToDelete;
  return true;
}

void WestInterfaceFacade::addElement(std::uint8_t interfaceId,
                                     ElementProxy *element) {
  ContainerElement *interface = findInterfaceById(interfaceId);
  if (interface == nullptr)
    return;

  assert(element != nullptr);
  IElement *e = _builder->transform(element);
  assert(e != nullptr);
#ifdef DEBUG
  _logger.log(
      Level::Info,
      std::format(
          "@@@ Adding additional element to interface: {}, after creation\n",
          interfaceId));
#endif
  interface->addChild(e, element->column, element->row);
};

bool WestInterfaceFacade::removeElement(std::uint8_t interfaceId,
                                        std::uint32_t elementId) {
  ContainerElement *interface = findInterfaceById(interfaceId);
  if (interface == nullptr)
    return false;

  return interface->deleteChildById(elementId);
};

void WestInterfaceFacade::updateRenderData() {
#ifdef DEBUG
  _logger.log(Level::Cycle, "@@@ Updating render Data for interfaces\n");
#endif
  _renderManager->updateRenderData(_interfaces, count);
};

std::vector<ComponentData *> WestInterfaceFacade::getRenderData() {
  return _renderManager->getRenderData();
};

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
