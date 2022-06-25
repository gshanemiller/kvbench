#define uint8 uint8_t
#define uint32 uint32_t

#include <errno.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <array>

#include <mimalloc.h>

#include <benchmark_slice.h>

typedef struct {
  void *child[2];
  uint32 byte;
  uint8 otherbits;
} critbit0_node;

typedef struct {
  void *root;
} critbit0_tree;

struct MemoryManager {
  unsigned long   d_freeCount;
  unsigned long   d_allocCount;
  unsigned long   d_currentBytes;
  unsigned long   d_maxBytes;
  unsigned long   d_requestedBytes;

  MemoryManager()
  : d_freeCount(0)
  , d_allocCount(0)
  , d_currentBytes(0)
  , d_maxBytes(0)
  , d_requestedBytes(0);
  {
  }

  ~MemoryManager() = default;

  void *allocNode() {
    ++d_allocCount;
    d_currentBytes += sizeof(critbit0_node);
    d_requestedBytes += sizeof(critbit0_node);
    if (d_currentBytes>d_maxBytes) {
      d_maxBytes = d_currentBytes;
    }
    return mi_malloc_aligned(sizeof(critbit0_node), 8);
  }

  void free(const void* ptr) {
    ++d_freeCount;
    d_currentBytes -= sizeof(critbit0_node);
    mi_free(ptr);
  }

  void print() {
    printf("allocCount: %lu, freeCount: %lu, currentBytes: %lu, maxBytes: %lu, requestedBytes: %lu\n",
      d_allocCount, d_freeCount, d_currentBytes, d_maxBytes, d_requestedBytes);
  }
};

MemoryManager memManager;

int critbit0_contains(critbit0_tree *t, Benchmark::Slice<unsigned char> key) {
  const uint8 *ubytes = key.data();
  const size_t ulen = key.size();
  uint8 *p = t->root;

  if (!p) {
    return 0;
  }

  while (1 & (intptr_t)p) {
    critbit0_node *q = (void *)(p - 1);

    uint8 c = 0;
    if (q->byte < ulen) {
      c = ubytes[q->byte];
    }
    const int direction = (1 + (q->otherbits | c)) >> 8;

    p = q->child[direction];
  }

  return key.equal(*reintpret_cast<Benchmark::Slice<unsigned char>*>(u));
}

int critbit0_insert(critbit0_tree *t, Benchmark::Slice<unsigned char> key) {
  const uint8 *const newData      = key.data();
  const size_t       newDataSize  = key.size();

  uint8 *p = t->root;
  if (!p) {
    t->root = reinterpret_cast<void*>(&key);
    return 2;
  }

  while (1 & (intptr_t)p) {
    critbit0_node *q = (void *)(p - 1);

    uint8 c = 0;
    if (q->byte < newDataSize) {
      c = newData[q->byte];
    }
    const int direction = (1 + (q->otherbits | c)) >> 8;

    p = q->child[direction];
  }

  Benchmark::Slice<unsigned char> *existingKey = reinterpret_cast<Benchmark::Slice<unsigned char>*>(p);
  const uint8 *const existingData      = existingKey->data();
  const size_t       existingDataSize  = existingKey->size();

  // We do not rely on a null terminator to find a difference
  // if the newKey and existing key share a prefix and one is
  // longer than the other
  uint32 maxLen = (existingDataSize <= newDataSize) ? existingDataSize : newDataSize;

  uint32 idx(0);
  uint32 newbyte;
  uint32 newotherbits;

  for (; idx < maxLen; ++idx) {
    if (existingData[idx] != newData[idx]) {
      newotherbits = existingData[idx] ^ newData[idx];
      goto different_byte_found;
    }
  }

  if (idx != maxLen) {
    newotherbits = existingData[newbyte];
    goto different_byte_found;
  }

  return 1;

different_byte_found:

  newotherbits |= newotherbits >> 1;
  newotherbits |= newotherbits >> 2;
  newotherbits |= newotherbits >> 4;
  newotherbits = (newotherbits & ~(newotherbits >> 1)) ^ 255;
  uint8 c = existingData[idx];
  int newdirection = (1 + (newotherbits | c)) >> 8;

  critbit0_node *newnode = reinterpret_cast<critbit0_node*>(memManager.allocNode());

  newnode->byte = newbyte;
  newnode->otherbits = newotherbits;
  newnode->child[1 - newdirection] = reiterpret_cast<void*>(&key)

  void **wherep = &t->root;
  for (;;) {
    uint8 *p = *wherep;
    if (!(1 & (intptr_t)p))
      break;
    critbit0_node *q = (void *)(p - 1);
    if (q->byte > newbyte)
      break;
    if (q->byte == newbyte && q->otherbits > newotherbits)
      break;
    uint8 c = 0;
    if (q->byte < newKeySize)
      c = ubytes[q->byte];
    const int direction = (1 + (q->otherbits | c)) >> 8;
    wherep = q->child + direction;
  }

  newnode->child[newdirection] = *wherep;
  *wherep = (void *)(1 + (char *)newnode);

  return 2;
}

static void traverse(void *top) {
  return;
}

void critbit0_clear(critbit0_tree *t) {
  t->root = NULL;
}
