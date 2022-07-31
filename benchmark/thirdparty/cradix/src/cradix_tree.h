#pragma once

#include <iostream>
#include <assert.h>
#include <sys/types.h>

#include <cradix_constants.h>
#include <cradix_iterator.h>

#include <benchmark_slice.h>

namespace CRadix {

struct Node256;
struct MemManager;

class Tree {
  // DATA
  MemManager *d_memManager;
  Node256    *d_root;
  u_int64_t   d_currentMaxDepth;

public:
  // CREATORS
  Tree() = delete;
    // Default constructor not provided

  Tree(MemManager *memManager);
    // Create empty Compressed Radix tree using specified 'memManager' for
    // memory management.

  ~Tree();
    // Destroy this tree deallocating all its memory 

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

  Iterator begin() const;
    // Return a in-order key iterator on this tree. It's behavior is defined
    // provided 'insert/remove' not run while in scope.

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
    // 'size>0'. lastMatchIndex, lastMatch, depth' are defined only when
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
Iterator Tree::begin() const {
  return Iterator(const_cast<MemManager*>(d_memManager), const_cast<Node256&>(d_root), currentMaxDepth());
}

// MANIPULATORS
inline
int Tree::find(const Benchmark::Slice<u_int8_t> key) const {
  return findHelper(key.data(), key.size());
}

} // namespace Radix
