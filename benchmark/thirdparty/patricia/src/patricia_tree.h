#pragma once

#include <mimalloc.h>
#include <benchmark_slice.h>
#include <vector>

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

extern void dotTree(Tree *t);
extern void allKeysSorted(Tree *t, std::vector<Benchmark::UKey>& leaf);

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
  InternalNode *allocInternalNode();
    // Return a pointer to memory for a new InternalNode. Note, only the children are iniialized 0.

  Tree *allocTree();
    // Return a pointer to memory for a new Tree. Node, 'root' attribute initialized 0.

  void freeInternalNode(InternalNode *ptr);
    // Free memory previously allocated by 'allocInternalNode()'

  void freeTree(Tree *ptr);
    // Free memory previously allocated by 'allocTree()'

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
InternalNode *MemoryManager::allocInternalNode() {
  ++d_allocCount;
  d_currentBytes += sizeof(InternalNode);
  d_requestedBytes += sizeof(InternalNode);
  if (d_currentBytes>d_maxBytes) {
    d_maxBytes = d_currentBytes;
  }
  InternalNode *ptr = reinterpret_cast<InternalNode*>(mi_malloc_aligned(sizeof(InternalNode), sizeof(void*)));
  assert(ptr);
  ptr->child[0] = ptr->child[1] = 0;
  return ptr;
}

inline
Tree *MemoryManager::allocTree() {
  Tree *ptr = reinterpret_cast<Tree*>(mi_malloc_aligned(sizeof(Tree), sizeof(void*)));
  assert(ptr);
  ptr->root = 0;
  return ptr;
}

inline
void MemoryManager::freeInternalNode(InternalNode *ptr) {
  ++d_freeCount;
  d_currentBytes -= sizeof(InternalNode);
  mi_free(ptr);
}

inline
void MemoryManager::freeTree(Tree *ptr) {
  mi_free(ptr);
}

inline
void MemoryManager::print() {
  printf("allocCount: %lu, freeCount: %lu, currentBytes: %lu, maxBytes: %lu, requestedBytes: %lu\n",
    d_allocCount, d_freeCount, d_currentBytes, d_maxBytes, d_requestedBytes);
}

}
