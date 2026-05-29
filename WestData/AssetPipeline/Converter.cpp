#include "Converter.hpp"

#include "../Constants/MagicNumbers.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <PathUtils.h>

Converter::Converter(WestLogger* l)
{
  _logger   = l;
  _importer = std::make_unique<AssetImporter>(l);
}

Converter::~Converter()
{
  shutdown();
}

void Converter::init()
{
#ifdef DEBUG
  _logger->log(Level::Info, "|*| Starting up Converter ... \n");
#endif
  assert(_importer != nullptr);
  _importer->init();
}

void Converter::shutdown()
{
  _importer->shutdown();
}

void Converter::convertQueueElements()
{
  for (Model& m : _importer->getMeshQueue())
  {
    assert(m.getGuid().length() > 0);
    bake(m);
  }
};

void Converter::bake(Model& m)
{
#ifdef DEBUG
  _logger->log(Level::Info, std::format("|*| Backing mesh: {}\n", m.getName()));
#endif
  const std::filesystem::path p = m.getName();
  std::string filename = std::format("{}/{}.west", PathUtils::getExecutableDir() + "/bin", p.filename().string());
  std::ofstream file(filename, std::ios::binary);
  if (!file)
  {
    _logger->log(Level::Error, std::format("|*| Can not create or open file: {}\n", filename));
    return;
  }

  file.write(MagicNumbers::MAGIC_NUMBER.data(), MagicNumbers::MAGIC_NUMBER.size());
  for (Mesh& me : m.getMeshes())
  {
    file.write(MagicNumbers::MESH.data(), MagicNumbers::MESH.size());

#ifdef DEBUG
    _logger->log(Level::Cycle, std::format("|*| writing vertices for {}", m.getGuid()));
#endif
    file.write(MagicNumbers::VERTICE.data(), MagicNumbers::VERTICE.size());
    assert(std::is_trivially_copyable_v<Vertex>);
    write(file, me.vertices.size());
    file.write(reinterpret_cast<const char*>(me.vertices.data()), me.vertices.size() * sizeof(Vertex));

#ifdef DEBUG
    _logger->log(Level::Cycle, std::format("|*| writing indices for {}", m.getGuid()));
#endif
    file.write(MagicNumbers::INDICE.data(), MagicNumbers::INDICE.size());
    write(file, me.indices.size());
    file.write(reinterpret_cast<const char*>(me.indices.data()), me.indices.size() * sizeof(std::uint32_t));

#ifdef DEBUG
    _logger->log(Level::Cycle, std::format("|*| writing textures for {}", m.getGuid()));
#endif
    file.write(MagicNumbers::TEXTURE.data(), MagicNumbers::TEXTURE.size());
    write(file, me.textures.size());
    for (const Texture& t : me.textures)
    {
      write(file, t.width);
      write(file, t.height);
      write(file, t.numComponents);
      write(file, t.imageData.size());
      file.write(reinterpret_cast<const char*>(t.imageData.data()), t.imageData.size() * sizeof(unsigned char));
      write(file, t.type.length());
      file.write(reinterpret_cast<const char*>(t.type.data()), t.type.size());
    }
  }
}
