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
  float stretchX;
  float stretchY;

  float colorR;
  float colorG;
  float colorB;
  float colorA;
};

struct WEST_INTERFACE ElementBounds {
  std::uint32_t id;
  float xLeft, xRight, yBottom, yTop;
};

/*****************************************
 * FLAG DESCRIPTION
 * 0x01 = Border and no filling pixels -> discarded
 * 0x02 = Mouse Hovered
 *
 *
 */
