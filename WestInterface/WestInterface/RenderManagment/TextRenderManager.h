#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <WestLogger.h>
#include <cstdint>
#include <glm/glm.hpp>
#include <unordered_map>
#include <vector>

class TextRenderManager {
public:
  struct GlyphData {
    std::vector<GLfloat> textureCoords;
  };

  TextRenderManager();
  ~TextRenderManager();

  void initializeFontAtlas();
  void precomputeGlyphData();

  void renderText(const std::string &text, glm::vec2 screenPosition,
                  float scale = 1.0f);
  void setTextShaderProgram();
  void resetTextShaderProgram();

  static const std::unordered_map<char, GlyphData> &getGlyphCache() {
    return _glyphCache;
  }

private:
  void calculateGlyphCoordinates(char character, GlyphData &glyphData);

  static constexpr std::int32_t BITMAP_WIDTH = 512;
  static constexpr std::int32_t BITMAP_HEIGHT = 512;
  static constexpr std::int32_t CHAR_W = 32;
  static constexpr std::int32_t CHAR_H = 32;
  static constexpr std::int32_t COLUMNS = 16;
  static constexpr std::int32_t ROWS = 16;

  static std::unordered_map<char, GlyphData> _glyphCache;

  WestLogger &_logger = WestLogger::getLoggerInstance();
};
