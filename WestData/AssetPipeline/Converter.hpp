#pragma once

#include "AssetImporter.hpp"

#include <WestLogger.h>

class Converter
{
public:
  Converter(WestLogger* l);
  ~Converter();

  void init();
  void shutdown();
  void convertQueueElements();

private:
  void bake(const Mesh& m);

  std::unique_ptr<AssetImporter> _importer;
  WestLogger* _logger;
};
