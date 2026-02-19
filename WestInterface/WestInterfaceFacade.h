#pragma once
#include "FacadeStructs.h"
#include "WestInterface/Elements/ContainerElement.hpp"
#include "WestInterface/InterfaceBuilder.h"
#include "WestInterface/Observer/EventObserver.h"
#include "WestInterface/Observer/ValueObserver.h"
#include "WestInterface/RenderManagment/ComponentData.h"
#include "WestInterface/RenderManagment/TextRenderManager.h"
#include "WestInterface/RenderManagment/UIRenderManager.h"
#include "WestInterface/SettingsInterface.h"

#include <array>
#include <cstdint>
#include <GL/glew.h>
#include <string>
#include <WestLogger.h>

namespace WestInterface
{

class WestInterfaceFacade
{
public:
  static WestInterfaceFacade& getInterfaceInstance();
  std::int32_t init();
  void shutdown();

  // Managing Interfaces
  std::uint8_t createNewInterface(Container* c);
  bool destroyInterface(std::uint8_t interfaceId);

  // Adding Elements or removing
  void addElement(std::uint8_t interfaceId, ElementProxy* element);
  bool removeElement(std::uint8_t interfaceId, std::uint32_t elementId);

  // RenderLoop
  void updateRenderData();
  const std::vector<ComponentData*>& getRenderData() const;

  // Events
  void notify(std::int16_t elementId, std::uint8_t event, std::uint16_t mouseX, std::uint16_t mouseY);
  void notify(std::int16_t elementId, std::uint8_t event, std::string value);

  // Changes to Interface
  bool resize(std::int8_t interfaceId, std::uint32_t width, std::uint32_t height);
  bool reposition(std::uint8_t interfaceId, std::uint16_t xScreenPosition, std::uint16_t yScreenPosition);
  bool resolutionChange(std::uint32_t width, std::uint32_t height);

  // Get Resources TODO: reallobrate
  const char* getResource(std::string resource);

  const std::vector<ElementBounds*>& getShownElementsBoundaries() const;

  static constexpr std::string_view interfaceVertexShader    = "shader/InterfaceVertexShader.vs";
  static constexpr std::string_view interfaceFragementShader = "shader/InterfaceFragementShader.fs";
  static constexpr std::uint32_t indices[6]                  = {0, 1, 3, 1, 2, 3};
  static constexpr float baseQuad[]                          = {
    0.0f, 0.0f, 1.0f * SIZE_E, 0.0f, 1.0f * SIZE_E, 1.0f * SIZE_E, 0.0f, 1.0f * SIZE_E};
  static constexpr float baseTex[] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};

  GLuint _interfaceVBO, _interfaceVAO, _interfaceEBO, _interfaceCOL, _interfaceOFFSET, _interfaceFLAGS, _interfaceTEX,
    _interfaceFONT_TEXTURE_ID, _interfaceUV;

  friend class UIRenderManager;


private:
  WestInterfaceFacade();
  ~WestInterfaceFacade();

  std::array<ContainerElement*, 32> _interfaces{nullptr};

  ValueObserver* _valueObserver   = nullptr;
  EventObserver* _eventObserver   = nullptr;
  InterfaceBuilder* _builder      = nullptr;
  UIRenderManager* _renderManager = nullptr;
  TextRenderManager* _textManager = nullptr;
  SettingsInterface* _settings    = nullptr;
  WestLogger& _logger             = WestLogger::getLoggerInstance();

  ContainerElement* findInterfaceById(std::uint8_t id);
  void setupInstancedAttribute(GLuint buffer, GLuint index, GLint size, bool withDivisor);
};
};  // namespace WestInterface
