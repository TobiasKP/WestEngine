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

// ─── Edge cases ───────────────────────────────────────────────

static std::size_t countModelsWithGuid(DataPool& pool, const std::string& guid)
{
  std::size_t n = 0;
  for (const Model& m : pool.getSceneModels())
  {
    if (m.getGuid() == guid)
    {
      n++;
    }
  }
  return n;
}

TEST_F(DataPoolTest, DeleteLastKeepsEarlierModels)
{
  pool.addModelToScene(makeTestModel("A", "guid-a"));
  pool.addModelToScene(makeTestModel("B", "guid-b"));

  EXPECT_TRUE(pool.deleteModelFromScene("guid-b"));

  EXPECT_EQ(pool.getModelByGuid("guid-b"), nullptr);
  EXPECT_EQ(pool.getModelByName("B"), nullptr);
  const Model* a = pool.getModelByGuid("guid-a");
  ASSERT_NE(a, nullptr);
  EXPECT_EQ(a->getName(), "A");
}

// BUG: WestData/MiscDataHandler/DataPool.cpp:80-83 deleting the element at the tail swaps it with itself, clears it, then re-indexes the now empty model, leaving a stale "" key in _guidToIndex that resolves to an empty slot.
TEST_F(DataPoolTest, DISABLED_DeleteLastElementLeavesNoStaleEmptyGuid)
{
  pool.addModelToScene(makeTestModel("A", "guid-a"));
  pool.addModelToScene(makeTestModel("B", "guid-b"));

  ASSERT_TRUE(pool.deleteModelFromScene("guid-b"));

  EXPECT_EQ(pool.getModelByGuid(""), nullptr) << "no model was ever added with an empty guid";
  EXPECT_FALSE(pool.deleteModelFromScene("")) << "an empty guid must not be deletable";
}

// BUG: WestData/MiscDataHandler/DataPool.cpp:59 addModelToScene does not check for an existing guid; the second add takes a new slot and repoints the index, so the first copy is orphaned and survives every later delete.
TEST_F(DataPoolTest, DISABLED_DuplicateGuidLeavesNoOrphanAfterDelete)
{
  pool.addModelToScene(makeTestModel("First", "guid-dup"));
  pool.addModelToScene(makeTestModel("Second", "guid-dup"));

  // Either policy is fine (reject the duplicate or replace the model), but once
  // the guid is deleted nothing may remain under it.
  ASSERT_TRUE(pool.deleteModelFromScene("guid-dup"));
  EXPECT_EQ(pool.getModelByGuid("guid-dup"), nullptr);
  EXPECT_EQ(countModelsWithGuid(pool, "guid-dup"), 0u) << "an unreachable copy is still stored in the pool";
}

TEST_F(DataPoolTest, DuplicateGuidLookupReturnsAModelWithThatGuid)
{
  pool.addModelToScene(makeTestModel("First", "guid-dup"));
  pool.addModelToScene(makeTestModel("Second", "guid-dup"));

  const Model* m = pool.getModelByGuid("guid-dup");
  ASSERT_NE(m, nullptr);
  EXPECT_EQ(m->getGuid(), "guid-dup");
}

TEST_F(DataPoolTest, AddFailsOnceThePoolIsFull)
{
  bool sawFailure = false;
  for (std::uint32_t i = 0; i <= Limit::cachesize; i++)
  {
    if (!pool.addModelToScene(makeTestModel("M" + std::to_string(i), "guid-" + std::to_string(i))))
    {
      sawFailure = true;
    }
  }
  EXPECT_TRUE(sawFailure) << "adding cachesize + 1 models must be rejected at some point";
}

// BUG: WestData/MiscDataHandler/DataPool.cpp:54 the full check `max_size() - 1 == _currentIdx` rejects the add when one slot is still free, so the pool only ever holds cachesize - 1 models.
TEST_F(DataPoolTest, DISABLED_PoolHoldsExactlyCachesizeModels)
{
  for (std::uint32_t i = 0; i < Limit::cachesize; i++)
  {
    ASSERT_TRUE(pool.addModelToScene(makeTestModel("M" + std::to_string(i), "guid-" + std::to_string(i))))
      << "add #" << i << " of " << Limit::cachesize << " rejected";
  }
  EXPECT_FALSE(pool.addModelToScene(makeTestModel("Overflow", "guid-overflow")));
  EXPECT_NE(pool.getModelByGuid("guid-" + std::to_string(Limit::cachesize - 1)), nullptr);
}
