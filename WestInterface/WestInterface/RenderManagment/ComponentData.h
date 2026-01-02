#pragma once

#include <cstdint>

struct ComponentData
{
  float vertices[2];
  float stretchX;
  float stretchY;
  float textureCoords[4] = {0.0f, 0.0f, 1.0f, 1.0f};

  float colorR;
  float colorG;
  float colorB;
  float colorA;


  std::uint32_t flags;
  std::uint32_t texture = 0;
  std::int8_t zIndex    = 0;
};

struct ElementBounds
{
  float xLeft, xRight, yBottom, yTop;
  std::uint32_t id;
  std::int8_t zIndex;
  bool eventDriven = false;
};

/**********************************************************************************
 * FLAG DESCRIPTION
 * 0x00000001 = Border
 * 0x00000002 = Mouse Hovered
 * 0x00000004 = hasTexture
 * 0x00000008 = hasText
 * 0x00000010 = Border and discarded center Pixels
 * 0x00000020 = blending texture borders
 * 0x00000040 = highlight Color Burgundy
 * 0x00000080 = hightlight Color Gold
 * 0x00000100 = highlicht Color Emerald
 * 0x00000200 = ---
 * 0x00000400 = ---
 * 0x00000800 = ---
 * 0x00001000 = ---
 * 0x00002000 = ---
 * 0x00004000 = ---
 * 0x00008000 = ---
 * 0x00010000 = ---
 * 0x00020000 = ---
 * 0x00040000 = ---
 * 0x00080000 = ---
 * 0x00100000 = ---
 * 0x00200000 = ---
 * 0x00400000 = ---
 * 0x00800000 = ---
 * 0x01000000 = ---
 * 0x02000000 = ---
 * 0x04000000 = ---
 * 0x08000000 = ---
 * 0x10000000 = ---
 * 0x20000000 = ---
 * 0x40000000 = ---
 * 0x80000000 = ---
 **********************************************************************************/
