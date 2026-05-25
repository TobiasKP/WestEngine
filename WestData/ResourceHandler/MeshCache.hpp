#pragma once

#include "../Constants/Limits.hpp"
#include "../Data/Mesh.hpp"

#include <array>
#include <optional>
#include <unordered_map>

class MeshCache
{
public:
  MeshCache();
  ~MeshCache();

  std::optional<Mesh> getMesh(const std::string& guid);
  void setupMesh(const Mesh& m);
  void putMesh(const Mesh& m);
  void clear();

private:
  std::array<Mesh, Limit::cachesize> _cache;
  std::unordered_map<std::string, std::uint16_t> _guidToIdx;
};
