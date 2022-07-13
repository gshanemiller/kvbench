#pragma once

#include <benchmark_slice.h>

#include <assert.h>
#include <iostream>

namespace Radix {

class MemManager;

extern "C" {
  int radix_insert(MemManager *memManager, const u_int8_t *key, u_int16_t size);
}

ennum {
  k_MAX_CHILDREN256 = 256,
  k_IS_INNER_NODE = 1,
  k_IS_INNER_NODE256 = 2,
  k_IS_KEY_COMPRESSED = 4,
  k_IS_CHILDREN_COMPRESSED = 8,
  e_OK = 0,
  e_EXISTS = 1,
  e_NOT_FOUND = 2,
  e_MEMORY_ERROR = 3,
};

struct TreeStats {
  // DATA
  u_int64_t d_innerNodeCount;
  u_int64_t d_leafCount;
  u_int64_t d_emptyChildCount;
  u_int64_t d_maxDepth;
  u_int64_t d_totalSizeBytes;
  u_int64_t d_totalCompressedSizeBytes;

  // CREATORS
  TreeStats();
    // Create stats object with all attributes initialized zero

  ~TreeStats() = default;
    // Destroy this object

  TreeStats(const TreeStats& other) = default;
    // Create stats object s.t. all attributes equal to specified 'other'

  // MANIPULATORS
  void reset();
    // Reset all attributes to 0

  TreeStats& operator=(const TreeStats&rhs) = default;
    // Create and return a copy of specified 'rhs' s.t. all attributes equal.

  // ASPECTS
  std::stream& print(std::ostream& stream) const;
    // Pretty print into specified 'stream' a human readable dump of attributes
    // returning 'stream'
};

// INLINE DEFINITIONS
// CREATORS
inline
TreeStats::TreeStats()
: reset()
{
}

// ASPECTS
inline 
void TreeStats::print(std::stream& stream) {
  stream  << "innerNodeCount: "   << d_innerNodeCount
          << " leafCount: "       << d_leafCount
          << " emptyChildCount: " << d_emptyChildCount
          << " depth: "           << d_depth
          << " totalSizeBytes: "  << d_totalSizeBytes
          << " compressionRatio: "<< d_compressionRatio
          << std::endl;
  return stream;
}

inline
void TreeStats::reset(void) {
  d_innerNodeCount = 0;
  d_leafCount = 0;
  d_emptyChildCount = 0;
  d_maxDepth = 0;
  d_totalSizeBytes = 0;
  d_compressionRatio = 0.0;

  d_depth = 0;
  d_totalSizeBytes = 0;
  d_compressionRatio = 0.0;
}

class Node256 {
  // DATA
  void     *d_children[k_MAX_CHILDREN256];
};

class Tree {
  // DATA
  Node256     *d_root;
  MemManager  *d_memManager;

  // CREATORS
  Tree() = delete;
    // Default constructor not provided

  Tree(MemManager *memManager);
    // Create an empty Radix tree using specified 'memManager' to satisfy
    // memory allocations.

  ~Tree();
    // Destroy this tree deallocating all memory 

  Tree(const Tree& other) = delete;
    // Copy constructor not provided

  // ACCESSORS
  void statistics(TreeStats *stats) const;
    // Compute tree statistics setting result into specified 'stats'.

  int find(const Benchmark::Slice<u_int8_t> key) const;
    // Return 'e_EXISTS' if specified key was found in tree, and 'e_NOT_FOUND'
    // otherwise.

  // MANIUPLATORS
  int insert(const Benchmark::Slice<u_int8_t> key);
    // Return 'e_OK' if specified key was inserted into tree or 'e_EXISTS' if 
    // if key already exists, and otherwise return 'e_MEMORY_ERROR' if there's
    // insufficient memory to perform insertion.

  int remove(const Benchmark::Slice<u_int8_t> key);
    // Return 'e_OK' if specified key was removed from tree or 'e_NOT_FOUND'
    // if key was not found.

  void destroy();
    // Deallocate all memory leaving tree empty

  Tree& operator=(const Tree& rhs);
    // Assignment operator not provided
};

// INLINE DEFINITIONS
// CREATORS
inline
Tree::Tree(MemManager *memManager)
: d_root(0)
, d_memManager(memManager)
{
  assert(memManager!=0);
}

// MANIPULATORS
inline
int Tree::insert(const Benchmark::Slice<u_int8_t> key) {
  return radix_insert(d_memManager, key.data(), key.size());
}

} // namespace Radix
