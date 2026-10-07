#pragma once

#ifdef DEBUG

#include "../Components/ComponentRegistry.hpp"
#include "../Entity/Scene.h"

#include <WestAssetFacade.hpp>
#include <WestRendererFacade.hpp>
#include <vector>

class DebugDrawSystem
{
public:
  DebugDrawSystem(std::shared_ptr<Scene> scene, WestRenderer::WestRendererFacade* renderer);

  void init();
  void update();

private:
  std::uint32_t createDebugLine(const std::vector<glm::vec3>& points);
  void removeDebugEntity(std::uint32_t entityId);

  std::shared_ptr<Scene> _scene;
  WestRenderer::WestRendererFacade* _renderer;
  GLuint _debugShaderId = -1;
  std::vector<std::uint32_t> _projectileDebugEntities;
};

#endif
