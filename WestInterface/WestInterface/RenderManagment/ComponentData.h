#pragma once

#include <cstdint>

#if defined(_WIN32) || defined(_WIN64)
#ifdef WEST_INTERFACE_EXPORTS
#define WEST_INTERFACE __declspec(dllexport)
#else
#define WEST_INTERFACE __declspec(dllimport)
#endif
#else
#define WEST_INTERFACE __attribute__((visibility("default")))
#endif

struct WEST_INTERFACE ComponentData {
  std::uint32_t flags;
  float vertices[2];
  float textureCoords[4] = {0.0f, 0.0f, 1.0f, 1.0f};
  std::uint32_t texture = 0;
  float stretchX;
  float stretchY;
  std::int8_t zIndex = 0;

  float colorR;
  float colorG;
  float colorB;
  float colorA;
};

struct WEST_INTERFACE ElementBounds {
  std::uint32_t id;
  std::int8_t zIndex;
  float xLeft, xRight, yBottom, yTop;
  bool eventDriven = false;
};

/**********************************************************************************
 * FLAG DESCRIPTION
 * 0x0001 = Border and no filling pixels -> discarded
 * 0x0002 = Mouse Hovered
 * 0x0004 = hasTexture
 * 0x0008 = hasText
 * 0x0010 = ---
 * 0x0020 = blending texture borders
 * 0x0040 = ---
 **********************************************************************************/
