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

private:
  std::uint8_t _cachedInterfaces;
  std::uint32_t _currentX, _currentY;

  WestInterfaceFacade* _facade;
  WindowManager* _windowManager;

#ifdef DEBUG
  std::int32_t buildTechDemoFooter();
#endif
};
