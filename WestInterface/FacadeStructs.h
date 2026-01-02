#pragma once

#include <cstdint>
#include <functional>
#include <string>

namespace WestInterface
{

enum ElementType { LABEL, BUTTON, CONTAINER, ICON, DEBUG_ELEMENT, DROPDOWN };

struct TextureInformation
{
  bool mipmap;
  std::int32_t wrapping_x;
  std::int32_t wrapping_y;
  const char* path;
};

struct ElementProxy
{
  ElementType type;
  std::uint32_t elementId;

  std::function<void()> eventHandler = nullptr;

  float colorR;
  float colorG;
  float colorB;
  float colorA;

  std::int8_t zIndex = -1;
  float xPosition;
  float yPosition;
  float stretchX = 1.0f;
  float stretchY = 1.0f;

  std::uint8_t rowElements    = 1;
  std::uint8_t columnElements = 1;
  std::uint8_t row;
  std::uint8_t column;
  std::uint32_t givenFlags = 0x00;

  std::string text = "";

  TextureInformation* texture = nullptr;
};

struct Container : ElementProxy
{
  std::uint16_t xScreenPosition;
  std::uint16_t yScreenPosition;
  float stretchX;
  float stretchY;
  std::uint16_t rows;
  std::uint16_t columns;
  bool hiddenContainer;
  std::vector<ElementProxy*>& elements;
  TextureInformation* background = nullptr;

  Container(std::vector<ElementProxy*>& vec) : elements(vec) {};
};
};  // namespace WestInterface
