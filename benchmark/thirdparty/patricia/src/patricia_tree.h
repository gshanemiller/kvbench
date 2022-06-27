#pragma once

#include <benchmark_typedefs.h>
#include <mimalloc.h>

namespace Patricia {

typedef struct {
  void *root;
} Tree;

typedef struct {
  void      *child[2];    // children
  u_int16_t diffIndex;    // offset into key(s) where diff starts
  u_int8_t  diffMask;     // bit mask for bit location of difference
} InternalNode;

enum Errno {
  e_OK        = 0,
  e_NOT_FOUND = 1,
  e_EXISTS    = 2,
  e_MEMORY    = 3,
};

extern void destroy(PatriciaTree *t);
extern int  insert(PatriciaTree *t,   const Benchmark::Key *key);
extern int  delete(PatriciaTree *t,   const Benchmark::Key *key);
exterm bool find(PatriciaTree *t,     const Benchmark::Key *key);

class MemoryManager {
  // DATA
  u_int64_t       d_freeCount;
  u_int64_t       d_allocCount;
  u_int64_t       d_currentBytes;
  u_int64_t       d_maxBytes;
  u_int64_t       d_requestedBytes;

  MemoryManager();
    // Create an object to allocate/free memory for Patricia tree's via mimalloc

  MemoryManager(const MemoryManager& other);
    // Copy constructor not provided

  ~MemoryManager() = default;
    // Note that Patricia trees must be destoyed beforr this object goes out of scope

  void *allocInternalNode()
    // Return a pointer to memory for a new InternalNode. Note that it is not initialized.

  void *allocTree();
    // Return a pointer to memory for a new Tree. Note that it is not initialized. 

  void free(const void* ptr);
    // Free memory previously allocated by this object

  void print() {
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
, d_requestedBytes(0);
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

void *MemoryManager::allocTree() {
  ++d_allocCount;
  d_currentBytes += sizeof(Tree);
  d_requestedBytes += sizeof(Tree);
  if (d_currentBytes>d_maxBytes) {
    d_maxBytes = d_currentBytes;
  }
  return mi_malloc_aligned(sizeof(Tree), sizeof(void*));
}

inline
void MemoryManager::free(const void* ptr) {
  ++d_freeCount;
  d_currentBytes -= d_size;
  mi_free(ptr);
}

inline
void MemoryManager::print() {
  printf("allocCount: %lu, freeCount: %lu, currentBytes: %lu, maxBytes: %lu, requestedBytes: %lu\n",
    d_allocCount, d_freeCount, d_currentBytes, d_maxBytes, d_requestedBytes);
}

};

extern MemoryManager memManager;

}
