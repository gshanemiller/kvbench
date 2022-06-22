#include <gtest/gtest.h>
#include <mimalloc.h>
#include <stdio.h>

const unsigned long SIZE = 512;

TEST(mimalloc, allocateHeap) {
  mi_stats_reset();
  mi_heap_t *heap = mi_heap_new();
  EXPECT_TRUE(heap!=0);
  void *base = mi_heap_malloc_aligned(heap, SIZE, 8);
  unsigned long b = reinterpret_cast<unsigned long>(base);
  unsigned long mask1 = ~(0xFFFFFFFFFFFFUL);
  unsigned long mask2 = ~(0xFFFFFFFFFF);
  printf("%p\n", base);
  for (unsigned i=0; i<1000; ++i) {
    void *ptr= mi_heap_malloc_aligned(heap, SIZE, 8);
  }
  mi_heap_destroy(heap);
  return;
}
