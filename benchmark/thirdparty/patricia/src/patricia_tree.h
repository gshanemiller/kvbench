#pragma once

#include <mimalloc.h>
#include <benchmark_slice.h>

namespace Patricia {

typedef struct {
  void *root;
} Tree;

typedef struct {
  void      *child[2];    // children
  u_int16_t diffIndex;    // offset into key(s) where diff starts
  u_int8_t  diffMask;     // bit mask at byte location of difference
} InternalNode;

enum Errno {
  e_OK        = 0,
  e_NOT_FOUND = 1,
  e_EXISTS    = 2,
  e_MEMORY    = 3,
};

extern void destroyTree(Tree *t);
extern int  insertKey(Tree *t,   const Benchmark::UKey key);
extern int  deleteKey(Tree *t,   const Benchmark::UKey key);
extern int  findKey(Tree *t,     const Benchmark::UKey key);

class MemoryManager {
  // DATA
  u_int64_t       d_freeCount;
  u_int64_t       d_allocCount;
  u_int64_t       d_currentBytes;
  u_int64_t       d_maxBytes;
  u_int64_t       d_requestedBytes;

public:
  // CREATORS
  MemoryManager();
    // Create an object to allocate/free memory for Patricia tree's via mimalloc

  MemoryManager(const MemoryManager& other);
    // Copy constructor not provided

  ~MemoryManager() = default;
    // Note that Patricia trees must be destoyed beforr this object goes out of scope

  // MANIPULATORS
  void *allocInternalNode();
    // Return a pointer to memory for a new InternalNode. Note that it is not initialized.

  void *allocTree();
    // Return a pointer to memory for a new Tree. Note that it is not initialized. 

  void free(void *ptr);
    // Free memory previously allocated by this object

  void print();
    // Print to stdout highlevel statistics on memory work

  const MemoryManager& operator=(const MemoryManager& rhs) = delete;
    // Assignment operator not provided
};

// INLINE DEFINITIONS

// CREATORS
inline
MemoryManager::MemoryManager()
: d_freeCount(0)
, d_allocCount(0)
, d_currentBytes(0)
, d_maxBytes(0)
, d_requestedBytes(0)
{
} 

// MANIPULATORS
inline
void *MemoryManager::allocInternalNode() {
  ++d_allocCount;
  d_currentBytes += sizeof(InternalNode);
  d_requestedBytes += sizeof(InternalNode);
  if (d_currentBytes>d_maxBytes) {
    d_maxBytes = d_currentBytes;
  }
  return mi_malloc_aligned(sizeof(InternalNode), sizeof(void*));
}

inline
void *MemoryManager::allocTree() {
  return mi_malloc_aligned(sizeof(Tree), sizeof(void*));
}

inline
void MemoryManager::free(void* ptr) {
  ++d_freeCount;
  d_currentBytes -= sizeof(InternalNode);
  mi_free(ptr);
}

inline
void MemoryManager::print() {
  printf("allocCount: %lu, freeCount: %lu, currentBytes: %lu, maxBytes: %lu, requestedBytes: %lu\n",
    d_allocCount, d_freeCount, d_currentBytes, d_maxBytes, d_requestedBytes);
}

extern MemoryManager memManager;

}
