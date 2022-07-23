#pragma once

// PURPOSE: 8bit-ary Radix tree over keys of type 'Benchmark::Slice<unsigned char>'
//
// CLASSES:
//  Radix::Tree: Holds root of Radix tree with APIs for insert, find, remove, iteration
//  Radix::Node256: Radix inner node with 2^8=256 children
//  Radix::TreeStats: Summarizing stats over Radix tree e.g. counts, depth, size
//  Radix::TreeIterState: Pre-order traversal helper state for Radix tree
//  Radix::TreeIterator: Pre-order traversal of tree key-by-key
//

#include <stack>
#include <iostream>

#include <assert.h>

#include <radix_enums.h>
#include <benchmark_slice.h>

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

class TreeIterator {
  // DATA
  MemManager                      *d_memManager;   // for key allocation management
  Node256&                         d_root;         // reference to tree's root
  Node256                         *d_rawNode;      // current node
  u_int8_t                        *d_key;          // holds key (owned)
  std::stack<Radix::TreeIterState> d_stack;        // stack to revisit parents
  u_int64_t                        d_attributes;   // current key attributes
  u_int16_t                        d_index;        // current index on current node's d_children attribute
  u_int16_t                        d_depth;        // size-1 in bytes of current key
  const u_int16_t                  d_maxDepth;     // maximum length of key (for debugging)
  bool                             d_end;          // true when no more keys

  union {                                                                                                               
    Node256  *ptr;    // as pointer                                                                                     
    u_int64_t val;    // as u_int64_t                                                                                   
  } d_memNode, d_nodeHelper; 

public:
  // CREATORS
  TreeIterator(MemManager *memManager, Node256& root, u_int64_t maxDepth);
    // Create a TreeIterator on specified 'root' holding keys of at most 'currentMaxDepth' bytes. Specified
    // 'memManager' is used to allocate keyspace

  TreeIterator(const TreeIterator& other) = delete;
    // Copy constructor not provided

  ~TreeIterator();
    // Destory this object

  // ACCESSORS
  const u_int8_t *key() const;
    // Return the current key. Behavior is defined provided 'end()==false'

  u_int16_t keySize() const;
    // Return the size of the current key in bytes. Behavior is defined provided 'end()==false'

  bool isCompressed() const;
    // Return 'true' if current node is compressed. Behavior is defined provided 'end()==false'

  bool isTerminal() const;
    // Return 'true' if current node is a terminal node or leaf node. Behavior is defined provided 'end()==false'

  bool isLeaf() const;
    // Return 'true' if current node is a leaf node. Behavior is defined provided 'end()==false'

  bool end() const;
    // Return 'true' if all keys have been iterated over

  // MANIPULATORS
  void next();
    // Advance to the next key or set 'end()==true' if no more keys. Caller must ensure 'end()==false' after
    // call returns before invoking accessors

  TreeIterator& operator=(const TreeIterator& rhs) = delete;
    // Assignment operator not provided

  // ASPECTS
  std::ostream& print(std::ostream& stream) const;
    // Print into specified 'stream' a human readable representation of the current key returning 'stream'.
    // Behavior is defined provided 'end()==false'
};

// INLINE DEFINITIONS
// ACCESSORS
inline
const u_int8_t *TreeIterator::key() const {
  assert(!d_end);
  return d_key;
}

inline
u_int16_t TreeIterator::keySize() const {
  assert(!d_end);
  return d_depth+1;
}

inline
bool TreeIterator::isCompressed() const {
  assert(!d_end);
  return (d_attributes & k_IS_CHILDREN_COMPRESSED) && d_attributes!=k_IS_LEAF_NODE;
}

inline
bool TreeIterator::isTerminal() const {
  assert(!d_end);
  return (d_attributes & k_IS_TERMINAL_NODE) || d_attributes==k_IS_LEAF_NODE;
}

inline
bool TreeIterator::isLeaf() const {
  assert(!d_end);
  return d_attributes == k_IS_LEAF_NODE;
}

inline
bool TreeIterator::end() const {
  return d_end;
}

class Tree {
  // DATA
  MemManager  *d_memManager;
  Node256     d_root;
  u_int64_t   d_currentMaxDepth;

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
  u_int64_t currentMaxDepth() const;
    // Return the size in bytes of the maximum sized key in tree

  void statistics(TreeStats *stats) const;
    // Compute tree statistics setting result into specified 'stats'.

  void dotGraph(std::ostream& stream) const;
    // Print into specified 'stream' a AT&T dot graph representation of tree

  int find(const Benchmark::Slice<u_int8_t> key) const;
    // Return 'e_EXISTS' if specified key was found in tree, and 'e_NOT_FOUND'
    // otherwise.

  TreeIterator begin() const;
    // Return a in-order key iterator on this tree. It's behavior is defined
    // provided 'insert/delete' not run while in scope.

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
    // 'size>0', and 'lastMatchIndex, lastMatch, depth' are defined only when
    // 'e_NOT_FOUND' is returned. In that case '0<=lastMatchIndex<size' is set
    // to the last byte matched in key, and '*lastMatch' points to the node in
    // which the last byte match was found. Note that in the special case the
    // prefix already exists in the tree, but the node containing the last byte
    // of 'key' is not tagged terminal, it is so tagged and 'e_EXISTS' is
    // returned.
};

// INLINE DEFINITIONS
// CREATORS
inline
Tree::Tree(MemManager *memManager)
: d_memManager(memManager)
, d_currentMaxDepth(0)
{
  assert(memManager!=0);
}

inline
Tree::~Tree() {
  destroy();
}

// ACCESSORS
inline
u_int64_t Tree::currentMaxDepth() const {
  return d_currentMaxDepth;
}

inline
TreeIterator Tree::begin() const {
  return TreeIterator(const_cast<MemManager*>(d_memManager), const_cast<Node256&>(d_root), currentMaxDepth());
}

// MANIPULATORS
inline
int Tree::find(const Benchmark::Slice<u_int8_t> key) const {
  return findHelper(key.data(), key.size());
}

} // namespace Radix
