#pragma once

#include "Components/Umbrella.h"
#include "Entity/Scene.h"
#include "Interfaces/IManager.h"
#include "Systems/Umbrella.h"

#include <map>
#include <WestInterfaceFacade.h>
#include <WestRendererFacade.hpp>

class ShaderManager : public IManager
{
public:
  ShaderManager();
  ShaderManager(WestLogger* logger, const std::shared_ptr<Scene>& s);
  ~ShaderManager() override;

  // Overrides
  std::int32_t startup() override;
  void shutdown() override;
  void update() override;
  std::int32_t init() override;
  GLuint getShaderByGroup(std::int32_t group)
  {
    return _programList.at(group);
  }

private:
  std::map<std::int32_t, GLuint> _programList;
  std::int32_t _lastEntityCount;
  std::shared_ptr<Scene> _scene;
  WestInterface::WestInterfaceFacade* _facade;
  WestRenderer::WestRendererFacade* _rendererFacade;

  GLuint initInterfaceShader();
  void initWorldShader();
  void initEntityShader(Entity& entity);

  // Functions
  void addUniforms(GLuint programId, const Entity& entity);
};
