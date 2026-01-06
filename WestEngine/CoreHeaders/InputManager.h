#pragma once

#include "Interfaces/IManager.h"
#include "Utils/InputUtils/InputObserver.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <list>
#include <map>

class InputManager : public IManager
{
public:
  InputManager();
  InputManager(WestLogger* logger);
  ~InputManager();

  // Overrides
  std::int32_t startup() override;
  void shutdown() override;
  void update() override;
  std::int32_t init() override;

  // Functions
  void setKey(std::int32_t key, std::string command);
  std::int32_t findByOperation(std::string command);
  const std::string findByKey(std::int32_t key);

private:
  std::map<std::int32_t, std::string> _inputMap;
  InputObserver* _observer;
  FILE* _inputConfig;
  FILE* _availableCommands;

  // Functions
  std::int32_t checkInputConfigLineForErrors(std::string key, std::string value, std::list<std::string> _commandList);
};
