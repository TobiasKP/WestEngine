#include "BinaryLoader.hpp"

#include "../Constants/MagicNumbers.hpp"
#include "../Utils/AssetUtils.hpp"

#include <filesystem>
#include <PathUtils.h>

BinaryLoader::BinaryLoader(WestLogger* l)
{
  _logger    = l;
  _converter = std::make_unique<Converter>(l);
}

BinaryLoader::~BinaryLoader() {}


void BinaryLoader::init()
{
  _converter->init();
}

void BinaryLoader::shutdown()
{
  _converter->shutdown();
}

bool BinaryLoader::exists(const std::string& modelname)
{
  std::string filename = std::format("{}/{}.west", PathUtils::getExecutableDir() + "/bin", "_" + modelname);
  return std::filesystem::exists(filename);
}

std::optional<Model> BinaryLoader::load(const std::string& modelname)
{
  if (exists(modelname))
  {
    return loadFromDisk(modelname);
  }
  else
  {
    _logger->log(Level::Info, std::format("|*| Need to bake model {}, this could take a while ...  \n", modelname));
    _converter->addOnRequest(modelname);
    _converter->convertQueueElements();
    return loadFromDisk(modelname);
  }
}

std::optional<Model> BinaryLoader::loadFromDisk(const std::string& modelname)
{
  std::string filename = std::format(
    "{}/{}.west", PathUtils::getExecutableDir() + "/bin", "_" + std::filesystem::path(modelname).stem().string());
  std::string guid = AssetUtils::generateGUID(filename);
  _logger->log(Level::Info, std::format("|*| Loading binary model {} from disk\n ", filename));
  assert(std::filesystem::exists(filename));
  std::ifstream file(filename, std::ios::binary);
  if (!file)
  {
    _logger->log(Level::Error, std::format("|*| Error opening binary file: {} from disk\n", filename));
    return {};
  }
  char* buffer = new char[MagicNumbers::MAGIC_NUMBER.size()];
  file.read(buffer, MagicNumbers::MAGIC_NUMBER.size());
  if (std::memcmp(buffer, MagicNumbers::MAGIC_NUMBER.data(), MagicNumbers::MAGIC_NUMBER.size()) != 0)
  {
    _logger->log(Level::Error, std::format("|*| File: {} not matching the expected file format.\n", filename));
    delete[] buffer;
    return {};
  }
  delete[] buffer;
  _logger->log(Level::Cycle, std::format("|*| Converting file: {} to model\n", filename));
  return convertToModel(file, guid);
}

