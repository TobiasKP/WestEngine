#include <gtest/gtest.h>
#include <Entity/Entity.h>

struct TestComponent : public IComponent
{
  int value = 42;
};

TEST(Entity, AddAndGetComponentRoundTrip)
{
  Entity entity(1);
  TestComponent comp;
  comp.value = 99;

  entity.addComponent(BitMasks::Components::POSITION, &comp);

  IComponent* result = entity.getComponent(BitMasks::Components::POSITION);
  ASSERT_NE(result, nullptr);
  EXPECT_EQ(static_cast<TestComponent*>(result)->value, 99);
}

TEST(Entity, GetComponentReturnsNullForMissing)
{
  Entity entity(2);
  IComponent* result = entity.getComponent(BitMasks::Components::MODEL);
  EXPECT_EQ(result, nullptr);
}
