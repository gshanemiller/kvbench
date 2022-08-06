#pragma once

#include <cradix_constants.h>
#include <cradix_node256.h>
#include <cradix_iterstate.h>

#include <iostream>

#include <assert.h>

#include <stack>

namespace CRadix {

class Iterator {
  // DATA
  std::stack<IterState> d_stack;        // stack to revisit parents
  u_int8_t             *d_basePtr;      // point to start of memory containing root
  u_int8_t             *d_key;          // current key (owned)
  Node256              *d_nodePtr;      // d_node as valid pointer
  u_int32_t             d_root;         // offset to tree's root
  u_int32_t             d_node;         // offset to current node
  u_int32_t             d_childNode;    // offset of current child of node
  u_int32_t             d_attributes;   // current key attributes
  u_int16_t             d_index;        // current index on current node's children offset array
  u_int16_t             d_maxIndex;     // the maximum valid index on current node's children offet array
  u_int16_t             d_depth;        // current tree depth & size of current key minus 1 in bytes
  const u_int16_t       d_maxDepth;     // maximum length of key
  bool                  d_end;          // true when no more keys
  bool                  d_jump;         // true when resuming from terminal inner node

public:
  // CREATORS
  Iterator(u_int32_t root, u_int64_t maxDepth, const u_int8_t *basePtr, u_int8_t *keySpace);
    // Create a Iterator on specified 'root' holding keys of at most 'maxDepth' bytes where specified 'keySpace' is 
    // memory sufficient to hold the current key. 'keySpace' becomes owned by this object and freed in its destructor.
    // Specified 'basePtr' points to the start of the memory managed by the root's memory manager when it was 
    // constructed. Behavior is defined provided there's at least 'maxDepth' bytes of valid, contiguous writeable bytes
    // in range '[keySpace, keySpace+maxDepth)'.

  Iterator(const Iterator& other) = delete;
    // Copy constructor not provided

  ~Iterator();
    // Destroy this object freeing memory for current key

  // ACCESSORS
  const u_int8_t *key() const;
    // Return the current key. Behavior is defined provided 'end()==false'

  u_int16_t keySize() const;
    // Return the size of the current key in bytes. Behavior is defined provided 'end()==false'

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
// CREATORS

inline
Iterator::Iterator(u_int32_t root, u_int64_t maxDepth, const u_int8_t *basePtr, u_int8_t *keySpace)
: d_basePtr(const_cast<u_int8_t*>(basePtr))
, d_key(keySpace)
, d_nodePtr(0)
, d_root(root)
, d_node(root)
, d_childNode(root)
, d_attributes(0)
, d_index(0)
, d_maxIndex(0)
, d_depth(0)
, d_maxDepth((u_int16_t)maxDepth)
, d_end(false)
, d_jump(false)
{                                                                                                                       
  assert(d_basePtr);
  assert(d_key);
  assert(d_root);
  // Root cannot be a leaf
  assert((d_root & k_NODE256_IS_LEAF) == 0);
  // Root cannot be a terminal node
  assert((d_root & k_NODE256_IS_TERMINAL) == 0);
  // Set up a valid Node256 pointer from root offset
  d_nodePtr = (Node256*)(d_basePtr+d_root); 
  assert(d_nodePtr);

  if (maxDepth>0) {
    d_index = d_nodePtr->minIndex();
    d_maxIndex = d_nodePtr->maxIndex();
    next();
  } else {
    d_end = true;
  }
}

inline
Iterator::~Iterator() {                                                                                         
  if (d_key) {                                                                                                          
    free(d_key);                                                                                  
    d_key = 0;                                                                                                          
  }                                                                                                                     
}     

inline
std::ostream& Iterator::print(std::ostream& stream) const {
  assert(!d_end);

  stream << "key: '";

  for (unsigned i=0; i<keySize(); ++i) {
    if (!isprint(d_key[i])) {
      char buf[5];
      sprintf(buf, "0x%02x", d_key[i]);
      stream << buf;
    } else {
      stream << d_key[i];
    }
  }

  stream << "' isTerminal: "  << isTerminal()
         << " isLeafNode: " << isLeaf()
         << std::endl;

  return stream;
}

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
bool Iterator::isTerminal() const {
  assert(!d_end);
  return (d_attributes & k_NODE256_IS_TERMINAL) || d_attributes==k_NODE256_IS_LEAF;
}

inline
bool Iterator::isLeaf() const {
  assert(!d_end);
  return d_attributes == k_NODE256_IS_LEAF;
}

inline
bool Iterator::end() const {
  return d_end;
}

} // namespace CRadix
