#pragma once

#include "Interfaces/IManager.h"

#include <map>

#include "Entity/Scene.h"
#include "Systems/Umbrella.h"

class ShaderManager : public IManager {
public:
  ShaderManager();
  ShaderManager(WestLogger *logger);
  ~ShaderManager() override;

  // Overrides
  std::int32_t startup() override;
  void shutdown() override;
  void update() override;
  std::int32_t init() override;
  GLuint getShaderByGroup(std::int32_t group) { return _programList.at(group); }

private:
  std::map<std::int32_t, GLuint> _programList;
  std::int32_t _lastEntityCount;
  Scene *_scene;

  void initInterfaceShader();
  GLuint initShader(Shader *s, Entity *entity);
  GLuint createShader(const std::string shaderFile, std::int32_t shaderTyp,
                      GLuint programId);
  GLchar *readShaderSource(const std::string shaderFile);
  void link(GLuint programId, GLuint vertexId, GLuint fragmentId);

  // Functions
  GLuint createVertexShader(const std::string file, GLuint programId) {
    return createShader(file, GL_VERTEX_SHADER, programId);
  }
  GLuint createFragmentShader(const std::string file, GLuint programId) {
    return createShader(file, GL_FRAGMENT_SHADER, programId);
  }
  void addUniforms(GLuint programId, Entity *entity, std::int32_t shadergroup);
};
