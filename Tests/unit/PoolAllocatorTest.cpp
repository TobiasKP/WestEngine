#include <gtest/gtest.h>
#include <PoolAllocator.h>

TEST(PoolAllocator, AllocateReturnsNonNull)
{
  PoolAllocator allocator;
  void* ptr = allocator.allocate(64);
  ASSERT_NE(ptr, nullptr);
  allocator.deallocate(ptr, 64);
}

TEST(PoolAllocator, MultipleAllocationsReturnUniquePointers)
{
  PoolAllocator allocator;
  void* p1 = allocator.allocate(64);
  void* p2 = allocator.allocate(64);
  void* p3 = allocator.allocate(64);

  EXPECT_NE(p1, p2);
  EXPECT_NE(p2, p3);
  EXPECT_NE(p1, p3);

  allocator.deallocate(p1, 64);
  allocator.deallocate(p2, 64);
  allocator.deallocate(p3, 64);
}

TEST(PoolAllocator, DeallocatedChunkIsReused)
{
  PoolAllocator allocator;
  void* p1 = allocator.allocate(64);
  allocator.deallocate(p1, 64);

  void* p2 = allocator.allocate(64);
  EXPECT_EQ(p1, p2) << "Deallocated chunk should be reused on next allocation";
  allocator.deallocate(p2, 64);
}
