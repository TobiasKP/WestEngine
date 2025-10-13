#pragma once

#if defined(_WIN32) || defined(_WIN64)
#ifdef WEST_INTERFACE_EXPORTS
#define WEST_INTERFACE __declspec(dllexport)
#else
#define WEST_INTERFACE __declspec(dllimport)
#endif
#else
#define WEST_INTERFACE __attribute__((visibility("default")))
#endif

#include "FacadeStructs.h"
#include "WestInterface/Elements/ContainerElement.cpp"
#include "WestInterface/InterfaceBuilder.h"
#include "WestInterface/Observer/EventObserver.h"
#include "WestInterface/Observer/ValueObserver.h"
#include "WestInterface/RenderManagment/ComponentData.h"
#include "WestInterface/RenderManagment/UIRenderManager.h"

#include <WestLogger.h>
#include <array>
#include <cstdint>
#include <string>

#include <GL/glew.h>

class WEST_INTERFACE WestInterfaceFacade {

public:
  static WestInterfaceFacade &getInterfaceInstance();
  void init();
  void shutdown();

  // Managing Interfaces
  std::uint8_t createNewInterface(std::uint16_t xScreenPosition,
                                  std::uint16_t yScreenPosition, float scale,
                                  std::uint8_t rows, std::uint8_t columns,
                                  bool hiddenContainer,
                                  std::vector<ElementProxy *> elements);
  bool destroyInterface(std::uint8_t interfaceId);

  // Adding Elements or removing
  void addElement(std::uint8_t interfaceId, ElementProxy *element);
  bool removeElement(std::uint8_t interfaceId, std::uint32_t elementId);

  // RenderLoop
  void updateRenderData();
  std::vector<ComponentData *> getRenderData();

  // Events
  bool notify(std::uint8_t event, std::uint16_t mouseX, std::uint16_t mouseY);
  bool notify(std::uint8_t event, std::string value);

  // Changes to Interface
  bool resize(std::uint8_t interfaceId, std::uint16_t width,
              std::uint16_t height);
  bool reposition(std::uint8_t interfaceId, std::uint16_t xScreenPosition,
                  std::uint16_t yScreenPosition);

  // Get Resources
  const char *getResource(std::string resource);

  static constexpr std::string_view interfaceVertexShader =
      "shader/InterfaceVertexShader.vs";
  static constexpr std::string_view interfaceFragementShader =
      "shader/InterfaceFragementShader.fs";
  static constexpr std::uint32_t indices[6] = {0, 1, 3, 1, 2, 3};
  static constexpr float baseQuad[] = {0.0f, 0.0f,          1.0f * SIZE_E,
                                       0.0f, 1.0f * SIZE_E, 1.0f * SIZE_E,
                                       0.0f, 1.0f * SIZE_E};
  static constexpr float baseTex[] = {0.0f, 0.0f, 1.0f, 0.0f,
                                      1.0f, 1.0f, 0.0f, 1.0f};
  GLuint _interfaceVBO, _interfaceVAO, _interfaceEBO, _interfaceCOL,
      _interfaceOFFSET, _interfaceFLAGS, _interfaceTEX;

  friend class UIRenderManager;

protected:
  size_t count;

private:
  WestInterfaceFacade();
  ~WestInterfaceFacade();

  std::array<ContainerElement *, 32> _interfaces;

  ValueObserver *_valueObserver;
  EventObserver *_eventObserver;
  InterfaceBuilder *_builder;
  UIRenderManager *_renderManager;
  WestLogger &_logger = WestLogger::getLoggerInstance();

  ContainerElement *findInterfaceById(std::uint8_t id);
};
