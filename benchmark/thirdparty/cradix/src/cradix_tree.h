#pragma once

#include <iostream>
#include <assert.h>
#include <sys/types.h>

#include <cradix_constants.h>
#include <cradix_iterator.h>
#include <cradix_treestats.h>

#include <benchmark_slice.h>

namespace CRadix {

struct MemManager;

class Tree {
  // DATA
  MemManager *d_memManager;             // to manage memory
  u_int32_t   d_root;                   // root node offset
  u_int16_t   d_currentMaxDepth;        // max depth of tree

public:
  // CREATORS
  Tree() = delete;
    // Default constructor not provided

  Tree(MemManager *memManager);
    // Create empty Compressed Radix tree using specified 'memManager' for
    // memory management

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
    // Return a in-order read-only key iterator on this tree. It's behavior is
    // defined provided 'insert/remove' not run while in scope.

  // MANIUPLATORS
  int insert(const Benchmark::Slice<u_int8_t> key);
    // Return 'e_OK' if specified key was inserted into tree or 'e_EXISTS' if 
    // if key already exists, and otherwise return 'e_MEMORY_ERROR' if there's
    // insufficient memory to perform insertion.

  int remove(const Benchmark::Slice<u_int8_t> key);
    // Return 'e_OK' if specified key was removed from tree or 'e_NOT_FOUND'
    // if key was not found.

  void destroy();
    // Destory this object and deallocate all memory leaving tree empty

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
    u_int16_t *lastMatchIndex, u_int32_t *lastMatch, u_int32_t *lastMatchParent);
    // Search for specified 'key' of specified 'size' returning 'e_EXISTS'
    // if found, and 'e_NOT_FOUND' otherwise. The behavior is defined provided
    // 'size>0'. 'lastMatchIndex, lastMatch, lastMatchParent' are set and defined
    // only when 'e_NOT_FOUND' is returned. In that case '0<=lastMatchIndex<size'
    // is set to the last byte matched in key, 'lastMatch' points to the node
    // offset in which 'key[*lastMatchIndex]' terminates. 'lastMatchParent' is
    // 'lastMatch's parent.

  u_int32_t reallocateAndLink(Node256 *parentPtr, u_int8_t byte, int32_t min, int32_t max, u_int32_t child);
    // Copy-reallocate the specified 'child' offset of 'parentPtr' node at
    // specified 'byte' so it has sufficient capacity to hold its current
    // entries plus all offsets in '[min, max]' returing the offset to the
    // new node or 0 if there's no memory. Behavior is defined provided
    // 'byte<k_MAX_CHILDREN', 'parentPtr->tryOffset(byte)==child'. Note that
    // the memory at 'child' is marked dead by memory manager. Also note that
    // 'min, max' are calculated by 'canSetOffset'. See example below.
    //
    // Typical call flow appears below in pseudo code matching this cartoon:
    // 
    // Before:
    // +------------+      +-------+
    // | parent@'D' |----> | child | 0x1234 (offset 1200)
    // +------------+      +-------+ min/max ['a', 'c']
    // offset('D')=1200
    //
    // Now suppose we need to add 'z' to child e.g. edge 'D'->'z'. If there's
    // not enough memory it has to be reallocated, and the pointer to it updated:
    //
    // After:
    // +------------+      +-------+
    // | parent@'D' |----> | child | 0x5678 (offset 5000)
    // +------------+      +-------+ min/max ['a', 'c']
    // offset('D')=5000              but now has capacity to set 'z'
    //                     +-------+
    //                     | child | 0x1234 (offset 1200)
    //                     +-------+ **DEAD** marked for reclamation
    //
    // int32_t newMin, newMax;
    // # Try to set 'z' to 555 in child:
    // if (!childPtr->trySetOffset('z', 555, newMin, newMax)) {
    //   # false returned so childPtr didn't have enough capacity
    //   # copy-reallocate and relink using newMin, newMax calc'd
    //   # for us by trySetOffset. Here 'childOffset' is 'childPtr'
    //   # in its offset form. parentPtr is the pointer form of the
    //   # parent offset holding child:
    //   u_int32_t newOffset = reallocateAndLink(parentPtr, 'D', min, max, childOffset);
    //   # make sure 0 not returned implies out of memory
    //   assert(newOffset!=0);
    //   # convert newOffset to pointer. alternative: d_memManager->ptr(newOffset)
    //   childPtr = (Node256*)(d_memManager->basePtr()+newOffset);
    //   # make assignment as intended
    //   childPtr->setOffset('z', 555);
    //   printf("offset @ 'z' set to 555 after reallocation");
    // } else {
    //   # true returned. assignment was done
    //   printf("offset @ 'z' set to 555");
    // }
};

// INLINE DEFINITIONS
// CREATORS
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
int Tree::find(const Benchmark::Slice<u_int8_t> key) const {
  return findHelper(key.data(), key.size());
}

} // namespace Radix
