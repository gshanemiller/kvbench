#include <art_mem.h>

#include <stdio.h>

#include <mimalloc.h>

u_int64_t art_freeCount = 0;
u_int64_t art_allocCount = 0;
u_int64_t art_currentBytes = 0;
u_int64_t art_maxBytes = 0;
u_int64_t art_requestedBytes = 0;

void *art_calloc(u_int64_t size) {
  ++art_allocCount;
  art_currentBytes += size;
  art_requestedBytes += size;
  if (art_currentBytes>art_maxBytes) {
    art_maxBytes = art_currentBytes;
  }
  void *ptr = mi_calloc_aligned(1, size, 2);
  return ptr;
}

void *art_malloc(u_int64_t size) {
  ++art_allocCount;
  art_currentBytes += size;
  art_requestedBytes += size;
  if (art_currentBytes>art_maxBytes) {
    art_maxBytes = art_currentBytes;
  }
  void *ptr = mi_malloc_aligned(size, 2);
  return ptr;
}

void art_free(void *ptr) {
  ++art_freeCount;
  mi_free(ptr);
}

void art_print_memory(void) {
  printf("art memory: free %lu, alloc: %lu, currentBytes: %lu, maxBytes: %lu, requestedBytes: %lu\n",
    art_freeCount, art_allocCount, art_currentBytes, art_maxBytes, art_requestedBytes);
}
