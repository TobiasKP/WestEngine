#pragma once

#include "Components/Umbrella.h"
#include "Entity/Scene.h"
#include "Interfaces/IManager.h"
#ifdef DEBUG
#include "Utils/Draw/DebugDrawUtils.h"
#endif

#include <WestInterfaceFacade.h>

class RenderManager : public IManager
{
public:
  RenderManager();
  RenderManager(WestLogger* logger);
  ~RenderManager() override;

  // Overrides
  std::int32_t startup() override;
  void shutdown() override;
  void update() override;
  std::int32_t init() override;

  // Functions
  static GLuint getUsedShaderProgram()
  {
    return _usedShaderProgram;
  }

private:
  static GLuint _usedShaderProgram;
  WestInterface::WestInterfaceFacade* _facade;
  Scene* _scene;

  void clearColor();
  void renderUserInterfaces();
  void renderGameEntities();
  void renderWorld();
  void renderMainLoop(const Entity& e);
  void updateUniforms(const Entity& e, Model* m, Material* m2);
  bool AABBcheck(const Entity& e);

#ifdef DEBUG
  DebugDrawUtils* _debugUtils = nullptr;
#endif
};
