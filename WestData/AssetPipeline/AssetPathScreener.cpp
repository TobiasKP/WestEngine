#include "AssetPathScreener.hpp"

#include <cassert>
#include <CoreConstants.hpp>
#include <filesystem>
#include <PathUtils.h>
#include <format>
#include <string>
#include <algorithm>

AssetPathScreener::AssetPathScreener(WestLogger* l, const std::function<void(const std::string& path)> callback)
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
}

void AssetPathScreener::stop()
{
  if (_stop)
  {
    _logger->log(Level::Error, "AssetPathScreener already stopping ... \n");
    return;
  }
  _stop = true;
  _t.join();
}

void AssetPathScreener::notifyOnNew()
{
  while (true)
  {
    if (_stop)
    {
      return;
    }

    std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(150));
#ifdef DEBUG
    _logger->log(Level::Cycle, "Scanning for new assets ... \n");
#endif

    scanDir();
    if (_files.size() == 0)
    {
      continue;
    }

    _logger->log(Level::Info, std::format("|*| : Found {} new files, adding to import queue\n", _files.size()));
    for (const std::string& file : _files)
    {
      _callback(file);
    }
    _files.clear();
  }
}

void AssetPathScreener::scanDir()
{
  std::string path = std::format("{}{}", PathUtils::getExecutableDir(), CoreConstants::ASSET_PATH);
  for (const auto& entry : std::filesystem::directory_iterator(path))
  {
    if (std::filesystem::is_directory(entry))
    {
      _logger->log(Level::Error,
                   std::format("Entry: {} for file loading is a directory not a file\n", entry.path().string()));
      continue;
    }
    _logger->log(Level::Cycle, std::format("|*| Reading: {} for import\n", entry.path().filename().string()));
    const std::string filename  = entry.path().stem().string();
    const std::string extension = entry.path().extension().string();
    if (filename.starts_with("_"))
    {
      continue;
    }
    if (std::end(_allowList) == std::find(std::begin(_allowList), std::end(_allowList), extension))
    {
      _logger->log(Level::Error, std::format("Extension: {} for file loading not allowed\n", extension));
      continue;
    }

    std::error_code ec;
    const std::string newName = path + "_" + entry.path().filename().string();
    const std::string oldName = entry.path().string();
    std::filesystem::rename(oldName, newName, ec);
    if (ec.value() > 0)
    {
      _logger->log(Level::Error,
                   std::format("Error renaming file from: {} to: {} with Error: {}\n", oldName, newName, ec.value()));
      continue;
    }
    _files.push_back(newName);
    if (_files.size() == 25)
    {
      return;
    }
  }
};

void AssetPathScreener::addOnRequest(const std::string& file)
{
  _logger->log(Level::Cycle, std::format("|*| Reading: {} for import\n", file));
  std::string path     = std::format("{}{}", PathUtils::getExecutableDir(), CoreConstants::ASSET_PATH);
  std::string fullfile = std::format("{}{}{}", PathUtils::getExecutableDir(), CoreConstants::ASSET_PATH, "_" + file);
  if (std::filesystem::exists(fullfile))
  {
    _logger->log(Level::Info, std::format("|*| Requested file: {} is already processed or in process\n", file));
    return;
  };

  std::error_code ec;
  const std::string newName = path + "_" + file;
  const std::string oldName = path + file;
  std::filesystem::rename(oldName, newName, ec);
  if (ec.value() > 0)
  {
    _logger->log(Level::Error,
                 std::format("Error renaming file from: {} to: {} with Error: {}\n", oldName, newName, ec.value()));
    return;
  }
  _callback(newName);
}
