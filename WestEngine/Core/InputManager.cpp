#include "../CoreHeaders/InputManager.h"

#include "../CoreHeaders/Utils/InputUtils/KeyboardCallbacks.h"
#include "../CoreHeaders/Utils/InputUtils/MouseCallbacks.h"

#include <cstring>
#include <format>
#include <PathUtils.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef _WIN32
#include <limits.h>
#endif

#define SH_DENYNO 0x40

InputManager::InputManager() : IManager(nullptr)
{
  setName(CoreConstants::INPUT_MANAGER);
}

InputManager::InputManager(WestLogger* logger) : IManager(logger)
{
  setName(CoreConstants::INPUT_MANAGER);
}

InputManager::~InputManager() {}

std::int32_t InputManager::startup()
{
#ifdef DEBUG
  logDebug(std::format("{} ### Executable dir: {}", getName(), PathUtils::getExecutableDir()));
#endif

  std::int32_t fd;
  _inputConfig       = PathUtils::openFile(CoreConstants::INPUT_CONFIG_FILE_NAME, true);
  _availableCommands = PathUtils::openFile(CoreConstants::AVAILABLE_INPUTS_FILE_NAME, true);

  assert(_inputConfig != NULL && _availableCommands != NULL);
  KeyboardCallbacks::setInputManager(this);

#ifdef DEBUG
  logDebug(std::format(
    "{} ### Loaded files for Input Configuration {}...\n", getName(), CoreConstants::INPUT_CONFIG_FILE_NAME));
#endif
  return 0;
}

void InputManager::shutdown()
{
#ifdef DEBUG
  logDebug(std::format("{} ### Shutting down {}...\n", getName(), getName()));
#endif

  delete _observer;
}

std::int32_t InputManager::init()
{
#ifdef DEBUG
  double start = TimeUtils::getCurrentTimeAsTime();
#endif

  std::int32_t success = 0;
  std::list<std::string> _commandList;
  char inbuf[128];

  while (fgets(inbuf, 128, _availableCommands) != NULL)
  {
    if (inbuf[0] == '#' || inbuf[0] == '\n' || inbuf[0] == '\r')
    {
      continue;
    }

    inbuf[strcspn(inbuf, "\r\n")] = 0;
    _commandList.emplace_back(inbuf);
  }

  while (fgets(inbuf, 128, _inputConfig) != NULL)
  {
    if (inbuf[0] == '#' || inbuf[0] == '\n' || inbuf[0] == '\r')
    {
      continue;
    }

    inbuf[strcspn(inbuf, "\r\n")] = 0;
    char* equalSign               = strchr(inbuf, '=');
#ifdef DEBUG
    logDebug(std::format("{} ### Reading input from cfg: {}\n", getName(), inbuf));
#endif
    if (equalSign)
    {
      *equalSign = '\0';
      std::string key(inbuf);
      std::string value(equalSign + 1);
      success = checkInputConfigLineForErrors(key, value, _commandList);
      if (success != 0)
      {
        break;
      }
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
  logDebug(std::format("{} ### InputManager init time: {} ms.\n", getName(), res));
#endif

  return success;
}

void InputManager::update()
{
  for (const auto& entry : _inputMap)
  {
    assert(entry.first > -1 && entry.second.length() > 0);
    KeyboardCallbacks::executeBoundOperation(entry.first, entry.second);
  }
  _observer->notify();
}

void InputManager::setKey(std::int32_t key, std::string command)
{
  _inputMap[key] = command;
}

std::int32_t InputManager::findByOperation(std::string command)
{
#ifdef DEBUG
  logCycle(std::format("{} ### Searching for command {}.\n", getName(), command));
#endif
  for (const auto& entry : _inputMap)
  {
    if (entry.second.compare(command) == 0)
    {
      return entry.first;
    }
  }
  return -1;
}

const std::string InputManager::findByKey(std::int32_t key)
{
#ifdef DEBUG
  logCycle(std::format("{} ### Searching for Key {}.\n", getName(), key));
#endif
  for (const auto& entry : _inputMap)
  {
    if (entry.first == key)
    {
      return entry.second;
    }
  }
#ifdef DEBUG
  logDebug(std::format("{} ### No Key: {} found in input map.\n", getName(), key));
#endif
  return CoreConstants::UNDEFINED_STRING.data();
}

std::int32_t
InputManager::checkInputConfigLineForErrors(std::string key, std::string value, std::list<std::string> _commandList)
{
  if (value.length() == 0)
  {
    return 0;
  }

  bool found = false;
  for (std::string command : _commandList)
  {
    if (value.compare(command) == 0)
    {
      found = true;
    }
  }

  if (!found)
  {
    logFailure(std::format("Command not supported: {}", value));
    return 1;
  }

  if (key.length() == 1)
  {
    _inputMap[static_cast<int>(key[0])] = value;
  }
  else if (key.compare("ESC") == 0)
  {
    _inputMap[GLFW_KEY_ESCAPE] = value;
  }
  else
  {
    // TODO Mouse Input
  }

  return 0;
}
