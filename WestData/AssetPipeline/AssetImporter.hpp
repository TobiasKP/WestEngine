#pragma once

#include "../Data/Mesh.hpp"
#include "AssetPathScreener.hpp"
#include "AssetQueue.hpp"

#include <memory>
#include <WestLogger.h>

class AssetImporter
{
public:
  AssetImporter(WestLogger* l);
  ~AssetImporter();

  void init();
  void shutdown();
  void handlePath(const std::string& path);

private:
  std::string generateUUID(const std::string& path);
  Mesh& handleFile(const std::string& path);
  void push(const Mesh& m);

  WestLogger* _logger;
  std::shared_ptr<AssetQueue> _queue;
  std::unique_ptr<AssetPathScreener> _screener;
};
