#pragma once

#include "../Data/Mesh.hpp"

#include <WestLogger.h>

class BinaryLoader
{
public:
  BinaryLoader(WestLogger* l);
  ~BinaryLoader();

  void init();
  void shutdown();

  const Mesh& load(const std::string path);

private:
  WestLogger* _logger;
};
