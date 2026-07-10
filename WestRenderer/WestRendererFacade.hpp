#pragma once

#include <GL/glew.h>
#include <string>
#include <unordered_map>

class WestRendererFacade
{
public:
  static WestRendererFacade& getRendererFacade();

  void clearColor() {};
  void renderEntity() {};
  void renderInterface() {};
  void renderWorld() {};
  void renderDebugEntities() {};

  GLuint registerShader(const std::string& vshader, const std::string& fshader) {};

private:
  std::unordered_map<std::string, GLuint> _vshaderToProgramId;
  std::unordered_map<std::string, GLuint> _fshaderToProgramId;
  std::unordered_map<std::string, GLuint> _modelGuidToProgramId;
};
