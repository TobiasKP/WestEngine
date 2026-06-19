#pragma once

#include "Converter.hpp"

#include <WestLogger.h>

class BinaryLoader
{
public:
  BinaryLoader(WestLogger* l);
  ~BinaryLoader();

  void init();
  void shutdown();

  bool exists(const std::string& path);
  std::optional<Model> load(const std::string& path);

private:
  std::optional<Model> loadFromDisk(const std::string& path);
  std::optional<Model> convertToModel(std::ifstream& is, const std::string& guid);

  std::unique_ptr<Converter> _converter;
  WestLogger* _logger;
};
