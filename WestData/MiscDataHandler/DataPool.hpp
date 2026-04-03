#pragma once

#include "../Constants/Limits.hpp"
#include "../Data/Model.hpp"

#include <array>

class DataPool
{
public:
  DataPool();
  ~DataPool();

  void init();
  void shutdown();
  const Model& getNextFree(const std::string& type);

private:
  std::array<Model, Limit::projectilePoolsize> _projectiles;
  std::array<Model, 1024> _placeholder;
};
