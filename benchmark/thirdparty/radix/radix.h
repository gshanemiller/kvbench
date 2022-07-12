#pragma once

#include <benchmark_slice.h>

#include <iostream>

namespace Radix {

ennum {
  k_MAX_CHILDREN16 = 16,
  k_MAX_CHILDREN = 256,
  k_MAX_BITS = 256,
  k_IS_INNER_NODE = 1,
  k_IS_INNER_NODE16 = 2,
  k_IS_KEY_COMPRESSED = 4,
  k_IS_CHILDREN_COMPRESSED = 8,
  e_OK = 0,
  e_EXISTS = 1,
  e_NOT_FOUND = 2,
  e_MEMORY_ERROR = 3,
};

struct Stats {
  // DATA
  u_int64_t d_innerNodeCount;
  u_int64_t d_leafCount;
  u_int64_t d_emptyChildCount;
  u_int64_t d_depth;
  u_int64_t d_tailSizeBytes;
  u_int64_t d_totalSizeBytes;
  double    d_compressionRatio;

  // CREATORS
  Stats();
    // Create stats object with all attributes initialized zero

  ~Stats() = default;
    // Destroy this object

  Stats(const Stats& other) = default;
    // Create stats object s.t. all attributes equal to specified 'other'

  // MANIPULATORS
  void reset();
    // Reset all attributes to 0

  Stats& operator=(const Stats&rhs) = default;
    // Create and return a copy of specified 'rhs' s.t. all attributes equal.

  // ASPECTS
  void print(std::ostream& stream) const;
    // Pretty print into specified 'stream' a human readable dump of attributes;
};

// INLINE DEFINITIONS
inline
Stats::Stats()
{
  reset();
}

inline
void Stats::reset(void) {
  d_innerNodeCount = 0;
  d_leafCount = 0;
  d_emptyChildCount = 0;
  d_depth = 0;
  d_tailSizeBytes = 0;
  d_totalSizeBytes = 0;
  d_compressionRatio = 0.0;
}

class Node16 {
  // DATA
  u_int16_t d_bitKeys;
  u_int32_t d_children[];
  
  // STATIC ASSERTS
  static_assert(sizeof(d_bitKeys)==2);
};

class Node256 {
  // DATA
  u_int64_t d_bitKeys[k_MAX_BITS>>3];
  u_int32_t d_children[];

  // STATIC ASSERTS
  static_assert(sizeof(d_bitKeys) == (k_MAX_BITS>>3));
};

class Leaf {
  u_int16_t d_tailSize;
  u_int8_t  d_data[];
};

class Tree {
  // DATA
  void *d_root;

  // CREATORS
  Tree();
    // Create an empty Radix tree

  ~Tree();
    // Destroy this tree deallocating all memory 

  Tree(const Tree& other) = delete;
    // Copy constructor not provided

  // ACCESSORS
  void statistics(Stats *stats) const;
    // Compute tree statistics setting result into specified 'stats'.

  int find(Benchmark::Slice<u_int8_t> key) const;
    // Return 'e_EXISTS' if specified key was found in tree, and 'e_NOT_FOUND'
    // otherwise.

  // MANIUPLATORS
  int insert(Benchmark::Slice<u_int8_t> key);
    // Return 'e_OK' if specified key was inserted into tree or 'e_EXISTS' if 
    // if key already exists, and otherwise return 'e_MEMORY_ERROR' if there's
    // insufficient memory to perform insertion.

  int delete(Benchmark::Slice<u_int8_t> key);
    // Return 'e_OK' if specified key was removed from tree or 'e_NOT_FOUND'
    // if key was not found.

  void destroy();
    // Deallocate all memory leaving tree empty

  Tree& operator=(const Tree& rhs);
    // Assignment operator not provided
};

} // namespace Radix
