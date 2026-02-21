#include "../CoreHeaders/EngineManager.h"

#include "../CoreHeaders/InterfaceManager.h"
#include "../CoreHeaders/RenderManager.h"
#include "../CoreHeaders/SceneManager.h"
#include "../CoreHeaders/ShaderManager.h"
#include "../CoreHeaders/SystemManager.h"
#include "../CoreHeaders/Utils/InputUtils/KeyboardCallbacks.h"

#include <cstring>
#include <format>
#include <PathUtils.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef _WIN32
#include <limits.h>
#endif

#define SH_DENYNO 0x40


EngineManager::EngineManager() : IManager(nullptr)
{
  setName(CoreConstants::ENGINE_MANAGER);
  _exitEngine    = true;
  _windowManager = nullptr;
}

EngineManager::EngineManager(WestLogger* logger) : IManager(logger)
{
  setName(CoreConstants::ENGINE_MANAGER);
  _exitEngine    = false;
  _windowManager = new WindowManager(logger);

  _manager[0] = new InputManager(logger);
  _manager[1] = _windowManager;
  _manager[2] = new ShaderManager(logger);
  _manager[3] = new SystemManager(logger);
  _manager[4] = new InterfaceManager(logger, _windowManager);
  _manager[5] = new RenderManager(logger);
  _manager[6] = new SceneManager(logger);
  assert(_manager.size() == CoreConstants::MAX_Q_SIZE);
}

EngineManager::~EngineManager()
{
  for (auto* manager : _manager)
  {
    if (manager && manager != _windowManager)
    {
      delete manager;
    }
  }
  delete _windowManager;
}

std::int32_t EngineManager::startup()
{
#ifdef DEBUG
  double start = TimeUtils::getCurrentTimeAsTime();
#endif

  std::int32_t success = initializeSettings();
  if (success == -1)
  {
    logFailure("Error occured during settings initialization...\n");
    return success;
  }
  success = iterateQ(CYCLE::STARTUP);
  logDebug(std::format("{} ### Startup complete\n", getName()));

#ifdef DEBUG
  double end = TimeUtils::getCurrentTimeAsTime();
  double res = TimeUtils::getDuration(start, end);
  logDebug(std::format("{} ### All Managers started! startup time: {} ms.\n", getName(), res));
#endif

  return success;
}

void EngineManager::shutdown()
{
#ifdef DEBUG
  logDebug(std::format("{} ### Shutting down {}...\n", getName(), getName()));
#endif
  for (auto* item : _manager)
  {
    if (item)
    {
      item->shutdown();
    }
  }
  KeyboardCallbacks::shutdown();
}

void EngineManager::update()
{
  std::int32_t frames = 0, success = 0;
  double frameCounter = 0;
  double lastTime     = TimeUtils::getNanoseconds();
  double delta        = 0;

#ifdef DEBUG
  logDebug(std::format("{} ### STARTING MAIN GAME LOOP.\n", getName()));
#endif

  while (!_exitEngine)
  {
    while (Config::PAUSE)
    {
      iterateQ(CYCLE::PAUSE);
      _windowManager->setWindowTitle("paused ...");
    }

    bool render       = false;
    double startTime  = TimeUtils::getNanoseconds();
    double passedTime = startTime - lastTime;
    lastTime          = startTime;

    delta        += passedTime / (double)_NANOSECOND;
    frameCounter += passedTime;

    while (delta > _FRAMETIME)
    {
      render  = true;
      delta  -= _FRAMETIME;

      if (_windowManager->windowShouldClose())
      {
        _exitEngine = true;
        break;
      }

      if (frameCounter >= _NANOSECOND)
      {
        setFps(frames);
        _windowManager->setWindowTitle(std::format("{} : {}", CoreConstants::TITLE, getFps()));

        frames       = 0;
        frameCounter = 0;
      }
    }

    if (render)
    {
      success = iterateQ(CYCLE::UPDATE);
      frames++;
    }
    else
    {
      double remainingTime = _FRAMETIME - delta;
      if (remainingTime >= Config::GeneralConfig.EPSILON)
      {
        std::int32_t sleepMs = static_cast<std::int32_t>(remainingTime * 1000) - 1;
        if (sleepMs > 0)
        {
          std::this_thread::sleep_for(std::chrono::milliseconds(sleepMs));
        }
      }
    }

    if (Config::requestedWidth > 0 && Config::requestedHeight > 0)
    {
      _windowManager->resizeWindow(Config::requestedWidth, Config::requestedHeight);
      Config::requestedWidth  = -1;
      Config::requestedHeight = -1;
    }
  }
}

