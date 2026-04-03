#pragma once

#include "../Data/Model.hpp"
#include "AssetQueue.hpp"

#include <memory>
#include <WestLogger.h>

class Converter
{
public:
  Converter(const std::shared_ptr<AssetQueue> q, WestLogger* l);
  ~Converter();

  void init();
  void shutdown();
  void pop();

private:
  void bake(const Model& m);

  std::shared_ptr<AssetQueue> _queue;
  WestLogger* _logger;
};
