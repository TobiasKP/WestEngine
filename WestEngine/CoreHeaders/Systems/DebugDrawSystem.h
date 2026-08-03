#pragma once

#ifdef DEBUG

#include "../Components/ComponentRegistry.hpp"
#include "../Entity/Scene.h"

#include <WestAssetFacade.hpp>
#include <WestRendererFacade.hpp>

class DebugDrawSystem
{
public:
  DebugDrawSystem(std::shared_ptr<Scene> scene, WestRenderer::WestRendererFacade* renderer);

  void init();
  void update();

private:
  std::uint32_t createDebugLine(glm::vec3 start, glm::vec3 end);
  void removeDebugEntity(std::uint32_t entityId);

  std::shared_ptr<Scene> _scene;
  WestRenderer::WestRendererFacade* _renderer;
  GLuint _debugShaderId = -1;
  std::vector<std::uint32_t> _projectileDebugEntities;
};

#endif
