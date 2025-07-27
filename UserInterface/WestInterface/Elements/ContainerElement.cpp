#pragma once

#include <vector>
#include <algorithm>

#include "IElement.h"

struct ContainerElement : public IElement {
  std::vector<IElement *> children;
  bool hidden;
  std::uint8_t gridLayout;

  bool deleteChildById(std::uint32_t d_id) {
    auto result =
        std::find_if(children.begin(), children.end(),
                     [&d_id](IElement* obj) { return obj->id == d_id; });
    if (result == children.end())
      return false;

    // TODO if it is a container all children have to be deleted too
    children.erase(result);
    return true;
  }
  void addChild(IElement *e) { children.push_back(e); };
};
