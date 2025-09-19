#include "../CoreHeaders/InputManager.h"

#include "../CoreHeaders/Utils/InputUtils/KeyboardCallbacks.h"
#include "../CoreHeaders/Utils/InputUtils/MouseCallbacks.h"

#include <TimeUtils.hpp>
#include <cstring>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
<include> direct.h
#define getcwd _getcwd
#define PATH_MAX MAX_PATH
#else
#include <limits.h>
#include <unistd.h>
#endif

#define SH_DENYNO 0x40

InputManager::InputManager()
    : IManager(nullptr) {
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
  logDebug(std::format("{} ### Current working dir: {}", getName(), filePath));
#endif

  std::int32_t fd;
  snprintf(filePath, sizeof(filePath), "%s%s", cwd,
           CoreConstants::INPUT_CONFIG_FILE_NAME);
  if ((fd = open(filePath, O_RDONLY)) == -1) {
    logFailure(std::format("{} ### Error opening Input Config file: {}\n {}\n",
                           getName(), filePath, std::strerror(errno)));
    return 1;
  }

  if ((_inputConfig = fdopen(fd, "r")) == NULL) {
    logFailure(std::format("{} ### Error opening Input Config filestream\n",
                           getName()));
    return 1;
  }

  snprintf(filePath, sizeof(filePath), "%s%s", cwd,
           CoreConstants::AVAILABLE_INPUTS_FILE_NAME);
  if ((fd = open(filePath, O_RDONLY)) == -1) {
    logFailure(
        std::format("{} ### Error opening avialable Inputs file: {}\n {}\n",
                    getName(), filePath, std::strerror(errno)));
    return 1;
  }

  if ((_availableCommands = fdopen(fd, "r")) == NULL) {
    logFailure(std::format("{} ### Error opening avialable Inputs filestream\n",
                           getName()));
    return 1;
  }

  assert(_inputConfig != NULL && _availableCommands != NULL);
  KeyboardCallbacks::setInputManager(this);

#ifdef DEBUG
  logDebug(std::format("{} ### Loaded files for Input Configuration {}...\n",
                       getName(), CoreConstants::INPUT_CONFIG_FILE_NAME));
#endif
  return 0;
}

void InputManager::shutdown() {
#ifdef DEBUG
  logDebug(std::format("{} ### Shutting down {}...\n", getName(), getName()));
#endif

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
    logDebug(
        std::format("{} ### Reading input from cfg: {}\n", getName(), inbuf));
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
  logDebug(
      std::format("{} ### InputManager init time: {} ms.\n", getName(), res));
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
  logDebug(
      std::format("{} ### Searching for command {}.\n", getName(), command));
#endif
  for (const auto &entry : _inputMap) {
    if (strcmp(entry.second, command) == 0)
      return entry.first;
  }
  return -1;
}

const std::string InputManager::findByKey(std::int32_t key) {
#ifdef DEBUG
  logDebug(std::format("{} ### Searching for Key {}.\n", getName(), key));
#endif
  for (const auto &entry : _inputMap) {
    if (entry.first == key)
      return entry.second;
  }
  return CoreConstants::UNDEFINED_STRING;
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
    logFailure(std::format("Command not supported: {}", value));
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
