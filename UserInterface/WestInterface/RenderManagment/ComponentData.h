#pragma once

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
  // Bottom Left corner & scale
  float xLL;
  float yLL;
  float scale;
  //float uvTopLeft;
  //float uvBottomRight;
  // Color of component:
  float colorR;
  float colorG;
  float colorB;
  float colorA;
};