std::optional<Model> BinaryLoader::convertToModel(std::ifstream& is, const std::string& guid)
{
  Model m;
  std::uint32_t count = 0;
  m.setGuid(guid);

  while (true)
  {
    glm::vec3 aabbMin(FLT_MAX);
    glm::vec3 aabbMax(-FLT_MAX);
    is.seekg(MagicNumbers::MESH.size() + MagicNumbers::VERTICE.size(), std::ios::cur);
    size_t verticeSize = 0;
    is.read(reinterpret_cast<char*>(&verticeSize), sizeof(size_t));
    if (verticeSize == 0)
    {
      _logger->log(Level::Info, std::format("|*| Warning: Read 0 vertices for given model\n"));
      return {};
    }
    is.seekg(MagicNumbers::DELIMITER.size(), std::ios::cur);
    std::vector<Vertex> vertices(verticeSize);
    is.read(reinterpret_cast<char*>(vertices.data()), verticeSize * sizeof(Vertex));
    for (const Vertex& v : vertices)
    {
      aabbMin = glm::min(aabbMin, v.Position);
      aabbMax = glm::max(aabbMax, v.Position);
    }

#ifdef DEBUG
    char* buffer = new char[MagicNumbers::INDICE.size()];
    is.read(buffer, MagicNumbers::INDICE.size());
    assert(std::memcmp(buffer, MagicNumbers::INDICE.data(), MagicNumbers::INDICE.size()) == 0);
    delete[] buffer;
#else
    is.seekg(MagicNumbers::INDICE.size(), std::ios::cur);
#endif
    size_t indiceSize = 0;
    is.read(reinterpret_cast<char*>(&indiceSize), sizeof(size_t));
    if (indiceSize == 0)
    {
      _logger->log(Level::Info, std::format("|*| Warning: Read 0 indices for given model\n"));
      return {};
    }
    is.seekg(MagicNumbers::DELIMITER.size(), std::ios::cur);
    std::vector<std::uint32_t> indices(indiceSize);
    is.read(reinterpret_cast<char*>(indices.data()), indiceSize * sizeof(std::uint32_t));

#ifdef DEBUG
    buffer = new char[MagicNumbers::MATERIAL.size()];
    is.read(buffer, MagicNumbers::MATERIAL.size());
    assert(std::memcmp(buffer, MagicNumbers::MATERIAL.data(), MagicNumbers::MATERIAL.size()) == 0);
    delete[] buffer;
#else
    is.seekg(MagicNumbers::MATERIAL.size(), std::ios::cur);
#endif

    Material material;
    is.seekg(MagicNumbers::KD.size(), std::ios::cur);
    is.read(reinterpret_cast<char*>(&material.diffuseColor), sizeof(glm::vec3));
    is.seekg(MagicNumbers::KE.size(), std::ios::cur);
    is.read(reinterpret_cast<char*>(&material.emissiveColor), sizeof(glm::vec3));
    is.seekg(MagicNumbers::KS.size(), std::ios::cur);
    is.read(reinterpret_cast<char*>(&material.specularColor), sizeof(glm::vec3));
    is.seekg(MagicNumbers::DELIMITER.size(), std::ios::cur);

#ifdef DEBUG
    buffer = new char[MagicNumbers::TEXTURE.size()];
    is.read(buffer, MagicNumbers::TEXTURE.size());
    assert(std::memcmp(buffer, MagicNumbers::TEXTURE.data(), MagicNumbers::TEXTURE.size()) == 0);
    delete[] buffer;
#else
    is.seekg(MagicNumbers::TEXTURE.size(), std::ios::cur);
#endif

    size_t textureSize = 0;
    is.read(reinterpret_cast<char*>(&textureSize), sizeof(size_t));
    std::vector<Texture> textures;
    if (textureSize == 0)
    {
      _logger->log(Level::Info, std::format("|*| Info: Read 0 textures for given model\n"));
      is.seekg(MagicNumbers::DELIMITER.size(), std::ios::cur);
    }
    else
    {
      textures.reserve(textureSize);
      is.seekg(MagicNumbers::DELIMITER.size(), std::ios::cur);
      for (std::uint32_t i = 0; i < textureSize; i++)
      {
        std::int32_t width = 0, height = 0, numComponents = 0;
        size_t dataLength = 0, typeLength = 0;
        std::string type;
        is.read(reinterpret_cast<char*>(&width), sizeof(std::int32_t));
        is.read(reinterpret_cast<char*>(&height), sizeof(std::int32_t));
        is.read(reinterpret_cast<char*>(&numComponents), sizeof(std::int32_t));

        is.seekg(MagicNumbers::IMG_DATA.size(), std::ios::cur);
        is.read(reinterpret_cast<char*>(&dataLength), sizeof(size_t));
        is.seekg(MagicNumbers::DELIMITER.size(), std::ios::cur);
        assert(dataLength != 0 && width != 0 && height != 0);
        std::vector<unsigned char> imageData(dataLength);
        is.read(reinterpret_cast<char*>(imageData.data()), sizeof(unsigned char) * dataLength);

        is.seekg(MagicNumbers::IMG_TYPE.size(), std::ios::cur);
        is.read(reinterpret_cast<char*>(&typeLength), sizeof(size_t));
        is.seekg(MagicNumbers::DELIMITER.size(), std::ios::cur);
        assert(typeLength != 0);
        type.resize(typeLength);
        is.read(reinterpret_cast<char*>(type.data()), typeLength);
        Texture tex;
        tex.width         = width;
        tex.height        = height;
        tex.numComponents = numComponents;
        tex.imageData     = std::move(imageData);
        tex.type          = type;
        textures.push_back(tex);
        is.seekg(MagicNumbers::DELIMITER.size(), std::ios::cur);
      }
    }

    m.addMesh(
      Mesh(std::format("{}_{}", guid, count), vertices, indices, textures, AABB(aabbMin, aabbMax), material));
    if (is.peek() == EOF)
    {
      break;
    }
    count++;
  }

  return m;
}
