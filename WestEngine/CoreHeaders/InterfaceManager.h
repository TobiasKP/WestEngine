#pragma once

#include "../Core/Events/EventDispatcher.hpp"
#include "Interfaces/IManager.h"
#include "WindowManager.h"

#include <WestInterfaceFacade.h>

class InterfaceManager : public IManager
{
public:
  InterfaceManager();
  InterfaceManager(WestLogger* logger, WindowManager* manager, std::shared_ptr<EventDispatcher> d);
  ~InterfaceManager() override;

  std::int32_t startup() override;
  void shutdown() override;
  void update() override;
  std::int32_t init() override;
  void pollEvents() override;

  static int registerInterface(lua_State*);
  static int updateInterfaceValue(lua_State*);
  static int destroyInterface(lua_State*);

private:
  static std::vector<WestInterface::ElementProxy*> fillInfo(lua_State* L);
  std::tuple<std::int16_t, bool> isInterfaceHovered();
  void refreshGameInterfaces();

  std::mutex _m;
  std::uint32_t _cachedInterfaces;
  std::uint32_t _currentX, _currentY, _currentTE, _currentCE;
  glm::vec2 _currentPos;
  std::tuple<std::int16_t, bool> _currentHover;

  std::vector<ElementBounds*> _elements;
  std::shared_ptr<EventDispatcher> _dispatcher;
  WestInterface::WestInterfaceFacade* _facade;
  WindowManager* _windowManager;

#ifdef DEBUG
  std::uint8_t _demoId;
  std::uint8_t buildTechDemoFooter();
  void refreshTechDemoFooter();
#endif
};
