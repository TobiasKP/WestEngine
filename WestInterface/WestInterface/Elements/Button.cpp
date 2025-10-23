
#pragma once

#include "Label.cpp"

struct Button : public Label {
  std::uint8_t eventId;

  void handler() {
    _logger.log(
        Level::Error,
        std::format(
            "@@@ Button handler of interface: {} called which does not exists!",
            this->id));
  };

  void describeMyself(ComponentData *cd, std::uint8_t row,
                      std::uint8_t column) {
    Label::describeMyself(cd, row, column); 
  };
};
