#pragma once

#include <iostream>
#include <assert.h>
#include <sys/types.h>

#include <cradix_constants.h>

namespace CRadix {

// Class Node256: Compressed CRadix inner node
//
// Organization of 4 bytes in union:
// (*) bits: 0-7  : minimum non-zero offset
// (*) bits: 8-15 : maximum non-zero offset
// (*) bits: 16-23: spare capacity
// (*) bits: 24-31: attributes:
//           bit 24   : 1 if this node is dead
//           bit 25-31: unused
struct Node256 {
  union {
    int32_t     d_data;     // span state
    u_int32_t   d_udata;
  };
  u_int32_t     d_offset[]; // array of offset integers for i in [min,max]

  // CREATORS
  Node256();
    // Create Node256 with full capacity setting all offsets 0. Node that this method is
    // intended for root not creation only

  explicit Node256(const u_int32_t index, const u_int32_t offset, const u_int32_t capacity);
    // Create Node256 with specified 'capacity' setting min/max to specified 'index' such that
    // on return 'offset[index]==offset'. Behavior is defined provided 'index<k_MAX_CHILDREN',
    // '1<=capacity<=k_MAX_CHILDREN' 

  ~Node256() = default;
    // Destroy this object. Note that this method does not cleanup memory. To reclaim memory
    // held in a CRadix tree by its internal nodes run 'Tree::destroy'

  Node256(u_int32_t newCapacity, const Node256 *oldNode);
    // Copy-construct this object from 'oldNode' where this object has 'newCapacity'.
    // Behavior is defined provided 'newCapacity>oldNode->capacity()' and 'newCapacity'
    // is less than '<=k_MAX_CHILDREN'

  Node256(const Node256& other) = delete;
    // Copy constuctor not provided

  // ACCESSORS
  u_int32_t tryOffset(const u_int32_t index) const;
    // Return the offset at specified 'index' or 0 if 'index' is invalid. Behavior is defined
    // provided 'i<k_MAX_CHILDREN' and 'isDead()==false'. Note that 0 is returned if 'index'
    // does not satisfy ''minIndex()<=index<=maxIndex()'.
 
  u_int32_t offset(const u_int32_t index) const;
    // Return the offset at specified 'index'. Behavior is defined provided 'minIndex()<=i<=maxIndex()'
    // and 'isDead()==false'

  u_int32_t minIndex() const;
    // Return the smallest index in '[0, k_MAX_CHILDREN)' this node holds

  u_int32_t maxIndex() const;
    // Return the largest index in '[0, k_MAX_CHILDREN)' this node holds

  int32_t spareCapacity() const;
    // Return the number of offsets this node can hold without requiring memory allocation

  u_int32_t uspareCapacity() const;
    // Return the number of offsets this node can hold without requiring memory allocation

  u_int32_t capacity() const;
    // Return the number of offsets this node can hold

  int32_t size() const;
    // Return the number of offsets this node is holding via previous 'setOffset/trySetOffset' calls

  u_int32_t usize() const;
    // Return the number of offsets this node is holding via previous 'setOffset/trySetOffset' calls

  bool isDead() const;
    // Return true of this node is considered deleted/deallocated

  bool canSetOffset(const int32_t i, int32_t& oldMin, int32_t& oldMax,
    int32_t& newMin, int32_t& newMax, int32_t& delta) const;
    // Return true if it's possible to set an offset at specified index 'i' without requiring
    // memory allocation, and false otherwise. Behavior is defined provided 'i<k_MAX_CHILDREN'.
    // The other arguments may be ignored, but note they are used by 'trySetOffset'

  u_int64_t uncompressedSizeBytes() const;
    // Return the uncompressed size of this node. This is a constant equal to k_MAX_CHILDREN
    // multiplied by 'sizeof(void*)' which would be the textbook way to model k_MAX_CHILDREN pointers to
    // children

  u_int64_t compressedSizeBytes() const;
    // Return the actual (compressed) size of this node in bytes

  double compressionRatio() const;
    // Return the ratio of 'compressedSizeBytes() / uncompressedSizeBytes()'

  // MANIPULATORS
  void setOffset(const u_int32_t i, u_int32_t offset);
    // Set the value at specified index 'i' to specified 'offset'. Behavior is defined
    // provided 'minIndex()<=i<=maxIndex()' and 'isDead()==false'

  bool trySetOffset(const u_int32_t index, const u_int32_t offset);
    // Return true and set specified 'index' to specified 'offset', and otherwise return false
    // if memory reallocation is required. Behavior is defined provided 'i<k_MAX_CHILDREN' and
    // 'isDead()==false'.

  void markDead();
    // Mark this object dead and eligble for reclaimation

  const Node256& operator=(const Node256& rhs) = delete;
    // Copy constructor not provided

  // ASPECTS
  std::ostream& statistics(std::ostream& stream) const;
    // Print into specified 'stream' a human readable set of statistics about this node
    // returning 'stream'

