#include "../WestInterfaceFacade.h"

#include "RenderManagment/TextRenderManager.h"

#include <PathUtils.h>

#include <format>
#include <GLFW/glfw3.h>
#include <stb_image.h>

using namespace WestInterface;

WestInterfaceFacade& WestInterfaceFacade::getInterfaceInstance()
{
  static WestInterfaceFacade instance;
  return instance;
}

WestInterfaceFacade::WestInterfaceFacade()
{
  _valueObserver = new ValueObserver(_logger);
  _eventObserver = new EventObserver(_logger);
  assert(_valueObserver != nullptr && _eventObserver != nullptr);
  _renderManager = new UIRenderManager();
  _textManager   = new TextRenderManager();
  _builder       = new InterfaceBuilder(_eventObserver, _valueObserver);
  assert(_builder != nullptr);
  _settings = new SettingsInterface(*_builder);
  for (std::uint8_t i = 0; i < 32; i++)
  {
    _interfaces.at(i) = nullptr;
  }
}

WestInterfaceFacade::~WestInterfaceFacade() {}

void WestInterfaceFacade::shutdown()
{
  while (Config::INTERNAL_UI_COUNT > 0)
  {
    delete _interfaces.at(Config::INTERNAL_UI_COUNT);
    Config::INTERNAL_UI_COUNT--;
  }
  delete _builder;
  delete _textManager;
}

std::int32_t WestInterfaceFacade::init()
{
  glGenVertexArrays(1, &_interfaceVAO);

  GLuint buffers[7] = {0};
  glGenBuffers(7, buffers);
  _interfaceVBO    = buffers[0];
  _interfaceEBO    = buffers[1];
  _interfaceCOL    = buffers[2];
  _interfaceOFFSET = buffers[3];
  _interfaceFLAGS  = buffers[4];
  _interfaceTEX    = buffers[5];
  _interfaceUV     = buffers[6];
  for (GLuint i : buffers)
  {
    if (i == -1)
    {
      _logger.log(Level::Error, "--- Error generating Buffers, check OpenGL error logs.\n");
      return -1;
    }
  }

  glGenTextures(1, &_interfaceFONT_TEXTURE_ID);

  glBindVertexArray(_interfaceVAO);

  glBindBuffer(GL_ARRAY_BUFFER, _interfaceVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(baseQuad), baseQuad, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _interfaceEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  setupInstancedAttribute(_interfaceCOL, 1, 4, true);
  setupInstancedAttribute(_interfaceOFFSET, 2, 4, true);

  glBindBuffer(GL_ARRAY_BUFFER, _interfaceTEX);
  glBufferData(GL_ARRAY_BUFFER, sizeof(baseTex), baseTex, GL_STATIC_DRAW);

  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(3);

  setupInstancedAttribute(_interfaceUV, 4, 4, true);

  glBindBuffer(GL_ARRAY_BUFFER, _interfaceFLAGS);
  glBufferData(GL_ARRAY_BUFFER, NULL, NULL, GL_STATIC_DRAW);
  glVertexAttribIPointer(5, 1, GL_UNSIGNED_INT, sizeof(std::uint32_t), (void*)0);
  glVertexAttribDivisor(5, 1);
  glEnableVertexAttribArray(5);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  std::int32_t width, height, numComponents;
  std::string filePath = PathUtils::resolve("/assets/Textures/courier.bmp");

  unsigned char* imgData = stbi_load(filePath.c_str(), &width, &height, &numComponents, 0);
  if (imgData == NULL)
  {
    _logger.log(
      Level::Error,
      std::format("---No Imagedata loaded for texture: {} - STBI Error: {}\n", filePath, stbi_failure_reason()));
    return -1;
  }

  glBindTexture(GL_TEXTURE_2D, _interfaceFONT_TEXTURE_ID);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imgData);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(imgData);

  _textManager->initializeFontAtlas();
  ContainerElement* tmp                     = _settings->init();
  _interfaces.at(Config::INTERNAL_UI_COUNT) = tmp;
  Config::INTERNAL_UI_COUNT++;
  _renderManager->toggleDirty();
  return 0;
}

void WestInterfaceFacade::setupInstancedAttribute(GLuint buffer, GLuint index, GLint size, bool withDivisor)
{
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glBufferData(GL_ARRAY_BUFFER, NULL, NULL, GL_STATIC_DRAW);
  glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, size * sizeof(float), (void*)0);
  if (withDivisor)
  {
    glVertexAttribDivisor(index, 1);
  }
  glEnableVertexAttribArray(index);
}

