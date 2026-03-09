#pragma once

#include "Interfaces/IManager.h"
#include "WindowManager.h"

#include <WestInterfaceFacade.h>

class InterfaceManager : public IManager
{
public:
  InterfaceManager();
  InterfaceManager(WestLogger* logger, WindowManager* manager);
  ~InterfaceManager() override;

  std::int32_t startup() override;
  void shutdown() override;
  void update() override;
  std::int32_t init() override;

  static int registerInterface(lua_State*);
  static int updateInterfaceValue(lua_State*);
  static int destroyInterface(lua_State*);

private:
  static std::vector<WestInterface::ElementProxy*> fillInfo(lua_State* L);
  void refreshGameInterfaces();

  std::uint32_t _cachedInterfaces;
  std::uint32_t _currentX, _currentY, _currentTE, _currentCE;

  WestInterface::WestInterfaceFacade* _facade; 
  WindowManager* _windowManager;

#ifdef DEBUG
  std::uint8_t _demoId;
  std::uint8_t buildTechDemoFooter();
  void refreshTechDemoFooter();
#endif
};
