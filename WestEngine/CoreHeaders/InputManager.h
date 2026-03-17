#pragma once

#include "../Core/Events/EventDispatcher.hpp"
#include "Interfaces/IManager.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <list>
#include <map>
#include <WestInterfaceFacade.h>

class InputManager : public IManager
{
public:
  InputManager();
  InputManager(WestLogger* logger, std::shared_ptr<EventDispatcher> d);
  ~InputManager();

  // Overrides
  std::int32_t startup() override;
  void shutdown() override;
  void update() override;
  std::int32_t init() override;

  // Functions
  void setKey(std::int32_t key, std::string command);
  static std::map<std::int32_t, std::string> getInputMap()
  {
    return _inputMap;
  }

private:
  std::mutex _m;
  static std::map<std::int32_t, std::string> _inputMap;
  std::shared_ptr<EventDispatcher> _dispatcher;
  FILE* _inputConfig;
  FILE* _availableCommands;

  // Functions
  std::int32_t checkInputConfigLineForErrors(std::string key, std::string value, std::list<std::string> _commandList);
};
