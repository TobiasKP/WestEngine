#pragma once

#include "Label.hpp"

struct ProgressBar : public IElement
{
  Label* progress          = nullptr;
  std::uint8_t progresPerc = 0;


  ProgressBar()
  {
    zIndex = 10;
  }

  ~ProgressBar()
  {
    delete progress;
  }

  void handler() override {

  };

  void describeMyself(ComponentData* cd, std::uint8_t row = 0, std::uint8_t column = 0) override
  {
    assert(progress != nullptr);
    if (column < columnElements * progresPerc / 100)
    {
      progress->describeMyself(cd, row, column);
    }
  };
};