  std::ostream& print(std::ostream& stream) const;
    // Print into specified 'stream' a human readable list of the offsets for each index
    // this node holds returning stream. Note that only dead or non-zero offsets are
    // dumped.
};

// INLINE DEFINITIONS
// CREATORS

inline
Node256::Node256() {
  d_udata = 0 | (k_MAX_CHILDREN-1)<<8;
  for (u_int32_t i=0; i<=maxIndex(); ++i) {
    d_offset[i] = 0; 
  }
}

inline
Node256::Node256(const u_int32_t index, const u_int32_t offset, const u_int32_t capacity)
{
  assert(index<k_MAX_CHILDREN);
  assert(capacity>0&&capacity<=k_MAX_CHILDREN);
  d_udata = index | (index<<8) | ((capacity-1)<<16);
  d_offset[0] = offset;
}

inline
Node256::Node256(u_int32_t newCapacity, const Node256 *oldNode) {
  assert(newCapacity>0&&newCapacity<=k_MAX_CHILDREN);
  assert(newCapacity>oldNode->capacity());
  d_udata = oldNode->minIndex() | (oldNode->maxIndex()<<8) | ((newCapacity)<<16);
  memcpy(d_offset, oldNode->d_offset, oldNode->capacity()<<2);
}

// ACCESSORS
inline
u_int32_t Node256::tryOffset(const u_int32_t index) const {
  assert(!isDead());
  if (index>=minIndex() && index<=maxIndex()) {
    return d_offset[index-minIndex()];
  }
  return 0;
}
 
inline
u_int32_t Node256::offset(const u_int32_t index) const {
  assert(!isDead());
  assert(index>=minIndex() && index<=maxIndex());
  return d_offset[index-minIndex()];
}

inline
u_int32_t Node256::minIndex() const {
  return d_udata & 0xff;
}

inline
u_int32_t Node256::maxIndex() const {
  return (d_udata & 0xff00) >> 8;
}

inline
int32_t Node256::spareCapacity() const {
  return (d_data & 0xff0000) >> 16;
}

inline
u_int32_t Node256::uspareCapacity() const {
  return (d_udata & 0xff0000) >> 16;
}

inline
u_int32_t Node256::capacity() const {                                                                                          
  return maxIndex() - minIndex() + 1 + uspareCapacity();
}

inline
int32_t Node256::size() const {
  return maxIndex() - minIndex() + 1;
}

inline
u_int32_t Node256::usize() const {
  return maxIndex() - minIndex() + 1;
}

inline
bool Node256::isDead() const {
  // Dead if bit-24 set
  return d_udata & k_NODE256_IS_DEAD;
}

inline
u_int64_t Node256::uncompressedSizeBytes() const {
  // sizeof textbook array of k_MAX_CHILDREN pointers
  return k_MAX_CHILDREN*sizeof(void*);
}

inline
u_int64_t Node256::compressedSizeBytes() const {
  // NB: sizeof does not include d_offset memory
  return (capacity()<<2) + sizeof(Node256);
}

inline
double Node256::compressionRatio() const {
  return static_cast<double>(compressedSizeBytes()) /
         static_cast<double>(uncompressedSizeBytes());
}

// MANIPULATORS
inline
bool Node256::canSetOffset(const int32_t i, int32_t& oldMin, int32_t& oldMax,
  int32_t& newMin, int32_t& newMax, int32_t& delta) const {
  assert(!isDead());
  oldMin = d_data & 0xff;
  oldMax = (d_data & 0xff00) >> 8;
  delta  = oldMax - oldMin;
  // branchless std::min(oldMin, i)
  newMin = i + ((oldMin - i) & ((oldMin - i) >> ((sizeof(int32_t)<<3) - 1)));       
  // branchless std::max(oldMax, i)
  newMax = oldMax - ((oldMax - i) & ((oldMax - i) >> ((sizeof(int32_t)<<3) - 1)));     
  delta = newMax - newMin - delta;
  // false implies memory re-allocation required
  return (delta <= spareCapacity());
}

inline
void Node256::setOffset(const u_int32_t i, u_int32_t offset) {
  // special case when existing pointers are mutated and it's known
  // with certainty that specified index is valid
  assert(!isDead());
  assert(i>=minIndex()&&i<=maxIndex());
  d_offset[i-minIndex()] = offset;
  printf("**************************************************************** setOffset %p: [%u]==%u\n", (void*)this, i, offset);
}

inline
void Node256::markDead() {
  d_udata |= k_NODE256_IS_DEAD;
}

// ASPECTS
inline
std::ostream& Node256::statistics(std::ostream& stream) const {
  stream  << "minIndex: "           << minIndex()
          << " maxIndex: "          << maxIndex()
          << " size: "              << size()
          << " capacity: "          << capacity()
          << " spareCapacity: "     << spareCapacity()
          << " compressedSize: "    << compressedSizeBytes() 
          << " uncompressedSize: "  << uncompressedSizeBytes()
          << " compressionRatio: "  << compressionRatio()
          << " isDead: " << isDead()
          << std::endl;
  return stream;
}

inline
std::ostream& Node256::print(std::ostream& stream) const {
  for (u_int32_t i = minIndex(); i<=maxIndex(); ++i) {
    if (d_offset[i-minIndex()]!=0 || isDead()) {
      stream << "[" << i << "] = " << d_offset[i-minIndex()];
      if (isDead()) {
        stream << " (dead)";
      }
      stream << std::endl;
    }
  }
  return stream;
}

} // namespace CRadix
