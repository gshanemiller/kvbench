#pragma once

#include <cradix_enums.h>
#include <cradix_IterState.h>

#include <assert.h>

namespace CRadix {

class Iterator {
  // DATA
  Node256&                         d_root;         // reference to tree's root
  Node256                         *d_rawNode;      // current node
  u_int8_t                        *d_key;          // current key (owned)
  std::stack<Radix::TreeIterState> d_stack;        // stack to revisit parents
  u_int64_t                        d_attributes;   // current key attributes
  u_int16_t                        d_index;        // current index on current node's d_children attribute
  u_int16_t                        d_depth;        // size-1 in bytes of current key
  const u_int16_t                  d_maxDepth;     // maximum length of key (for debugging)
  bool                             d_end;          // true when no more keys
  bool                             d_jump;         // true when need to resume from a innerNode that's also terminal

  union {                                                                                                               
    Node256  *ptr;    // as pointer                                                                                     
    u_int64_t val;    // as u_int64_t                                                                                   
  } d_memNode, d_nodeHelper; 

public:
  // CREATORS
  Iterator(MemManager *memManager, Node256& root, u_int64_t maxDepth);
    // Create a Iterator on specified 'root' holding keys of at most 'currentMaxDepth' bytes. Specified
    // 'memManager' is used to allocate keyspace

  Iterator(const Iterator& other) = delete;
    // Copy constructor not provided

  ~Iterator();
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

  Iterator& operator=(const Iterator& rhs) = delete;
    // Assignment operator not provided

  // ASPECTS
  std::ostream& print(std::ostream& stream) const;
    // Print into specified 'stream' a human readable representation of the current key returning 'stream'.
    // Behavior is defined provided 'end()==false'
};

// INLINE DEFINITIONS
// ACCESSORS
inline
const u_int8_t *Iterator::key() const {
  assert(!d_end);
  return d_key;
}

inline
u_int16_t Iterator::keySize() const {
  assert(!d_end);
  return d_depth+1;
}

inline
bool Iterator::isCompressed() const {
  assert(!d_end);
  return (d_attributes & k_IS_CHILDREN_COMPRESSED) && d_attributes!=k_IS_LEAF_NODE;
}

inline
bool Iterator::isTerminal() const {
  assert(!d_end);
  return (d_attributes & k_IS_TERMINAL_NODE) || d_attributes==k_IS_LEAF_NODE;
}

inline
bool Iterator::isLeaf() const {
  assert(!d_end);
  return d_attributes == k_IS_LEAF_NODE;
}

inline
bool Iterator::end() const {
  return d_end;
}

} // namespace CRadix
