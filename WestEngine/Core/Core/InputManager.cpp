#include "../CoreHeaders/InputManager.h"

#include "../CoreHeaders/Utils/InputUtils/KeyboardCallbacks.h"
#include "../CoreHeaders/Utils/InputUtils/MouseCallbacks.h"
#include "../CoreHeaders/Utils/TimeUtils.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
  <include> direct.h
  #define getcwd _getcwd
  #define PATH_MAX MAX_PATH
#else
  #include <unistd.h>
  #include <limits.h>
#endif



#define SH_DENYNO 0x40

InputManager::InputManager() : IManager(nullptr) {
  setName(CoreConstants::INPUT_MANAGER);
}

InputManager::InputManager(WestLogger *logger) : IManager(logger) {
  setName(CoreConstants::INPUT_MANAGER);
}

InputManager::~InputManager() {}

std::int32_t InputManager::startup() {

  char cwd[PATH_MAX];
  char filePath[PATH_MAX];
  if (getcwd(cwd, sizeof(cwd)) == NULL)
    return 1;

#ifdef DEBUG
  snprintf(filePath, sizeof(filePath), "%s\n", cwd); 
  logDebug(filePath);
#endif

  std::int32_t fd;
  snprintf(filePath, sizeof(filePath), "%s%s", cwd,
           CoreConstants::INPUT_CONFIG_FILE_NAME);
  if ((fd = open(filePath, O_RDONLY)) == -1) {
    getString()->format("%s ### Error opening Input Config file: &s\n %s\n",
                        getName(), filePath,strerror(errno));
    logFailure(getString()->getBuffer());
    return 1;
  }

  if ((_inputConfig = fdopen(fd, "r")) == NULL) {
    getString()->format("%s ### Error opening Input Config filestream\n",
                        getName());
    logFailure(getString()->getBuffer());
    return 1;
  }

  snprintf(filePath, sizeof(filePath), "%s%s", cwd,
           CoreConstants::AVAILABLE_INPUTS_FILE_NAME);
  if ((fd = open(filePath, O_RDONLY)) == -1) {
    getString()->format("%s ### Error opening avialable Inputs file: %s\n %s\n",
                        getName(), filePath,strerror(errno));
    logFailure(getString()->getBuffer());
    return 1;
  }

  if ((_availableCommands = fdopen(fd, "r")) == NULL) {
    getString()->format("%s ### Error opening avialable Inputs filestream\n",
                        getName());
    logFailure(getString()->getBuffer());
    return 1;
  }

  assert(_inputConfig != NULL && _availableCommands != NULL);
  KeyboardCallbacks::setInputManager(this);

#ifdef DEBUG
  getString()->format("%s ### Loaded files for Input Configuration %s...\n",
                      getName(), CoreConstants::INPUT_CONFIG_FILE_NAME);
  logDebug(getString()->getBuffer());
#endif
  return 0;
}

void InputManager::shutdown() {
#ifdef DEBUG
  getString()->format("%s ### Shutting down %s...\n", getName(), getName());
  logDebug(getString()->getBuffer());
#endif
  delete _inputConfig;
  delete _availableCommands;
  delete _observer;
}

std::int32_t InputManager::init() {
#ifdef DEBUG
  double start = TimeUtils::getCurrentTimeAsTime();
#endif

  std::int32_t success = 0;
  std::list<const char *> _commandList;
  char inbuf[128];

  while (fgets(inbuf, 128, _availableCommands) != NULL) {
    if (inbuf[0] == '#' || inbuf[0] == '\n')
      continue;

    inbuf[strcspn(inbuf, "\n")] = 0;
    _commandList.emplace_back(strdup(inbuf));
  }

  while (fgets(inbuf, 128, _inputConfig) != NULL) {
    if (inbuf[0] == '#' || inbuf[0] == '\n')
      continue;

    inbuf[strcspn(inbuf, "\n")] = 0;
    char *equalSign = strchr(inbuf, '=');
#ifdef DEBUG
    getString()->format("%s ### Reading input from cfg: %s\n", getName(),
                        inbuf);
    logDebug(getString()->getBuffer());
#endif
    if (equalSign) {
      *equalSign = '\0';
      const char *key = strdup(inbuf);
      const char *value = strdup(equalSign + 1);
      success = checkInputConfigLineForErrors(key, value, _commandList);
      if (success != 0)
        break;
    }
  }

  fclose(_inputConfig);
  fclose(_availableCommands);

  _observer = new InputObserver();
  KeyboardCallbacks::setInputObserver(_observer);
  MouseCallbacks::setInputObserver(_observer);

#ifdef DEBUG
  double end = TimeUtils::getCurrentTimeAsTime();
  double res = TimeUtils::getDuration(start, end);
  getString()->format("%s ### InputManager init time: %f ms.\n", getName(),
                      res);
  logDebug(getString()->getBuffer());
#endif

  return success;
}

void InputManager::update() {
  for (const auto &entry : _inputMap) {
    assert(entry.first > -1 && entry.second != nullptr);
    KeyboardCallbacks::executeBoundOperation(entry.first, entry.second);
  }
  _observer->notify();
}

void InputManager::setKey(std::int32_t key, const char *command) {
  _inputMap[key] = command;
}

std::int32_t InputManager::findByOperation(const char *command) {
#ifdef DEBUG
  getString()->format("%s ### Searching for command %s.\n", getName(), command);
#endif
  for (const auto &entry : _inputMap) {
    if (strcmp(entry.second, command) == 0)
      return entry.first;
  }
  return -1;
}

const char *InputManager::findByKey(std::int32_t key) {
#ifdef DEBUG
  getString()->format("%s ### Searching for Key %d.\n", getName(), key);
#endif
  for (const auto &entry : _inputMap) {
    if (entry.first == key)
      return entry.second;
  }
  return nullptr;
}

std::int32_t InputManager::checkInputConfigLineForErrors(
    const char *key, const char *value, std::list<const char *> _commandList) {
  if (strlen(value) == 0) {
    return 0;
  }

  bool found = false;

  for (const char *command : _commandList) {
    if (strcmp(command, value) == 0)
      found = true;
  }

  if (!found) {
    getString()->format("Command not supported: %s", value);
    logFailure(getString()->getBuffer());
    return 1;
  }

  if (strlen(key) == 1) {
    _inputMap[static_cast<int>(key[0])] = strdup(value);
  } else if (strcmp(key, "ESC") == 0) {
    _inputMap[GLFW_KEY_ESCAPE] = strdup(value);
  } else {
    // TODO Mouse Input
  }

  return 0;
}
