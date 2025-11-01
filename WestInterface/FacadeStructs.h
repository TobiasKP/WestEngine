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

#include <cstdint>
#include <functional>
#include <string>

enum ElementType { LABEL, BUTTON, CONTAINER, ICON, DEBUG_ELEMENT };

struct WEST_INTERFACE TextureInformation {
  bool mipmap;
  std::int32_t wrapping;
  std::string path;
};

struct WEST_INTERFACE ElementProxy {
  ElementType type;
  std::uint32_t elementId;

  std::function<void()> eventHandler = nullptr;

  float colorR;
  float colorG;
  float colorB;
  float colorA;

  float xPosition;
  float yPosition;
  float stretchX = 1.0f;
  float stretchY = 1.0f;

  std::uint8_t rowElements = 1;
  std::uint8_t columnElements = 1;
  std::uint8_t row;
  std::uint8_t column;

  std::string text = "";

  TextureInformation *texture = nullptr;
};
