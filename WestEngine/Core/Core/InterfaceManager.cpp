#include "../CoreHeaders/InterfaceManager.h"

//#include "UserInterface.h"
#include <Config.h>
// std::vector<IUserInterface *> InterfaceManager::_interfaces;

InterfaceManager::InterfaceManager() : IManager(nullptr) {
  setName(CoreConstants::INTERFACE_MANAGER);
};

InterfaceManager::InterfaceManager(WestLogger *logger) : IManager(logger) {
  setName(CoreConstants::INTERFACE_MANAGER);
};

InterfaceManager::~InterfaceManager() {
  // for (IUserInterface *interface : _interfaces) {
  //  interface->~IUserInterface();
  // }
}

std::int32_t InterfaceManager::startup() {
  //Config::Interface.BITMAP_LOCATION = UserInterface::getBitmap();
  //Config::Interface.VERTEX_LOCATION = UserInterface::getVertexShader();
  //Config::Interface.FRAG_LOCATION = UserInterface::getFragmentShader();
  // _interfaces.reserve(8);
  return 0;
}

void InterfaceManager::shutdown() {
  // for (IUserInterface *interface : _interfaces) {
  //   interface->~IUserInterface();
  // }
}

std::int32_t InterfaceManager::init() {
  // assert(Global::UserInterface::SHADER_PROGRAM != -1 &&
  //       Global::UserInterface::ORTHO_UNIFORM != -1 &&
  //      Global::UserInterface::TEXTURE_SAMPLER != -1);
  createMainMenu();
  return 0;
}

void InterfaceManager::update() {}

void InterfaceManager::createMainMenu() {
  /* MainMenu *mainMenu = new MainMenu();
   mainMenu->addExitButton();
   mainMenu->toggleVisible();
   addInterface(mainMenu);*/
}
