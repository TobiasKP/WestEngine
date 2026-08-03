#pragma once

#include "Components/Umbrella.h"
#include "Entity/Scene.h"
#include "Interfaces/IManager.h"

#include <WestRendererFacade.hpp>

#ifdef DEBUG
#include "Systems/DebugDrawSystem.h"
#endif

class RenderManager : public IManager
{
public:
  RenderManager();
  RenderManager(WestLogger* logger, const std::shared_ptr<Scene>& s);
  ~RenderManager() override;

  // Overrides
  std::int32_t startup() override;
  void shutdown() override;
  void update() override;
  std::int32_t init() override;

private:
  bool AABBcheck(const Entity& e);

  WestRenderer::WestRendererFacade* _facade;
  std::shared_ptr<Scene> _scene;

#ifdef DEBUG
  void renderDebugEntities();
  std::unique_ptr<DebugDrawSystem> _debugDrawSystem;
#endif
};
