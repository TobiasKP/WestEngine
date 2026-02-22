#include <gtest/gtest.h>
#include <PathUtils.h>

TEST(PathUtils, GetExecutableDirReturnsNonEmptyPath)
{
  const std::string& dir = PathUtils::getExecutableDir();
  EXPECT_FALSE(dir.empty());
}

TEST(PathUtils, ResolveAppendsToExecutableDir)
{
  std::string resolved = PathUtils::resolve("/shader/test.vs");
  std::string expected = PathUtils::getExecutableDir() + "/shader/test.vs";
  EXPECT_EQ(resolved, expected);
}
