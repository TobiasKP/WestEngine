#include "AssetPathScreener.hpp"

#include <cassert>
#include <CoreConstants.hpp>
#include <filesystem>

AssetPathScreener::AssetPathScreener(WestLogger* l, const std::function<void*(const std::string& path)> callback)
{
  _callback = callback;
  _logger   = l;
  _stop     = true;
}

AssetPathScreener::~AssetPathScreener()
{
  if (isRunning())
  {
    stop();
  }
}

void AssetPathScreener::run()
{
  if (!_stop)
  {
    _logger->log(Level::Error, "AssetPathScreener already running, something went wrong! Interrupting process ... \n");
    stop();
    return;
  }
  _stop = false;
  _files.reserve(25);
  _t = std::thread(&AssetPathScreener::notifyOnNew, this);
  _t.detach();
}

void AssetPathScreener::stop()
{
  if (_stop)
  {
    _logger->log(Level::Error, "AssetPathScreener already stopping ... \n");
    return;
  }
  _stop = true;
}

void AssetPathScreener::notifyOnNew()
{
  while (true)
  {
    if (_stop)
    {
      return;
    }
    assert(_files.size() == 0);
    std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(150));

#ifdef DEBUG
    _logger->log(Level::Cycle, "Scanning for new assets ... \n");
#endif

    scanDir();
    if (_files.size() == 0)
    {
      continue;
    }

    _logger->log(Level::Info, std::format("AAA : Found {} new files, adding to import queue\n", _files.size()));
    for (const std::string& file : _files)
    {
      _callback(file);
    }
    _files.clear();
  }
}

void AssetPathScreener::scanDir()
{
  for (const auto& entry : std::filesystem::directory_iterator(CoreConstants::ASSET_PATH))
  {
    const std::string filename  = entry.path().stem().string();
    const std::string extension = entry.path().extension();
    if (filename.starts_with("_"))
    {
      continue;
    }
    if (std::end(_allowList) == std::find(std::begin(_allowList), std::end(_allowList), extension))
    {
      _logger->log(Level::Error, std::format("Extension: {} for file loading not allowed\n", extension));
      return;
    }
    _files.push_back(entry.path());
    std::error_code ec;
    const std::string newName = std::string(CoreConstants::ASSET_PATH) + "_" + entry.path().filename().string();
    const std::string oldName = entry.path().string();
    std::filesystem::rename(oldName, newName, ec);
    if (ec.value() > 0)
    {
      _files.pop_back();
      _logger->log(Level::Error,
                   std::format("Error renaming file from: {} to: {} with Error: {}\n", oldName, newName, ec.value()));
    }
    if (_files.size() == 25)
    {
      return;
    }
  }
};
