// TODO REMOVE -> to interface

#pragma once

#include "../../Entity/Entity.h"
#include "../DataUtils/ObjectLoader.h"

#include <cstdint>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <unordered_map>

class TextDrawUtils
{
public:
  TextDrawUtils();

  // Getter
  inline Entity* getTextEntity()
  {
    return _textGlyphs;
  }

  // Functions
  void createGlyph(char c);
  void setTextShaderProgram();
  void resetTextShaderProgram();
  void renderText(size_t length, glm::vec2 screenPosition);

private:
  std::int32_t _bitmapWidth = 512, _bitmapHeight = 512, _charWidth = 32, _charHeight = 32, _columns = 16, _rows = 16;
  std::vector<std::vector<GLfloat>> _bitmapCoords;

  Entity* _textGlyphs;
  ObjectLoader* _loader;
  // Make static for shared cache
  std::unordered_map<std::int32_t, std::vector<std::vector<GLfloat>>> _textCache;
};
