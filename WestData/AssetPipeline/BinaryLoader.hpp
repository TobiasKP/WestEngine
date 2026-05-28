#pragma once

#include "../Data/Mesh.hpp"
#include "Converter.hpp"
#include "AssetImporter.hpp"

#include <WestLogger.h>

class BinaryLoader
{
public:
  BinaryLoader(WestLogger* l);
  ~BinaryLoader();

  void init();
  void shutdown();

  const bool exists(const std::string path);
  const Mesh& load(const std::string path);

private:
  std::shared_ptr<AssetImporter> _importer;
  std::unique_ptr<Converter> _converter;
  WestLogger* _logger;
};
