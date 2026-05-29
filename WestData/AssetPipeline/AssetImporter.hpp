#pragma once

#include "../Data/Model.hpp"
#include "AssetPathScreener.hpp"
#include "assimp/Importer.hpp"

#include <assimp/scene.h>
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
  std::vector<Model> getMeshQueue();


private:
  void handlePath(const std::string& path);
  void processNode(aiNode* node, const aiScene* scene, const std::string& guid);
  Mesh processMesh(aiMesh* mesh, const aiScene* scene, const std::string& guid);
  std::string generateGUID(const std::string& path);
  void handleFile(const std::string& path, const std::string& guid);
  std::vector<Vertex> processVertices(aiMesh* mesh);
  std::vector<std::uint32_t> processIndices(aiMesh* mesh);
  std::vector<Texture> processTextures(aiMesh* mesh, const aiScene* scene);
  std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
  Texture textureFromFile(const std::string& path, const std::string& typeName);

  Model _model;
  WestLogger* _logger;
  tQueue<Model> _queue;
  Assimp::Importer _importer;
  std::unique_ptr<AssetPathScreener> _screener;
};
