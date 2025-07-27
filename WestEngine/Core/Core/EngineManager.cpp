#include "../CoreHeaders/EngineManager.h"

#include "../Config/Config.h"
#include "../Constants/CoreConstants.h"

#include "../CoreHeaders/Utils/InputUtils/KeyboardCallbacks.h"
#include "../CoreHeaders/Utils/TimeUtils.h"

#include "../CoreHeaders/InputManager.h"
#include "../CoreHeaders/InterfaceManager.h"
#include "../CoreHeaders/RenderManager.h"
#include "../CoreHeaders/SceneManager.h"
#include "../CoreHeaders/ShaderManager.h"
#include "../CoreHeaders/SystemManager.h"

EngineManager::EngineManager() : IManager(nullptr) {
  setName(CoreConstants::ENGINE_MANAGER);
  _exitEngine = true;
  _engineQ = nullptr;
  _windowManager = nullptr;
}

EngineManager::EngineManager(WestLogger *logger) : IManager(logger) {
  setName(CoreConstants::ENGINE_MANAGER);
  _exitEngine = false;
  _windowManager = new WindowManager(logger);

  _engineQ = new WestQ(CoreConstants::MAX_Q_SIZE, logger);
  _engineQ->enqueue(new InputManager(logger));
  _engineQ->enqueue(_windowManager);
  _engineQ->enqueue(new ShaderManager(logger));
  _engineQ->enqueue(new SystemManager(logger));
  _engineQ->enqueue(new RenderManager(logger));
  _engineQ->enqueue(new InterfaceManager(logger));
  _engineQ->enqueue(new SceneManager(logger));
  assert(_engineQ->getSize() == _engineQ->getCapacity());
}

EngineManager::~EngineManager() {}

std::int32_t EngineManager::startup() {
#ifdef DEBUG
  double start = TimeUtils::getCurrentTimeAsTime();
#endif

  std::int32_t success = iterateQ(CYCLE::STARTUP);

  getString()->format("%s ### Startup complete\n", getName());
  logDebug(getString()->getBuffer());

#ifdef DEBUG
  double end = TimeUtils::getCurrentTimeAsTime();
  double res = TimeUtils::getDuration(start, end);
  getString()->format("%s ### All Managers started! startup time: %f ms.\n",
                      getName(), res);
  logDebug(getString()->getBuffer());
#endif

  return success;
}

void EngineManager::shutdown() {
#ifdef DEBUG
  getString()->format("%s ### Shutting down %s...\n", getName(), getName());
  logDebug(getString()->getBuffer());
#endif
  assert(_engineQ != nullptr && _engineQ->getSize() > 0);
  while (!_engineQ->isEmpty()) {
    IManager *item = _engineQ->dequeue();
    assert(item != nullptr);
    item->shutdown();
  }
  KeyboardCallbacks::shutdown();
  _engineQ->~WestQ();
  delete _engineQ;
}

void EngineManager::update() {
  std::int32_t frames = 0, success = 0;
  double frameCounter = 0;
  double lastTime = TimeUtils::getNanoseconds();
  double delta = 0;

#ifdef DEBUG
  getString()->format("%s ### STARTING MAIN GAME LOOP.\n", getName());
  logDebug(getString()->getBuffer());
#endif

  while (!_exitEngine) {

    while (Config::EngineInternals.PAUSE) {
      iterateQ(CYCLE::PAUSE);
      _windowManager->setWindowTitle("paused ...");
    }

    bool render = false;
    double startTime = TimeUtils::getNanoseconds();
    double passedTime = startTime - lastTime;
    lastTime = startTime;

    delta += passedTime / (double)_NANOSECOND;
    frameCounter += passedTime;

    while (delta > _FRAMETIME) {
      render = true;
      delta -= _FRAMETIME;

      if (_windowManager->windowShouldClose()) {
        _exitEngine = true;
        break;
      }

      if (frameCounter >= _NANOSECOND) {
        setFps(frames);
        getString()->format("%s : %d", CoreConstants::TITLE, getFps());
        _windowManager->setWindowTitle(getString()->getBuffer());

        frames = 0;
        frameCounter = 0;
      }
    }

    if (render) {
      success = iterateQ(CYCLE::UPDATE);
      frames++;
    }
  }
}

std::int32_t EngineManager::init() {
#ifdef DEBUG
  double start = TimeUtils::getCurrentTimeAsTime();
#endif

  std::int32_t success = iterateQ(CYCLE::INIT);

#ifdef DEBUG
  double end = TimeUtils::getCurrentTimeAsTime();
  double res = TimeUtils::getDuration(start, end);
  getString()->format("%s ### All Managers initialized! init time: %f ms.\n",
                      getName(), res);
  logDebug(getString()->getBuffer());
#endif
  return success;
}

std::int32_t EngineManager::iterateQ(CYCLE code) {
  assert(typeid(code) == typeid(EngineManager::CYCLE));
  assert(_engineQ->getSize() == _engineQ->getCapacity());
  std::int32_t success = 0;

  for (std::int32_t i = 0; i < _engineQ->getSize(); i++) {
#ifdef DEBUG
    double start = TimeUtils::getCurrentTimeAsTime();
#endif
    IManager *item = _engineQ->dequeue();
    assert(item != nullptr);

    if (isPauseCycle(code, item)) {
      _engineQ->enqueue(item);
      continue;
    }

    success = executeCycle(code, item);

    if (success != 0) {
#ifdef DEBUG
      getString()->format("%s ### Failure in %s for cycle: %i \n", getName(),
                          item->getName(), code);
      logFailure(getString()->getBuffer());
#endif
      break;
    }

    _engineQ->enqueue(item);

#ifdef DEBUG
    if (code == CYCLE::UPDATE) {
      double end = TimeUtils::getCurrentTimeAsTime();
      double res = TimeUtils::getDuration(start, end);
      getString()->format("%s ### Queue time: %f ms. for: %s \n", getName(),
                          res, item->getName());
      logDebug(getString()->getBuffer());
    }
#endif
  }

  return success;
}

std::int32_t EngineManager::executeCycle(CYCLE code, IManager *item) {
  switch (code) {
  case CYCLE::STARTUP:
    return item->startup();
  case CYCLE::INIT:
    return item->init();
  case CYCLE::UPDATE:
  case CYCLE::PAUSE:
    item->update();
    return 0;
  default:
    getString()->format("%s ### unknown territory ... code: %d\n", getName(),
                        code);
    logFailure(getString()->getBuffer());
    return 1;
  }
}

bool EngineManager::isPauseCycle(CYCLE code, IManager *item) {
  bool pauseExecute = code == CYCLE::PAUSE;
  bool isManager = strcmp(item->getName(), CoreConstants::INPUT_MANAGER) == 0 ||
                   strcmp(item->getName(), CoreConstants::WINDOW_MANAGER) == 0;

  return pauseExecute && !isManager;
}
