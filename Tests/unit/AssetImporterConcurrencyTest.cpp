#include <gtest/gtest.h>

#include <AssetPipeline/AssetImporter.hpp>
#include <CoreConstants.hpp>
#include <PathUtils.h>

#include <filesystem>
#include <format>
#include <latch>
#include <set>
#include <thread>

// AssetPathScreener only reads from the hardcoded <exeDir>/assets/Models/
class AssetImporterConcurrencyTest : public ::testing::Test
{
protected:
  WestLogger& logger = WestLogger::getLoggerInstance();
  std::filesystem::path dir =
    std::filesystem::path(PathUtils::getExecutableDir() + std::string(CoreConstants::ASSET_PATH));
  std::filesystem::path source = std::filesystem::path(WEST_SOURCE_DIR) / "WestGame/Assets/Models/human_01.obj";

  void SetUp() override
  {
    std::filesystem::remove_all(dir);
    std::filesystem::create_directories(dir);
  }

  void TearDown() override
  {
    std::filesystem::remove_all(dir);
  }

  void stage(const std::string& name)
  {
    std::filesystem::copy_file(source, dir / name);
  }
};

TEST_F(AssetImporterConcurrencyTest, ConcurrentOnRequestImportsProduceIndependentModels)
{
  AssetImporter importer(&logger);

  stage("reference.obj");
  importer.addOnRequest("reference.obj");
  std::vector<Model> reference = importer.getMeshQueue();
  ASSERT_EQ(reference.size(), 1u);
  const std::vector<Mesh>& refMeshes = reference[0].getMeshes();
  ASSERT_FALSE(refMeshes.empty());

  constexpr int rounds = 10;
  std::set<std::string> expected;
  for (int r = 0; r < rounds; r++)
  {
    const std::string a = std::format("human_{}_a.obj", r);
    const std::string b = std::format("human_{}_b.obj", r);
    stage(a);
    stage(b);
    expected.insert(std::format("_human_{}_a", r));
    expected.insert(std::format("_human_{}_b", r));

    std::latch start(2);
    auto request = [&](const std::string& file)
    {
      start.arrive_and_wait();
      importer.addOnRequest(file);
    };
    std::thread t1(request, a);
    std::thread t2(request, b);
    t1.join();
    t2.join();
  }

  std::vector<Model> models = importer.getMeshQueue();
  ASSERT_EQ(models.size(), expected.size());
  std::set<std::string> names;
  for (const Model& m : models)
  {
    names.insert(m.getName());
    EXPECT_FALSE(m.getGuid().empty());
    ASSERT_EQ(m.getMeshes().size(), refMeshes.size()) << m.getName();
    for (size_t i = 0; i < refMeshes.size(); i++)
    {
      EXPECT_EQ(m.getMeshes()[i].vertices.size(), refMeshes[i].vertices.size()) << m.getName();
      EXPECT_EQ(m.getMeshes()[i].indices.size(), refMeshes[i].indices.size()) << m.getName();
    }
  }
  EXPECT_EQ(names, expected);
}
