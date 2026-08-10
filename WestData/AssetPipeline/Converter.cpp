#include "Converter.hpp"

#include "../Constants/MagicNumbers.hpp"

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
  const std::string& p = m.getName();
  std::string filename = std::format("{}/{}.west", PathUtils::getExecutableDir() + "/bin", p);
  std::ofstream file(filename, std::ios::binary);
  if (!file)
  {
    _logger->log(Level::Error, std::format("|*| Can not create or open file: {}\n", filename));
    return;
  }

  file.write(MagicNumbers::MAGIC_NUMBER.data(), MagicNumbers::MAGIC_NUMBER.size());
  for (const Mesh& me : m.getMeshes())
  {
    file.write(MagicNumbers::MESH.data(), MagicNumbers::MESH.size());

#ifdef DEBUG
    _logger->log(Level::Cycle, std::format("|*| writing vertices for {}\n", m.getGuid()));
#endif
    file.write(MagicNumbers::VERTICE.data(), MagicNumbers::VERTICE.size());
    assert(std::is_trivially_copyable_v<Vertex>);
    write(file, me.vertices.size());
    file.write(MagicNumbers::DELIMITER.data(), MagicNumbers::DELIMITER.size());
    file.write(reinterpret_cast<const char*>(me.vertices.data()), me.vertices.size() * sizeof(Vertex));

#ifdef DEBUG
    _logger->log(Level::Cycle, std::format("|*| writing indices for {}\n", m.getGuid()));
#endif
    file.write(MagicNumbers::INDICE.data(), MagicNumbers::INDICE.size());
    write(file, me.indices.size());
    file.write(MagicNumbers::DELIMITER.data(), MagicNumbers::DELIMITER.size());
    file.write(reinterpret_cast<const char*>(me.indices.data()), me.indices.size() * sizeof(std::uint32_t));

#ifdef DEBUG
    _logger->log(Level::Cycle, std::format("|*| writing textures for {}\n", m.getGuid()));
#endif

    file.write(MagicNumbers::MATERIAL.data(), MagicNumbers::MATERIAL.size());
    file.write(MagicNumbers::KD.data(), MagicNumbers::KD.size());
    write(file, me.material.diffuseColor);
    file.write(MagicNumbers::KE.data(), MagicNumbers::KE.size());
    write(file, me.material.emissiveColor);
    file.write(MagicNumbers::KS.data(), MagicNumbers::KS.size());
    write(file, me.material.specularColor);
    file.write(MagicNumbers::DELIMITER.data(), MagicNumbers::DELIMITER.size());

    file.write(MagicNumbers::TEXTURE.data(), MagicNumbers::TEXTURE.size());
    write(file, me.textures.size());
    file.write(MagicNumbers::DELIMITER.data(), MagicNumbers::DELIMITER.size());
    for (const Texture& t : me.textures)
    {
      write(file, t.width);
      write(file, t.height);
      write(file, t.numComponents);

      file.write(MagicNumbers::IMG_DATA.data(), MagicNumbers::IMG_DATA.size());
      write(file, t.imageData.size());
      file.write(MagicNumbers::DELIMITER.data(), MagicNumbers::DELIMITER.size());
      file.write(reinterpret_cast<const char*>(t.imageData.data()), t.imageData.size() * sizeof(unsigned char));

      file.write(MagicNumbers::IMG_TYPE.data(), MagicNumbers::IMG_TYPE.size());
      write(file, t.type.length());
      file.write(MagicNumbers::DELIMITER.data(), MagicNumbers::DELIMITER.size());
      file.write(reinterpret_cast<const char*>(t.type.data()), t.type.size());

      file.write(MagicNumbers::DELIMITER.data(), MagicNumbers::DELIMITER.size());
    }
  }
}

void Converter::addOnRequest(const std::string& path)
{
  _importer->addOnRequest(path);
};
