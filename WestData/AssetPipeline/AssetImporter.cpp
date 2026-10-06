#include "AssetImporter.hpp"

#include "../Utils/AssetUtils.hpp"

#define STB_IMAGE_IMPLEMENTATION

#include "../Data/Model.hpp"
#include "../Constants/TextureTypes.hpp"

#include <assimp/postprocess.h>
#include <filesystem>
#include <format>
#include <PathUtils.h>
#include <stb_image.h>


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

void AssetImporter::handlePath(const std::string& path)
{
  assert(_logger != nullptr);
  std::string guid = AssetUtils::generateGUID(path);
  assert(guid.length() > 0);
#ifdef DEBUG
  _logger->log(Level::Info, std::format("|*| Generated new uuid: {} for resource: {}\n", guid, path));
#endif
  handleFile(path, guid);
};

std::vector<Model> AssetImporter::getMeshQueue()
{
  return _queue.drain();
}


void AssetImporter::handleFile(const std::string& path, const std::string& guid)
{
  std::lock_guard lock(_importMutex);
  Model model;
  model.setGuid(guid);
  model.setName(std::filesystem::path(path).stem().string());
  const aiScene* scene = _importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
  {
    _logger->log(Level::Error, std::format("|*| Error importing file: {}", _importer.GetErrorString()));
    return;
  }
  processNode(scene->mRootNode, scene, guid, model);
  _queue.push(std::move(model));
};
void AssetImporter::processNode(aiNode* node, const aiScene* scene, const std::string& guid, Model& model)
{
  for (std::uint32_t i = 0; i < node->mNumMeshes; i++)
  {
    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
    model.addMesh(processMesh(mesh, scene, guid, i));
  }
  for (std::uint32_t i = 0; i < node->mNumChildren; i++)
  {
    processNode(node->mChildren[i], scene, guid, model);
  }
}

Material AssetImporter::processMaterial(aiMesh* mesh, const aiScene* scene)
{
  Material m;
  if (mesh->mMaterialIndex >= 0)
  {
    aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
    aiColor4D diffuse, specular, emissive;
    if (AI_SUCCESS == aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &diffuse))
    {
      m.diffuseColor = glm::vec3(diffuse.r, diffuse.g, diffuse.b);
    }

    if (AI_SUCCESS == aiGetMaterialColor(mat, AI_MATKEY_COLOR_EMISSIVE, &emissive))
    {
      m.emissiveColor = glm::vec3(emissive.r, emissive.g, emissive.b);
    }

    if (AI_SUCCESS == aiGetMaterialColor(mat, AI_MATKEY_COLOR_SPECULAR, &specular))
    {
      m.specularColor = glm::vec3(specular.r, specular.g, specular.b);
    }
  }
  return m;
}

Mesh AssetImporter::processMesh(aiMesh* mesh, const aiScene* scene, const std::string& guid, std::uint32_t count)
{
  std::vector<Vertex> vertices       = processVertices(mesh);
  std::vector<std::uint32_t> indices = processIndices(mesh);
  std::vector<Texture> textures      = processTextures(mesh, scene);
  Material mat                       = processMaterial(mesh, scene);
  return Mesh(std::format("{}_{}", guid, count), vertices, indices, textures, AABB(), std::move(mat));
}

std::vector<Vertex> AssetImporter::processVertices(aiMesh* mesh)
{
  std::vector<Vertex> result;
  for (std::uint32_t i = 0; i < mesh->mNumVertices; i++)
  {
    Vertex vertex;
    glm::vec3 vector;
    vector.x        = mesh->mVertices[i].x;
    vector.y        = mesh->mVertices[i].y;
    vector.z        = mesh->mVertices[i].z;
    vertex.Position = vector;
    vector.x        = mesh->mNormals[i].x;
    vector.y        = mesh->mNormals[i].y;
    vector.z        = mesh->mNormals[i].z;
    vertex.Normal   = vector;
    if (mesh->mTextureCoords[0])
    {
      glm::vec2 vec;
      vec.x            = mesh->mTextureCoords[0][i].x;
      vec.y            = mesh->mTextureCoords[0][i].y;
      vertex.TexCoords = vec;
    }
    else
    {
      vertex.TexCoords = glm::vec2(0, 0);
    }
    result.push_back(std::move(vertex));
  }

  return result;
};

std::vector<std::uint32_t> AssetImporter::processIndices(aiMesh* mesh)
{
  std::vector<std::uint32_t> result;
  for (std::uint32_t i = 0; i < mesh->mNumFaces; i++)
  {
    aiFace face = mesh->mFaces[i];
    for (std::uint32_t j = 0; j < face.mNumIndices; j++)
    {
      result.push_back(face.mIndices[j]);
    }
  }
  return result;
};

std::vector<Texture> AssetImporter::processTextures(aiMesh* mesh, const aiScene* scene)
{
  std::vector<Texture> result;
  if (mesh->mMaterialIndex >= 0)
  {
    aiMaterial* mat                  = scene->mMaterials[mesh->mMaterialIndex];
    std::vector<Texture> diffuseMaps = loadMaterialTextures(mat, aiTextureType_DIFFUSE, TextureTypes::DIFFUSE.data());
    result.insert(result.end(), diffuseMaps.begin(), diffuseMaps.end());
    std::vector<Texture> specularMaps = loadMaterialTextures(mat, aiTextureType_SPECULAR, TextureTypes::SPECULAR.data());
    result.insert(result.end(), specularMaps.begin(), specularMaps.end());
  }

  return result;
};

std::vector<Texture>
AssetImporter::loadMaterialTextures(aiMaterial* mat, const aiTextureType type, const std::string typeName)
{
  std::vector<Texture> result;
  for (std::uint32_t i = 0; i < mat->GetTextureCount(type); i++)
  {
    aiString str;
    mat->GetTexture(type, i, &str);
    result.push_back(textureFromFile(str.C_Str(), typeName));
  }
  return result;
};

Texture AssetImporter::textureFromFile(const std::string& file, const std::string& typeName)
{
  Texture tex;
  tex.type = typeName;
  std::int32_t width, height, numComponents;
  std::string filePath = PathUtils::resolve("/" + file);
#ifdef DEBUG
  _logger->log(Level::Info, std::format("|*| Loading Texture: {}\n", filePath));
#endif
  unsigned char* imgData = stbi_load(filePath.c_str(), &width, &height, &numComponents, 0);
  if (imgData == NULL)
  {
    _logger->log(
      Level::Error,
      std::format("|*| No Imagedata loaded for texture: {} - STBI Error: {}\n", filePath, stbi_failure_reason()));
    tex.type = "Error";
    return tex;
  }
  tex.width         = width;
  tex.height        = height;
  tex.numComponents = numComponents;
  tex.imageData     = std::vector<unsigned char>(imgData, imgData + width * height * numComponents);
  stbi_image_free(imgData);
  return tex;
};

void AssetImporter::addOnRequest(const std::string& path)
{
  _screener->addOnRequest(path);
};
