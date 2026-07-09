#include "../CoreHeaders/RenderManager.h"

#include <Config.h>
#include <format>
#include <glm/ext/matrix_clip_space.hpp>
#include <WestAssetFacade.hpp>

RenderManager::RenderManager() : IManager(nullptr)
{
  setName(CoreConstants::RENDER_MANAGER);
  _facade = nullptr;
  _scene  = nullptr;
}

RenderManager::RenderManager(WestLogger* logger, const std::shared_ptr<Scene>& s) : IManager(logger)
{
  setName(CoreConstants::RENDER_MANAGER);
  _facade = nullptr;
  _scene  = s;
}

RenderManager::~RenderManager() {}

std::int32_t RenderManager::startup()
{
  return 0;
}

void RenderManager::shutdown()
{
#ifdef DEBUG
  logDebug(std::format("{} ### Shutting down {}...\n", getName(), getName()));
#endif
}

std::int32_t RenderManager::init()
{
#ifdef DEBUG
  double start = TimeUtils::getCurrentTimeAsTime();
#endif

  _facade = &WestRendererFacade::getRendererFacade();
  assert(_scene != nullptr && _facade != nullptr);

#ifdef DEBUG
  double end = TimeUtils::getCurrentTimeAsTime();
  double res = TimeUtils::getDuration(start, end);
  logDebug(std::format("{} ### RenderManager init time: {} ms.\n", getName(), res));
#endif

  return 0;
}

void RenderManager::update()
{
#ifdef DEBUG
  double start = TimeUtils::getCurrentTimeAsTime();
#endif
  _facade->clearColor();
  _facade->renderWorld();
  _facade->renderEntity();
  _facade->renderInterface();
  _facade->renderDebugEntities();
#ifdef DEBUG
  double end = TimeUtils::getCurrentTimeAsTime();
  double res = TimeUtils::getDuration(start, end);
  logCycle(std::format("{} ### render time for all entites in scene: {} ms.\n", getName(), res));
#endif
}
