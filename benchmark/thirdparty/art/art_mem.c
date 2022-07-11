#include <art_mem.h>

#include <stdio.h>
#include <stdlib.h>

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

void art_free_node(art_node*ptr) {
  ++art_freeCount;
  switch(ptr->type) {
    case NODE4:
      art_currentBytes -= (sizeof(art_node4));
      break;
    case NODE16:
      art_currentBytes -= (sizeof(art_node16));
      break;
    case NODE48:
      art_currentBytes -= (sizeof(art_node48));
      break;
    case NODE256:
      art_currentBytes -= (sizeof(art_node256));
      break;
    default:
      abort();
  }
  mi_free(ptr);
}

void art_free_leaf(art_leaf*ptr) {
  ++art_freeCount;
  art_currentBytes -= (sizeof(art_leaf)+ptr->key_len);
  mi_free(ptr);
}

void art_print_memory(void) {
  printf("art memory: free %lu, alloc: %lu, currentBytes: %lu, maxBytes: %lu, requestedBytes: %lu\n",
    art_freeCount, art_allocCount, art_currentBytes, art_maxBytes, art_requestedBytes);
}
