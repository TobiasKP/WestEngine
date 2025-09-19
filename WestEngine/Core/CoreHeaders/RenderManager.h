#pragma once

#include "Interfaces/IManager.h"

#include <WestInterfaceFacade.h>

#include "Components/Umbrella.h"
#include "Entity/Scene.h"

class RenderManager : public IManager {
public:
  RenderManager();
  RenderManager(WestLogger *logger);
  ~RenderManager() override;

  // Overrides
  std::int32_t startup() override;
  void shutdown() override;
  void update() override;
  std::int32_t init() override;

  // Functions
  static GLuint getUsedShaderProgram() { return _usedShaderProgram; }

private:
  static GLuint _usedShaderProgram;
  WestInterfaceFacade *_facade; 
  GLuint _interfaceVBO, _interfaceVAO, _interfaceEBO, _interfaceCOL;
  Scene *_scene;

  void clearColor();
  void renderUserInterfaces();
  void renderGameEntities();
  void updateUniforms(Entity *e, Model *m);
};
