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
  void bake(Model& m);

  template <typename T>
  void write(std::ofstream& file, const T& value)
  {
    file.write(reinterpret_cast<const char*>(&value), sizeof(T));
  };

  std::unique_ptr<AssetImporter> _importer;
  WestLogger* _logger;
};
