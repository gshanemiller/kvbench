#pragma once

#include <assert.h>
#include <iostream>

#include <benchmark_slice.h>
#include <radix_enums.h>

namespace Radix {

class MemManager;

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
  double ratio(0);

  if (d_totalUncompressedSizeBytes!=0) {
    ratio = static_cast<double>(d_totalCompressedSizeBytes) /
            static_cast<double>(d_totalUncompressedSizeBytes);
  }

  double bytesPerInnerNode = static_cast<double>(d_totalSizeBytes)/static_cast<double>(d_innerNodeCount);

  stream  << "innerNodeCount: "               << d_innerNodeCount
          << " leafCount: "                   << d_leafCount
          << " emptyChildCount: "             << d_emptyChildCount
          << " maxDepth: "                    << d_maxDepth
          << " totalSizeBytes: "              << d_totalSizeBytes
          << " totalCompressedSizeBytes: "    << d_totalCompressedSizeBytes
          << " totalUncompressedSizeBytes: "  << d_totalUncompressedSizeBytes
          << " compressionRatio: "            << ratio
          << " totalSizeBytes-per-innerNode: "<< bytesPerInnerNode
          << " sizeof(Node256): "             << sizeof(Node256)
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

class Tree {
  // DATA
  MemManager  *d_memManager;
  Node256     d_root;

public:
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

  void dotGraph(std::ostream& stream) const;
    // Print into specified 'stream' a AT&T dot graph representation of tree

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
  // PRIAVTE ACCESSORS
  int findHelper(const u_int8_t *key, const u_int16_t size) const;
    // Search for specified 'key' of specified 'size' returning 'e_EXISTS'
    // if found, and 'e_NOT_FOUND' otherwise. The behavior is defined provided
    // 'size>0'.

  // PRIVATE MANIPULATORS
  int insertHelper(const u_int8_t *key, const u_int16_t size,
    u_int16_t *lastMatchIndex, Node256 **lastMatch);
    // Search for specified 'key' of specified 'size' returning 'e_EXISTS'
    // if found, and 'e_NOT_FOUND' otherwise. The behavior is defined provided
    // 'size>0', and 'lastMatchIndex, lastMatch' are defined only when
    // 'e_NOT_FOUND' is returned. In that case '0<=lastMatchIndex<size' is set
    // to the last byte matched in key, and '*lastMatch' points to the node in
    // which the last byte match was found. Note that in the special case the
    // prefix already exists in the tree, but the node containing the last
    // byte of 'key' is not tagged terminal, it is so tagged and 'e_EXISTS' is
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

inline
Tree::~Tree() {
}

// MANIPULATORS
inline
int Tree::find(const Benchmark::Slice<u_int8_t> key) const {
  return findHelper(key.data(), key.size());
}

struct TreeIterState {
  Node256  *d_node;
  u_int16_t d_index;
  u_int16_t d_depth;

  TreeIterState() = delete;
    // Default constructor not provided

  TreeIterState(Node256 *node, u_int16_t index, u_int16_t depth);
    // Construct TreeIterState with specified arguments
};

// INLINE DEFINITIONS
// CREATORS

inline
TreeIterState::TreeIterState(Node256 *node, u_int16_t index, u_int16_t depth)
: d_node(node)
, d_index(index)
, d_depth(depth)
{
  assert(node!=0);
}

} // namespace Radix
