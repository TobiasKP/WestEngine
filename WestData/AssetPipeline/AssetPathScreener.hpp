#pragma once

#include <functional>
#include <WestLogger.h>

class AssetPathScreener
{
public:
  AssetPathScreener(WestLogger* l, const std::function<void*(const std::string& path)> callback);
  ~AssetPathScreener();

  void run();
  void stop();

private:
  void notify();

  std::function<void*(const std::string& path)> _callback;
  WestLogger* _logger;
};
