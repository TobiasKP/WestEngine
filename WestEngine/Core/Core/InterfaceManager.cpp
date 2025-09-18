#include "../CoreHeaders/InterfaceManager.h"

InterfaceManager::InterfaceManager() : IManager(nullptr) {
  setName(CoreConstants::INTERFACE_MANAGER);
  _facade = nullptr;
};

InterfaceManager::InterfaceManager(WestLogger *logger) : IManager(logger) {
  setName(CoreConstants::INTERFACE_MANAGER);
  _facade = nullptr;
};

InterfaceManager::~InterfaceManager() {}

std::int32_t InterfaceManager::startup() {
  _facade = &WestInterfaceFacade::getInterfaceInstance();
  assert(_facade != nullptr);
  return 0;
}

void InterfaceManager::shutdown() { _facade->shutdown(); }

std::int32_t InterfaceManager::init() {
#ifdef DEBUG
  
#endif
  return 0;
}

void InterfaceManager::update() {}
