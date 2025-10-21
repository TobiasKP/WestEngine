#pragma once

#include <WestLogger.h>
#include <cstdint>

#include "../RenderManagment/ComponentData.h"
#include "../RenderManagment/ComponentDataPool.h"

static constexpr std::uint8_t SIZE_E = 10;

// For Textrendering form Bitmap in Resources.
struct Text {
  std::string plaintext;
  std::vector<float> coordinates;
};

struct IElement {
  IElement() { text = nullptr; }
  ~IElement() {};

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
  std::uint64_t flags;

  // Number of quads being next to each other
  std::uint16_t rowElements = 0;
  std::uint16_t columnElements = 0;

  std::unique_ptr<Text> text;

  bool changed = true;
  std::uint32_t poolPosition = 0;
  ComponentDataPool *dataPool = nullptr;

  virtual void handler() {};

  ComponentData *describe(std::uint8_t row = 0, std::uint8_t column = 0) {
    ComponentData *data =
        dataPool->getDataAtLocation(poolPosition + row + column);

    if (!changed)
      return data;

    describeMyself(data, row, column); 
    return data;
  };

protected:
  WestLogger &_logger = WestLogger::getLoggerInstance();

private:
  virtual void describeMyself(ComponentData *cd, std::uint8_t row = 0,
                              std::uint8_t column = 0) {};
};
