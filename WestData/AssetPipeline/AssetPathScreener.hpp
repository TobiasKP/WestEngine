#pragma once

#include <functional>
#include <thread>
#include <array>
#include <string>
#include <vector>
#include <atomic>
#include <WestLogger.h>

class AssetPathScreener
{
public:
  AssetPathScreener(WestLogger* l, const std::function<void(const std::string& path)> callback);
  ~AssetPathScreener();

  void run();
  void stop();
  void addOnRequest(const std::string& file);

  inline bool isRunning()
  {
    return !_stop;
  };

private:
  void notifyOnNew();
  void scanDir(); 

  std::function<void(const std::string& path)> _callback;
  std::array<std::string, 1> _allowList = {".obj"};
  std::atomic_bool _stop;
  std::thread _t;
  std::vector<std::string> _files;

  WestLogger* _logger;
};