ContainerElement* WestInterfaceFacade::findInterfaceById(std::uint8_t id)
{
  if (id <= 0)
  {
    return nullptr;
  }

  for (std::uint8_t i = 0; i < Config::INTERNAL_UI_COUNT; i++)
  {
    assert(_interfaces.at(i) != nullptr);
    if (_interfaces.at(i)->id == id)
    {
      return _interfaces.at(i);
    }
  }

  return nullptr;
}

std::uint8_t WestInterfaceFacade::createNewInterface(Container* c)
{
  _builder->createNewInterface(c);

  if (c->background != nullptr)
  {
    _builder->createBackground(c->background);
  }

  for (auto* element : c->elements)
  {
#ifdef DEBUG
    _logger.log(Level::Cycle, std::format("@@@ Addding new element to interface: {}\n", (int)element->type));
#endif
    _builder->addElement(element);
  }
  ContainerElement* interface               = _builder->build();
  _interfaces.at(Config::INTERNAL_UI_COUNT) = interface;
  Config::INTERNAL_UI_COUNT++;
  _renderManager->toggleDirty();
  return interface->id;
}

bool WestInterfaceFacade::destroyInterface(std::uint8_t interfaceId)
{
  std::int8_t location = -1;
  for (std::uint8_t i = 0; i < Config::INTERNAL_UI_COUNT; i++)
  {
    if (_interfaces.at(i)->id == interfaceId)
    {
      location = i;
      break;
    }
  }

  if (location == -1)
  {
    return false;
  }

  ContainerElement* elementToDelete = _interfaces.at(location);
  for (IElement* child : elementToDelete->children)
  {
    // Deregister all elements to prevent use-after-free crashes
    _eventObserver->deregisterElement(child);
    _valueObserver->deregisterElement(child);
  }

  if (location != Config::INTERNAL_UI_COUNT - 1)
  {
    _interfaces.at(location) = _interfaces.at(Config::INTERNAL_UI_COUNT - 1);
  }
  _interfaces.at(Config::INTERNAL_UI_COUNT - 1) = nullptr;
  Config::INTERNAL_UI_COUNT--;

  _renderManager->toggleDirty();
  delete elementToDelete;
  return true;
}

void WestInterfaceFacade::addElement(std::uint8_t interfaceId, ElementProxy* element)
{
  ContainerElement* interface = findInterfaceById(interfaceId);
  if (interface == nullptr)
  {
    return;
  }

  assert(element != nullptr);
  IElement* e = _builder->transform(element);
  assert(e != nullptr);
#ifdef DEBUG
  _logger.log(Level::Info,
              std::format("@@@ Adding additional element to interface: {}, after creation\n", interfaceId));
#endif

  _renderManager->toggleDirty();
  interface->addChild(e, element->column, element->row);
};

bool WestInterfaceFacade::removeElement(std::uint8_t interfaceId, std::uint32_t elementId)
{
  ContainerElement* interface = findInterfaceById(interfaceId);
  if (interface == nullptr)
  {
    return false;
  }

  _renderManager->toggleDirty();
  return interface->deleteChildById(elementId);
};

void WestInterfaceFacade::updateRenderData()
{
#ifdef DEBUG
  _logger.log(Level::Cycle, "@@@ Updating render Data for interfaces\n");
#endif
  _renderManager->updateRenderData(_interfaces, Config::INTERNAL_UI_COUNT);
};

const std::vector<ComponentData*>& WestInterfaceFacade::getRenderData() const
{
  return _renderManager->getRenderData();
};

void WestInterfaceFacade::notify(std::int16_t elementId, std::uint8_t event, std::uint16_t mouseX, std::uint16_t mouseY)
{
  _eventObserver->handleEvent(elementId, event, mouseX, mouseY, "");
  _renderManager->toggleDirty();
};

void WestInterfaceFacade::notify(std::int16_t elementId, std::uint8_t event, std::string value)
{
  _eventObserver->handleEvent(elementId, event, -1, -1, value);
  _renderManager->toggleDirty();
};

const std::vector<ElementBounds*>& WestInterfaceFacade::getShownElementsBoundaries() const
{
  return _renderManager->getBoundaries();
}

bool WestInterfaceFacade::resolutionChange(std::uint32_t width, std::uint32_t height)
{
  _settings->refreshInterface();
  return true;
}

///////////////////////////

bool WestInterfaceFacade::resize(std::int8_t interfaceId, std::uint32_t width, std::uint32_t height)
{
  return true;
};

bool WestInterfaceFacade::reposition(std::uint8_t interfaceId,
                                     std::uint16_t xScreenPosition,
                                     std::uint16_t yScreenPosition)
{
  return false;
};

const char* WestInterfaceFacade::getResource(std::string resource)
{
  return NULL;
}
