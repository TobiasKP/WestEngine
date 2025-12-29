#include "TextRenderManager.h"

#include <format>

std::unordered_map<char, TextRenderManager::GlyphData> TextRenderManager::_glyphCache;

TextRenderManager::TextRenderManager() {}

TextRenderManager::~TextRenderManager() {}

void TextRenderManager::initializeFontAtlas()
{
  _logger.log(Level::Info, "@@@ Initializing font atlas\n");
  precomputeGlyphData();
}

void TextRenderManager::precomputeGlyphData()
{
  for (char c = 32; c <= 126; ++c)
  {
    GlyphData glyphData;
    calculateGlyphCoordinates(c, glyphData);
    _glyphCache[c] = glyphData;
#ifdef DEBUG
    _logger.log(Level::Info, std::format("\t: {} -> {}\n", c, glyphData.textureCoords));
#endif
  }
}

void TextRenderManager::calculateGlyphCoordinates(char character, GlyphData& glyphData)
{
  std::int32_t charIndex = static_cast<std::int32_t>(character);
  std::int32_t col       = charIndex % COLUMNS;
  std::int32_t row       = charIndex / ROWS - 1;

  // Normalize texture coordinates
  float left   = static_cast<float>(col * CHAR_W) / BITMAP_WIDTH;
  float right  = static_cast<float>((col + 1) * CHAR_W) / BITMAP_WIDTH;
  float top    = static_cast<float>((row - 1) * CHAR_H) / BITMAP_HEIGHT;
  float bottom = static_cast<float>(row * CHAR_H) / BITMAP_HEIGHT;

  glyphData.textureCoords = {
    left,
    bottom,
    right,
    top,
  };

  glyphData.width    = CHAR_W;
  glyphData.height   = CHAR_H;
  glyphData.bearingY = CHAR_H * 0.8f;

  switch (character)
  {
    case 'i':
    case 'l':
    case 't':
    case 'I':
    case 'f':
    case '!':
    case '.':
    case ',':
    case ':':
    case ';':
      glyphData.advance  = CHAR_W * 0.35f;
      glyphData.bearingX = CHAR_W * 0.1f;
      break;

    case ' ':
      glyphData.advance  = CHAR_W * 0.5f;
      glyphData.bearingX = 0.0f;
      break;

    default:
      glyphData.advance  = CHAR_W * 0.5f;
      glyphData.bearingX = CHAR_W * 0.2f;
      break;
  }
}

std::vector<float> TextRenderManager::calculateTextPositions(const std::string& text, float scale)
{
  std::vector<float> positions;
  positions.reserve(text.size());
  float currentX = 0.0f;
  for (char c : text)
  {
    auto it = _glyphCache.find(c);
    if (it != _glyphCache.end())
    {
      const GlyphData& glyph = it->second;
      positions.push_back(currentX + (glyph.bearingX * scale));
      currentX += glyph.advance * scale;
    }
    else
    {
      positions.push_back(currentX);
      currentX += CHAR_W * 0.7f * scale;
    }
  }

  return positions;
}

float TextRenderManager::calculateTextWidth(const std::string& text, float scale)
{
  if (text.empty())
  {
    return 0.0f;
  }

  float totalWidth = 0.0f;
  for (char c : text)
  {
    auto it = _glyphCache.find(c);
    if (it != _glyphCache.end())
    {
      totalWidth += (it->second.advance * scale) + 2.0;
    }
    else
    {
      totalWidth += CHAR_W * 0.7f * scale;
    }
  }

  return totalWidth;
}
