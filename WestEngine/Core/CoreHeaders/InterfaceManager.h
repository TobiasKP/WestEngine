#pragma once

#include "Interfaces/IManager.h"

#include <WestInterfaceFacade.h>

class InterfaceManager : public IManager {
public:
  InterfaceManager();
  InterfaceManager(WestLogger *logger);
  ~InterfaceManager() override;

  std::int32_t startup() override;
  void shutdown() override;
  void update() override;
  std::int32_t init() override;

private:
  WestInterfaceFacade *_facade;

#ifdef DEBUG
  std::int32_t buildTechDemoFooter();
#endif
};
