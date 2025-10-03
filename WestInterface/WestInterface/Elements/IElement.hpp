#pragma once

#include <WestLogger.h>
#include <cstdint>

#include "../RenderManagment/ComponentData.h"

static constexpr std::uint8_t SIZE_E = 100;
static constexpr std::uint16_t hasTexture = 0x01;
static constexpr std::uint16_t isInteractive = 0x02;
static constexpr std::uint16_t hasText = 0x04;
static constexpr std::uint16_t isHidden = 0x08;

// For Textrendering form Bitmap in Resources.
struct Text {};

struct IElement { 
  ~IElement() { delete _cd; };
  // Public identifier
  std::uint32_t id;

  // zIndex. gives the element "stack"
  std::uint8_t zIndex;

  // Sizing params: Each element has a basic size of
  // 10*10px with xLL & yLL as the most lower left corner of the element.
  // Size will be adjusted by different scaling given element.
  float xLL;
  float yLL;
  float scale;

  // Color of each Element with transparancy.
  float colorR;
  float colorG;
  float colorB;
  float colorA;

  // Bit flags for special behaviours
  std::uint8_t flags;

  virtual void handler() {};

  ComponentData *describe() {
    describeMyself(_cd);
    return _cd;
  };

protected:
  WestLogger &_logger = WestLogger::getLoggerInstance();

private:
  ComponentData *_cd = new ComponentData();
  virtual void describeMyself(ComponentData *cd) {};
};
