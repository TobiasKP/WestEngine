#pragma once

#include "../Data/Mesh.hpp"
#include "AssetPathScreener.hpp"

#include <memory>
#include <ThreadSafeQueue.hpp>
#include <WestLogger.h>

class AssetImporter
{
public:
  AssetImporter(WestLogger* l);
  ~AssetImporter();

  void init();
  void shutdown();
  std::vector<Mesh> getMeshQueue(); 


private:
  const void handlePath(const std::string& path);
  std::string generateGUID(const std::string& path);
  Mesh handleFile(const std::string& path, const std::string uuid); 

  WestLogger* _logger;
  tQueue<Mesh> _queue;
  std::unique_ptr<AssetPathScreener> _screener;
};
