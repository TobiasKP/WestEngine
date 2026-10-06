#include <gtest/gtest.h>
#include <PathUtils.h>

TEST(PathUtils, GetExecutableDirReturnsNonEmptyPath)
{
  const std::string& dir = PathUtils::getExecutableDir();
  EXPECT_FALSE(dir.empty());
}
