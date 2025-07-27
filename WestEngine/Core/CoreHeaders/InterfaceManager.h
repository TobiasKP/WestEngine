#pragma once

#include "Interfaces/IManager.h"
//#include "/Interfaces/IUserInterface.h"

class InterfaceManager : public IManager {
public:
  InterfaceManager();
  InterfaceManager(WestLogger *logger);
  ~InterfaceManager() override;

  std::int32_t startup() override;
  void shutdown() override;
  void update() override;
  std::int32_t init() override;

  //static std::vector<IUserInterface *> getInterfaces() { return _interfaces; }

private:
 // static std::vector<IUserInterface *> _interfaces;

  static void addInterface(/*IUserInterface *interface*/) {
   // _interfaces.emplace_back(interface);
  }
  void createMainMenu();
};
