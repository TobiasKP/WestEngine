#pragma once

#include "Interfaces/IManager.h"

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
  GLuint _quadVAO, _POS, _texture;
  Scene *_scene;

  const std::int32_t _quadIndices[6] = {0, 1, 2, 0, 2, 3};
  const float _quadVertices[12] = {0.0f,  50.0f,  // UL
                                   0.0f,  0.0f,   // OL
                                   50.0f, 0.0f,   // OR
                                   50.0f, 50.0f}; // UR

  void initInterfaceBuffer();
  void clearColor();
  void renderUserInterfaces();
  void renderGameEntities();
  void updateUniforms(Entity *e, Model *m);
};
