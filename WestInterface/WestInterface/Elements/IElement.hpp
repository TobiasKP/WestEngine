#pragma once

#include "../RenderManagment/ComponentData.h"
#include "../RenderManagment/ComponentDataPool.h"
#include "../RenderManagment/TextRenderManager.h"

#include <cstdint>
#include <functional>
#include <WestLogger.h>

static constexpr std::uint8_t SIZE_E = 40;

// For Textrendering form Bitmap in Resources.
struct Text
{
  std::string plaintext;
  std::vector<float> coordinates;
  std::vector<float> positions;
};

struct IElement
{
  IElement()
  {
    text = nullptr;
  }
  ~IElement()
  {
    if (texture != 0)
    {
      glDeleteTextures(1, &texture);
    }
  };

  float xLL;
  float yLL;
  float stretchX;
  float stretchY;

  float colorR = 0.0f;
  float colorG = 0.0f;
  float colorB = 0.0f;
  float colorA = 1.0f;

  std::uint64_t flags = 0x00;
  std::int8_t zIndex  = -1;
  bool changed        = true;
  bool supportsEvents = false;

  std::uint16_t rowElements    = 0;
  std::uint16_t columnElements = 0;

  GLuint texture = 0;

  std::uint32_t id;

  IElement* parent            = nullptr;
  std::uint32_t poolPosition  = 0;
  ComponentDataPool* dataPool = nullptr;
  std::unique_ptr<Text> text;
  std::function<void()> eventHandler = nullptr;

  virtual void handler() {};

  virtual void getBoundaries(ElementBounds& bounds) {};

  ComponentData* describe(std::uint8_t row = 0, std::uint8_t column = 0)
  {
    ComponentData* data = dataPool->getDataAtLocation(poolPosition + (row * columnElements) + column);

    if (!changed)
    {
      return data;
    }

    describeMyself(data, row, column);
    return data;
  };

  float getElementWidth()
  {
    float elementWidth = 0.0f;
    if (this->text != nullptr && !this->text->plaintext.empty())
    {
      elementWidth = TextRenderManager::calculateTextWidth(this->text->plaintext, this->stretchX);
      if (this->text->plaintext.size() == 1)
      {
        elementWidth += 15;
      }
    }
    else
    {
      elementWidth = this->columnElements * SIZE_E * this->stretchX;
    }
    return elementWidth;
  }

protected:
  WestLogger& _logger = WestLogger::getLoggerInstance();

private:
  virtual void describeMyself(ComponentData* cd, std::uint8_t row = 0, std::uint8_t column = 0) {};
};
