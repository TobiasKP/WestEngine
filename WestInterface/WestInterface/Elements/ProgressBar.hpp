#pragma once

#include "Label.hpp"

struct ProgressBar : public Label
{
  Label* progress          = nullptr;
  std::uint8_t progresPerc = 0;


  ProgressBar()
  {
    zIndex  = 10;
    flags  |= 0x0010;
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
      progress->xLL = this->xLL;
      progress->yLL = this->yLL;
      progress->describeMyself(cd, row, column);
    }
    else
    {
      Label::describeMyself(cd, row, column);
    }
  };
};
