#pragma once

#include <Config.h>
#include <gtest/gtest.h>

// Config keeps a process-wide freed UI id queue. Any test that pushes into it
// without popping everything back out leaks ids into whatever test runs next,
// so a test asserting "next id == previous + 1" passes or fails depending on
// execution order. Drain it around every test that touches it.
class ConfigQueueFixture : public ::testing::Test
{
protected:
  static void drainFreedIds()
  {
    while (Config::freedUiIds.tryPop().has_value()) {}
  }

  void SetUp() override
  {
    drainFreedIds();
  }

  void TearDown() override
  {
    drainFreedIds();
  }
};
