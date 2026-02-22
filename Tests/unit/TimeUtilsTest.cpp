#include <gtest/gtest.h>
#include <TimeUtils.hpp>

TEST(TimeUtils, GetDurationReturnsCorrectDifference)
{
  EXPECT_DOUBLE_EQ(TimeUtils::getDuration(100.0, 250.0), 150.0);
  EXPECT_DOUBLE_EQ(TimeUtils::getDuration(0.0, 0.0), 0.0);
  EXPECT_DOUBLE_EQ(TimeUtils::getDuration(300.0, 100.0), -200.0);
}

TEST(TimeUtils, GetCurrentTimeReturnsPositiveValue)
{
  double t1 = TimeUtils::getCurrentTimeAsTime();
  EXPECT_GT(t1, 0.0);

  double t2 = TimeUtils::getCurrentTimeAsTime();
  EXPECT_GE(t2, t1);
}
