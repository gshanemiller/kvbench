#pragma once

#include <assert.h>
#include <iostream>

#include <benchmark_slice.h>
#include <radix_enums.h>

namespace Radix {

class MemManager;

struct TreeStats {
  // DATA
  u_int64_t d_innerNodeCount;
  u_int64_t d_leafCount;
  u_int64_t d_emptyChildCount;
  u_int64_t d_maxDepth;
  u_int64_t d_totalSizeBytes;
  u_int64_t d_totalCompressedSizeBytes;
  u_int64_t d_totalUncompressedSizeBytes;

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
  std::ostream& print(std::ostream& stream) const;
    // Pretty print into specified 'stream' a human readable dump of attributes
    // returning 'stream'
};

// INLINE DEFINITIONS
// CREATORS
inline
TreeStats::TreeStats()
{
  reset();
}

// ASPECTS
inline 
std::ostream& TreeStats::print(std::ostream& stream) const {
  double ratio = static_cast<double>(d_totalCompressedSizeBytes) /
                 static_cast<double>(d_totalUncompressedSizeBytes);
  stream  << "innerNodeCount: "               << d_innerNodeCount
          << " leafCount: "                   << d_leafCount
          << " emptyChildCount: "             << d_emptyChildCount
          << " maxDepth: "                    << d_maxDepth
          << " totalSizeBytes: "              << d_totalSizeBytes
          << " totalCompressedSizeBytes: "    << d_totalCompressedSizeBytes
          << " totalUncompressedSizeBytes: "  << d_totalUncompressedSizeBytes
          << " compressionRatio: "            << ratio
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
  d_totalCompressedSizeBytes = 0;
  d_totalUncompressedSizeBytes = 0;
}

struct Node256 {
  // DATA
  Node256 *d_children[k_MAX_CHILDREN256];

  // CREATORS
  Node256();
    // Create a Node256 object with all children zeroed

  Node256(const Node256& other) = delete;
    // Copy constructor not provided

  ~Node256() = default;
    // Destory this object. Note, destructor does not deallocate memory. Those
    // roles and responsibilities are done in 'Tree::remove' or 'Tree::destroy'

  // MANIPULATORS
  Node256& operator=(const Node256& rhs) = delete;
    // Assignment opertor not provided
};

// INLINE DEFINITIONS
inline
Node256::Node256()
{
  memset(d_children, 0, sizeof(Node256 *)*k_MAX_CHILDREN256);
}

class Tree {
  // DATA
  MemManager  *d_memManager;
  Node256     d_root;

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

  Tree& operator=(const Tree& rhs) = delete;
    // Assignment operator not provided

private:
  // PRIVATE ACCESSORS
  int internalFind(const u_int8_t *key, const u_int16_t size,
    u_int16_t *lastMatchIndex, Node256 **lastMatch) const;
    // Search for specified 'key' of specified 'size' returning 'e_EXISTS'
    // if found, and 'e_NOT_FOUND' otherwise. The behavior is defined provided
    // 'size>0'. Upon return 'lastMatchIndex' is set to the last character
    // matched, and '*lastMatch' points to the node in which the last match was
    // found. That is, if 'e_EXISTS' is returned '*lastMatchIndex==size' and
    // '*lastMatch' points to the leaf containing last character in 'key'.
    // Otherwise 'e_NOT_FOUND' was returned and '*lastMatch' contains the node
    // in which the highest byte '0<=*lastMatchIndex<size' was matched. If this
    // call co-occurs with 'insert' the output data can be used to recursively
    // insert the rest of the key under '*lastMatch' when 'e_NOT_FOUND' is
    // returned.
};

// INLINE DEFINITIONS
// CREATORS
inline
Tree::Tree(MemManager *memManager)
: d_memManager(memManager)
{
  assert(memManager!=0);
}

} // namespace Radix
