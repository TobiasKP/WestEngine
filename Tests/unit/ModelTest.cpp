#include <gtest/gtest.h>

#include <Data/Model.hpp>

// ─── Model basics ─────────────────────────────────────────────

TEST(Model, DefaultConstructorCreatesEmptyModel)
{
  Model m;
  EXPECT_TRUE(m.getGuid().empty());
  EXPECT_TRUE(m.getName().empty());
  EXPECT_TRUE(m.getMeshes().empty());
}

TEST(Model, SetAndGetGuid)
{
  Model m;
  m.setGuid("test-guid");
  EXPECT_EQ(m.getGuid(), "test-guid");
}

TEST(Model, SetAndGetName)
{
  Model m;
  m.setName("TestModel");
  EXPECT_EQ(m.getName(), "TestModel");
}

TEST(Model, AddMeshIncreasesCount)
{
  Model m;
  std::vector<Vertex> verts = {{{0, 0, 0}, {0, 1, 0}, {0, 0}}};
  std::vector<std::uint32_t> indices = {0};
  std::vector<Texture> textures;
  AABB aabb = {{0, 0, 0}, {1, 1, 1}};
  Mesh mesh("m1", verts, indices, textures, aabb);
  m.addMesh(mesh);

  EXPECT_EQ(m.getMeshes().size(), 1u);
}

// ─── Mesh data ────────────────────────────────────────────────

TEST(Model, MeshRetainsVertexData)
{
  std::vector<Vertex> verts = {
    {{1, 2, 3}, {0, 1, 0}, {0.5, 0.5}},
    {{4, 5, 6}, {0, 0, 1}, {1.0, 0.0}},
  };
  std::vector<std::uint32_t> indices = {0, 1};
  std::vector<Texture> textures;
  AABB aabb = {{1, 2, 3}, {4, 5, 6}};
  Mesh mesh("m1", verts, indices, textures, aabb);

  EXPECT_EQ(mesh.vertices.size(), 2u);
  EXPECT_EQ(mesh.indices.size(), 2u);
  EXPECT_FLOAT_EQ(mesh.vertices[0].Position.x, 1.0f);
  EXPECT_FLOAT_EQ(mesh.vertices[1].Position.z, 6.0f);
  EXPECT_FLOAT_EQ(mesh.aabb.min.x, 1.0f);
  EXPECT_FLOAT_EQ(mesh.aabb.max.z, 6.0f);
}

// ─── Material defaults ───────────────────────────────────────

TEST(Model, MeshMaterialHasCorrectDefaults)
{
  std::vector<Vertex> verts = {{{0, 0, 0}, {0, 1, 0}, {0, 0}}};
  std::vector<std::uint32_t> indices = {0};
  std::vector<Texture> textures;
  AABB aabb = {{0, 0, 0}, {1, 1, 1}};
  Mesh mesh("m1", verts, indices, textures, aabb);

  EXPECT_EQ(mesh.material.diffuseColor, glm::vec3(1, 0, 0));
  EXPECT_EQ(mesh.material.emissiveColor, glm::vec3(0));
}

// ─── Multiple meshes ──────────────────────────────────────────

TEST(Model, MultipleMeshesAreStored)
{
  Model m;
  std::vector<Vertex> verts = {{{0, 0, 0}, {0, 1, 0}, {0, 0}}};
  std::vector<std::uint32_t> indices = {0};
  std::vector<Texture> textures;
  AABB aabb = {{0, 0, 0}, {1, 1, 1}};

  m.addMesh(Mesh("m1", verts, indices, textures, aabb));
  m.addMesh(Mesh("m2", verts, indices, textures, aabb));
  m.addMesh(Mesh("m3", verts, indices, textures, aabb));

  EXPECT_EQ(m.getMeshes().size(), 3u);
}
