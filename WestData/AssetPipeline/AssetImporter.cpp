#include "AssetImporter.hpp"

#include <random>
#include <sstream>

AssetImporter::AssetImporter(WestLogger* l)
{
  _logger   = l;
  _screener = std::make_unique<AssetPathScreener>(l, [this](const std::string& path) { handlePath(path); });
}

AssetImporter::~AssetImporter() {}

void AssetImporter::init()
{
#ifdef DEBUG
  _logger->log(Level::Info, "|*| Starting up asset import machine\n");
#endif
  _screener->run();
}

void AssetImporter::shutdown()
{

#ifdef DEBUG
  _logger->log(Level::Info, "|*| Stopping asset import machine\n");
#endif
  _screener->stop();
}

const void AssetImporter::handlePath(const std::string& path)
{
  std::string guid = generateGUID(path);
  assert(guid.length() > 0);
#ifdef DEBUG
  _logger->log(Level::Info, std::format("|*| Generated new uuid: {} for resource: {}\n", guid, path));
#endif
  Mesh mesh = handleFile(path, guid);
  assert(mesh.getGuid().compare(guid) == 0);
  _queue.push(std::move(mesh));
};

std::vector<Mesh> AssetImporter::getMeshQueue()
{
  return _queue.drain();
}

std::string AssetImporter::generateGUID(const std::string& path)
{
  static std::random_device rd;
  static std::mt19937_64 gen(rd());
  static std::uniform_int_distribution<> dis(0, 15);
  static std::uniform_int_distribution<> dis2(8, 11);

  std::stringstream ss;
  std::uint32_t i;
  ss << std::hex;
  for (i = 0; i < 8; i++)
  {
    ss << dis(gen);
  }
  ss << "-";
  for (i = 0; i < 4; i++)
  {
    ss << dis(gen);
  }
  ss << "-4";
  for (i = 0; i < 3; i++)
  {
    ss << dis(gen);
  }
  ss << "-" << dis2(gen);
  for (i = 0; i < 3; i++)
  {
    ss << dis(gen);
  }
  ss << "-";
  for (i = 0; i < 12; i++)
  {
    ss << dis(gen);
  }
  return ss.str();
};

Mesh AssetImporter::handleFile(const std::string& path, const std::string uuid)
{
  std::vector<Vertex> v;
  std::vector<std::uint32_t> i;
  std::vector<Texture> t;

  return Mesh(uuid, v, i, t);
};
