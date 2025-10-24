#include "TextRenderManager.h"

#include <format>

std::unordered_map<char, TextRenderManager::GlyphData>
    TextRenderManager::_glyphCache;

TextRenderManager::TextRenderManager() {}

TextRenderManager::~TextRenderManager() {}

void TextRenderManager::initializeFontAtlas() {
  _logger.log(Level::Info, "@@@ Initializing font atlas\n");
  precomputeGlyphData();
}

void TextRenderManager::precomputeGlyphData() {
  for (char c = 32; c <= 126; ++c) {
    GlyphData glyphData;
    calculateGlyphCoordinates(c, glyphData);
    _glyphCache[c] = glyphData;
#ifdef DEBUG
    _logger.log(Level::Info,
                std::format("\t: {} -> {}\n", c, glyphData.textureCoords));
#endif
  }
}

void TextRenderManager::calculateGlyphCoordinates(char character,
                                                  GlyphData &glyphData) {
  std::int32_t charIndex = static_cast<std::int32_t>(character);
  std::int32_t col = charIndex % COLUMNS;
  std::int32_t row = charIndex / ROWS - 1;

  // Normalize texture coordinates
  float left = static_cast<float>(col * CHAR_W) / BITMAP_WIDTH;
  float right = static_cast<float>((col + 1) * CHAR_W) / BITMAP_WIDTH;
  float top = static_cast<float>((row - 1) * CHAR_H) / BITMAP_HEIGHT;
  float bottom = static_cast<float>(row * CHAR_H) / BITMAP_HEIGHT;

  glyphData.textureCoords = {
      left,
      bottom,
      right,
      top,
  };
}
