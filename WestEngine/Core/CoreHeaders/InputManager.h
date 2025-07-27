#pragma once

#include "Interfaces/IManager.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <list>
#include <map>

#include "Utils/InputUtils/InputObserver.h"

class InputManager : public IManager {

public:
  InputManager();
  InputManager(WestLogger *logger);
  ~InputManager();

  // Overrides
  std::int32_t startup() override;
  void shutdown() override;
  void update() override;
  std::int32_t init() override;

  // Functions
  void setKey(std::int32_t key, const char *command);
  std::int32_t findByOperation(const char *command);
  const char *findByKey(std::int32_t key);

private:
  std::map<std::int32_t, const char *> _inputMap;
  InputObserver *_observer;
  FILE *_inputConfig;
  FILE *_availableCommands;

  // Functions
  std::int32_t
  checkInputConfigLineForErrors(const char *key, const char *value,
                                std::list<const char *> _commandList);
};
