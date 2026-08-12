#pragma once

#include "AssetImporter.hpp"

#include <WestLogger.h>

class Converter
{
public:
  Converter(WestLogger* l, std::shared_ptr<AssetImporter> importer);
  ~Converter();

  void init();
  void shutdown();
  void convertQueueElements();
  void addOnRequest(const std::string& path);

private:
  void bake(Model& m);

  template <typename T>
  void write(std::ofstream& file, const T& value)
  {
    file.write(reinterpret_cast<const char*>(&value), sizeof(T));
  };

  std::shared_ptr<AssetImporter> _importer;
  WestLogger* _logger;
};
