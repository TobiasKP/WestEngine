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
  float textureCoords[8];

  float colorR;
  float colorG;
  float colorB;
  float colorA;
};

/*****************************************
 * FLAG DESCRIPTION
 * 0x01 = Border and no filling pixels -> discarded
 *
 *
 *
 */
