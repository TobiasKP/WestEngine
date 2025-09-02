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

#include "WestInterface/RenderManagment/ComponentData.h"
#include "WestInterface/RenderManagment/DrawData.h"

#include <cstdint>
#include <functional>
#include <string>

enum ElementType { LABEL, BUTTON, CONTAINER, ICON, DEBUG_ELEMENT };

struct WEST_INTERFACE ElementProxy {
  ElementType type;
  std::uint32_t elementId;

  // std::uint8_t eventId;
  // std::function<void()> eventHandler;

  float colorR;
  float colorG;
  float colorB;
  float colorA;

  float xPosition;
  float yPosition;
  float scale;

  std::uint8_t row;
  std::uint8_t column;

  /*
    std::string value;

    std::string text;
    std::uint32_t textureId;

    std::uint32_t iconId;
    std::uint8_t padding;*/
};

struct WEST_INTERFACE RenderData {
  static DrawData d_instance;
  static ComponentData c_data;
};
