#include "../WestInterfaceFacade.h"

#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <format>

#include "RenderManagment/TextRenderManager.h"

WestInterfaceFacade &WestInterfaceFacade::getInterfaceInstance() {
  static WestInterfaceFacade instance;
  return instance;
}

WestInterfaceFacade::WestInterfaceFacade() {
  count = 0;
  _valueObserver = new ValueObserver();
  _eventObserver = new EventObserver();
  assert(_valueObserver != nullptr && _eventObserver != nullptr);
  _renderManager = new UIRenderManager();
  _textManager = new TextRenderManager();
  _builder = new InterfaceBuilder(_eventObserver, _valueObserver);
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
  delete _textManager;
}

void WestInterfaceFacade::init() {
  glGenVertexArrays(1, &_interfaceVAO);
  glGenBuffers(1, &_interfaceVBO);
  glGenBuffers(1, &_interfaceEBO);
  glGenBuffers(1, &_interfaceCOL);
  glGenBuffers(1, &_interfaceOFFSET);
  glGenBuffers(1, &_interfaceFLAGS);
  glGenBuffers(1, &_interfaceTEX);
  glGenBuffers(1, &_interfaceUV);
  glGenTextures(1, &_interfaceFONT_TEXTURE_ID);

  glBindVertexArray(_interfaceVAO);

  glBindBuffer(GL_ARRAY_BUFFER, _interfaceVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(baseQuad), baseQuad, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _interfaceEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, _interfaceCOL);
  glBufferData(GL_ARRAY_BUFFER, NULL, NULL, GL_STATIC_DRAW);

  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glVertexAttribDivisor(1, 1);
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, _interfaceOFFSET);
  glBufferData(GL_ARRAY_BUFFER, NULL, NULL, GL_STATIC_DRAW);

  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glVertexAttribDivisor(2, 1);
  glEnableVertexAttribArray(2);

  glBindBuffer(GL_ARRAY_BUFFER, _interfaceTEX);
  glBufferData(GL_ARRAY_BUFFER, sizeof(baseTex), baseTex, GL_STATIC_DRAW);

  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(3);

  glBindBuffer(GL_ARRAY_BUFFER, _interfaceUV);
  glBufferData(GL_ARRAY_BUFFER, NULL, NULL, GL_STATIC_DRAW);

  glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glVertexAttribDivisor(4, 1);
  glEnableVertexAttribArray(4);

  glBindBuffer(GL_ARRAY_BUFFER, _interfaceFLAGS);
  glBufferData(GL_ARRAY_BUFFER, NULL, NULL, GL_STATIC_DRAW);

  glVertexAttribIPointer(5, 1, GL_UNSIGNED_INT, sizeof(std::uint32_t),
                         (void *)0);
  glVertexAttribDivisor(5, 1);
  glEnableVertexAttribArray(5);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  std::int32_t width, height, numComponents;
  const char *textureFile = "/assets/Textures/font.bmp";
  char cwd[128];
  char filePath[PATH_MAX];
  if (getcwd(cwd, sizeof(cwd)) == NULL) {
    _logger.log(Level::Error, "---Error getting current working directory!\n");
  }

  snprintf(filePath, sizeof(filePath), "%s%s%s", cwd, "/", textureFile);

  unsigned char *imgData =
      stbi_load(filePath, &width, &height, &numComponents, 0);
  if (imgData == NULL) {
    _logger.log(
        Level::Error,
        std::format("---No Imagedata loaded for texture: {} - STBI Error: {}\n",
                    filePath, stbi_failure_reason()));
  }

  glBindTexture(GL_TEXTURE_2D, _interfaceFONT_TEXTURE_ID);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
               GL_UNSIGNED_BYTE, imgData);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(imgData);

  _textManager->initializeFontAtlas();
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
    std::uint16_t xScreenPosition, std::uint16_t yScreenPosition,
    float stretchX, float stretchY, std::uint16_t rows, std::uint16_t columns,
    bool hiddenContainer, std::vector<ElementProxy *> elements) {
#ifdef DEBUG
  _logger.log(Level::Info, "@@@ Creating new Interface\n");
#endif

  _builder->createNewInterface(xScreenPosition, yScreenPosition, stretchX,
                               stretchY, rows, columns, hiddenContainer);
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

bool WestInterfaceFacade::notify(std::int16_t elementId, std::uint8_t event,
                                 std::uint16_t mouseX, std::uint16_t mouseY) {
  bool evResult =
      _eventObserver->handleEvent(elementId, event, mouseX, mouseY, "");
  return evResult;
};

bool WestInterfaceFacade::notify(std::int16_t elementId, std::uint8_t event,
                                 std::string value) {
  bool evResult = _eventObserver->handleEvent(elementId, event, -1, -1, value);
  return false;
};

std::vector<ElementBounds *> WestInterfaceFacade::getShownElementsBoundaries() {
  std::vector<ElementBounds *> result;
  for (ContainerElement *ce : _interfaces) {
    if (ce == nullptr)
      break;
    for (IElement *el : ce->children) {
      if (!el->supportsEvents)
        continue;
      ElementBounds *b = new ElementBounds();
      b->id = el->id;
      b->xLeft = el->xLL;
      b->yBottom = el->yLL;

      float elementWidth = el->getElementWidth();
      b->xRight = el->xLL + elementWidth;
      b->yTop = el->yLL + (el->rowElements * SIZE_E * el->stretchY);
      result.push_back(b);
    }
  }
  return result;
}

///////////////////////////

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