std::int32_t EngineManager::init()
{
#ifdef DEBUG
  double start = TimeUtils::getCurrentTimeAsTime();
#endif

  std::int32_t success = iterateQ(CYCLE::INIT);

#ifdef DEBUG
  double end = TimeUtils::getCurrentTimeAsTime();
  double res = TimeUtils::getDuration(start, end);
  logDebug(std::format("{} ### All Managers initialized! init time: {} ms.\n", getName(), res));
#endif
  return success;
}

// TODO: all managers execute sequentially, independent ones (Input, System, Interface) could run in parallel
std::int32_t EngineManager::iterateQ(CYCLE code)
{
  assert(typeid(code) == typeid(EngineManager::CYCLE));
  std::int32_t success = 0;

  for (std::int32_t i = 0; i < CoreConstants::MAX_Q_SIZE; i++)
  {
#ifdef DEBUG
    double start = TimeUtils::getCurrentTimeAsTime();
#endif
    auto* item = _manager[i];
    if (!item)
    {
      logFailure(std::format("{} ### Item not retreivable something went horribly wrong!", getName()));
      return 1;
    }

    if (isPauseCycle(code, *item))
    {
      continue;
    }

    success = executeCycle(code, *item);

    if (success != 0)
    {
#ifdef DEBUG
      int enumCode = code;
      logFailure(std::format("{} ### Failure in {} for cycle: {}\n", getName(), item->getName(), enumCode));
#endif
      break;
    }


#ifdef DEBUG
    if (code == CYCLE::UPDATE)
    {
      double end = TimeUtils::getCurrentTimeAsTime();
      double res = TimeUtils::getDuration(start, end);
      logCycle(std::format("{} ### Queue time: {} ms. for: {} \n", getName(), res, item->getName()));
    }
#endif
  }

  return success;
}

std::int32_t EngineManager::executeCycle(CYCLE code, IManager& item)
{
  switch (code)
  {
    case CYCLE::STARTUP:
      return item.startup();
    case CYCLE::INIT:
      return item.init();
    case CYCLE::UPDATE:
    case CYCLE::PAUSE:
      item.update();
      return 0;
    default:
      logFailure(std::format("{} ### unknown territory ... code: {}\n", getName(), (int)code));
      return 1;
  }
}

std::int32_t EngineManager::initializeSettings()
{
#ifdef DEBUG
  logDebug(std::format("{} ### Initialized Settings from saved Configuration file.", getName()));
#endif
  std::int32_t success = 0;

#ifdef DEBUG
  logDebug(std::format("{} ### Executable dir: {}", getName(), PathUtils::getExecutableDir()));
#endif

  Config::GeneralConfig.SETTINGS = PathUtils::openFile(CoreConstants::SETTING_FILE_NAME, true);
  char inbuf[128];
  while (fgets(inbuf, 128, Config::GeneralConfig.SETTINGS) != NULL)
  {
    if (inbuf[0] == '#' || inbuf[0] == '\n')
    {
      continue;
    }

    inbuf[strcspn(inbuf, "\n")] = 0;
    char* equalSign             = strchr(inbuf, '=');
#ifdef DEBUG
    logDebug(std::format("{} ### Reading Setting: {}\n", getName(), inbuf));
#endif
    if (equalSign)
    {
      *equalSign = '\0';
      std::string key(inbuf);
      std::string value(equalSign + 1);
      success = fillSettings(key, value);
      if (success != 0)
      {
        break;
      }
    }
  }

  fclose(Config::GeneralConfig.SETTINGS);
  Config::GeneralConfig.SETTINGS = nullptr;
  return success;
}

std::int32_t EngineManager::fillSettings(std::string key, std::string value)
{
  std::int32_t success = 1;
  if (key.compare("width") == 0)
  {
    Config::GeneralConfig.WIDTH = stoi(value);
    success                     = 0;
  }
  else if (key.compare("height") == 0)
  {
    Config::GeneralConfig.HEIGHT = stoi(value);
    success                      = 0;
  }
  else if (key.compare("fps") == 0)
  {
    Config::GeneralConfig.FPS = stof(value);
    success                   = 0;
  }
  return success;
}

bool EngineManager::isPauseCycle(CYCLE code, IManager& item)
{
  bool pauseExecute = code == CYCLE::PAUSE;
  bool isManager    = CoreConstants::INPUT_MANAGER.compare(item.getName()) == 0
                   || CoreConstants::WINDOW_MANAGER.compare(item.getName()) == 0
                   || CoreConstants::INTERFACE_MANAGER.compare(item.getName()) == 0;

  return pauseExecute && !isManager;
}
