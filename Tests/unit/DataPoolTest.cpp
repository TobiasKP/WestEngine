#include <gtest/gtest.h>

#include <Data/Model.hpp>
#include <MiscDataHandler/DataPool.hpp>

static Model makeTestModel(const std::string& name, const std::string& guid)
{
  Model m;
  m.setName(name);
  m.setGuid(guid);

  std::vector<Vertex> verts = {
    {{0, 0, 0}, {0, 1, 0}, {0, 0}},
    {{1, 0, 0}, {0, 1, 0}, {1, 0}},
    {{0, 1, 0}, {0, 1, 0}, {0, 1}},
  };
  std::vector<std::uint32_t> indices = {0, 1, 2};
  std::vector<Texture> textures;
  AABB aabb = {{0, 0, 0}, {1, 1, 0}};
  Mesh mesh("mesh-1", verts, indices, textures, aabb);
  mesh.material.diffuseColor = glm::vec3(1, 0, 0);
  m.addMesh(mesh);
  return m;
}

class DataPoolTest : public ::testing::Test
{
protected:
  WestLogger& logger = WestLogger::getLoggerInstance();
  DataPool pool{&logger};

  void SetUp() override
  {
    pool.init();
  }
};

// ─── Add and retrieve ─────────────────────────────────────────

TEST_F(DataPoolTest, AddAndGetByGuid)
{
  Model m = makeTestModel("Cube", "guid-cube");
  EXPECT_TRUE(pool.addModelToScene(std::move(m)));

  const Model* result = pool.getModelByGuid("guid-cube");
  ASSERT_NE(result, nullptr);
  EXPECT_EQ(result->getName(), "Cube");
  EXPECT_EQ(result->getGuid(), "guid-cube");
}

TEST_F(DataPoolTest, AddAndGetByName)
{
  Model m = makeTestModel("Sphere", "guid-sphere");
  pool.addModelToScene(std::move(m));

  const Model* result = pool.getModelByName("Sphere");
  ASSERT_NE(result, nullptr);
  EXPECT_EQ(result->getGuid(), "guid-sphere");
}

TEST_F(DataPoolTest, GetByGuidReturnsNullForMissing)
{
  EXPECT_EQ(pool.getModelByGuid("nonexistent"), nullptr);
}

TEST_F(DataPoolTest, GetByNameReturnsNullForMissing)
{
  EXPECT_EQ(pool.getModelByName("nonexistent"), nullptr);
}

// ─── Delete ───────────────────────────────────────────────────

TEST_F(DataPoolTest, DeleteRemovesModel)
{
  Model m = makeTestModel("ToDelete", "guid-delete");
  pool.addModelToScene(std::move(m));

  EXPECT_TRUE(pool.deleteModelFromScene("guid-delete"));
  EXPECT_EQ(pool.getModelByGuid("guid-delete"), nullptr);
  EXPECT_EQ(pool.getModelByName("ToDelete"), nullptr);
}

TEST_F(DataPoolTest, DeleteNonexistentReturnsFalse)
{
  EXPECT_FALSE(pool.deleteModelFromScene("nonexistent"));
}

// ─── Swap-and-compact on delete ───────────────────────────────

TEST_F(DataPoolTest, DeleteMiddlePreservesOtherModels)
{
  pool.addModelToScene(makeTestModel("A", "guid-a"));
  pool.addModelToScene(makeTestModel("B", "guid-b"));
  pool.addModelToScene(makeTestModel("C", "guid-c"));

  pool.deleteModelFromScene("guid-b");

  EXPECT_NE(pool.getModelByGuid("guid-a"), nullptr);
  EXPECT_EQ(pool.getModelByGuid("guid-b"), nullptr);
  EXPECT_NE(pool.getModelByGuid("guid-c"), nullptr);

  EXPECT_EQ(pool.getModelByGuid("guid-a")->getName(), "A");
  EXPECT_EQ(pool.getModelByGuid("guid-c")->getName(), "C");
}

TEST_F(DataPoolTest, DeleteFirstPreservesLast)
{
  pool.addModelToScene(makeTestModel("First", "guid-first"));
  pool.addModelToScene(makeTestModel("Last", "guid-last"));

  pool.deleteModelFromScene("guid-first");

  EXPECT_EQ(pool.getModelByGuid("guid-first"), nullptr);
  const Model* last = pool.getModelByGuid("guid-last");
  ASSERT_NE(last, nullptr);
  EXPECT_EQ(last->getName(), "Last");
}

// ─── Model data integrity ─────────────────────────────────────

TEST_F(DataPoolTest, StoredModelRetainsMeshData)
{
  Model m = makeTestModel("Detailed", "guid-detail");
  pool.addModelToScene(std::move(m));

  const Model* result = pool.getModelByGuid("guid-detail");
  ASSERT_NE(result, nullptr);
  EXPECT_EQ(result->getMeshes().size(), 1u);
  EXPECT_EQ(result->getMeshes().front().vertices.size(), 3u);
  EXPECT_EQ(result->getMeshes().front().indices.size(), 3u);
  EXPECT_EQ(result->getMeshes().front().material.diffuseColor, glm::vec3(1, 0, 0));
}

// ─── Add after delete reuses slot ─────────────────────────────

TEST_F(DataPoolTest, AddAfterDeleteWorks)
{
  pool.addModelToScene(makeTestModel("Old", "guid-old"));
  pool.deleteModelFromScene("guid-old");

  pool.addModelToScene(makeTestModel("New", "guid-new"));
  const Model* result = pool.getModelByGuid("guid-new");
  ASSERT_NE(result, nullptr);
  EXPECT_EQ(result->getName(), "New");
}

// ─── Multiple add-delete cycles ───────────────────────────────

TEST_F(DataPoolTest, MultipleAddDeleteCycles)
{
  for (int i = 0; i < 10; i++)
  {
    std::string name = "Model" + std::to_string(i);
    std::string guid = "guid-" + std::to_string(i);
    pool.addModelToScene(makeTestModel(name, guid));
  }

  // Delete even-numbered
  for (int i = 0; i < 10; i += 2)
  {
    pool.deleteModelFromScene("guid-" + std::to_string(i));
  }

  // Odd-numbered should still be accessible
  for (int i = 1; i < 10; i += 2)
  {
    const Model* m = pool.getModelByGuid("guid-" + std::to_string(i));
    ASSERT_NE(m, nullptr) << "Model with guid-" << i << " should still exist";
    EXPECT_EQ(m->getName(), "Model" + std::to_string(i));
  }

  // Even-numbered should be gone
  for (int i = 0; i < 10; i += 2)
  {
    EXPECT_EQ(pool.getModelByGuid("guid-" + std::to_string(i)), nullptr);
  }
}
