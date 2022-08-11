#include <assert.h>
#include <stdlib.h>
#include <sys/types.h>
#include <iostream>
#include <vector>

#include <cradix_constants.h>
#include <cradix_memstats.h>
#include <cradix_node256.h>

namespace CRadix {

struct MemManager {
  // DATA
  union {
    u_int8_t *d_basePtr;                  // memory mananged by this object
    u_int64_t d_baseVal;                  // 'd_basePtr' as uint64 
  };
  u_int64_t   d_size;                     // size in bytes of memory at basePtr
  u_int64_t   d_offset;                   // first free byte after 'd_basePtr' not in use
  u_int64_t   d_alignment;                // alignment in bytes of all addresses returned
  u_int64_t   d_alignMask;                // bit mask used to check for correct alignment
#ifdef CRADIX_MEMMANAGER_RUNTIME_STATISTICS
  MemStats    d_stats;                    // runtime memory statistics
#endif
  std::vector<u_int32_t>  d_deadOffsets;
  bool                    d_owned;        // true if memory freed in destructor

  // CREATORS
  MemManager() = delete;
    // Default constructor not provided

  MemManager(const MemManager& other) = delete;
    // Copy constructor not provided

  explicit MemManager(u_int8_t *ptr, u_int64_t size, u_int64_t alignment);
    // Create MemManager to manage specified 'size' bytes starting at 'ptr'.
    // Allocations will have specified byte 'alignment'. The behavior is
    // defined provided 'k_MEMMANAGER_MIN_MEMORY<=size<k_MEMMANAGER_MIN_MEMORY',
    // 'alignment>=k_MEMMANAGER_MIN_ALIGN' and 'alignment' is a power of two.
    // This memory is not owned and is not freed in destructor. The memory in
    // range '[ptr, ptr+size)' must be contiguous, valid, and writeable

  explicit MemManager(u_int64_t size, u_int64_t alignment);
    // Create MemManager object by allocating 'size' bytes. Allocations will
    // have specified byte 'alignment'. The behavior is defined provided
    // 'k_MEMMANAGER_MIN_MEMORY<=size<k_MEMMANAGER_MIN_MEMORY',
    // 'alignment>=k_MEMMANAGER_MIN_ALIGN' and 'alignment' is a power of two.
    // This memory is owned and is freed in destructor.

  ~MemManager();
    // Destroy this object freeing any owned memory allocated

  // ACCESSORS
  const u_int8_t *basePtr() const;
    // Return a non-modifiable pointer to the start of memory provided at
    // construction time. Note: required for 'Tree', 'TreeIterator' objects

  void statistics(MemStats *stats) const;
    // Write into specified 'stats' statistics summarizing memory activity

  // MANIPULATORS
  Node256 *ptr(u_int32_t offset);
    // Convert specified 'offset' into a memory pointer to a Node256 object.
    // Behavior is defined provided 'offset>=k_MEMMANAGER_MIN_OFFSET'

  u_int32_t newNode256(u_int32_t capacity, u_int32_t index, u_int32_t offset);
    // Allocate memory and construct a Node256 object with specified 'capacity'
    // s.t. on return specified 'offset' is assigned to specified 'index'. The
    // offset to this new object is returned. Behavior is defined provided
    // '1<=capacity<=k_MAX_CHILDREN'. The rest of the arguments are passed to
    // Node256's constructor and must satisfy its contract. If there's not
    // enough free memory 0 is returned.
    
  u_int32_t copyAllocateNode256(u_int32_t newMin, u_int32_t newMax, u_int32_t oldObject);
    // Allocate memory with capacity of 'newMax-newMin-1' offsets then copy-construct
    // 'oldObject' into it. The memory at 'oldObject' is marked dead. The offset
    // to the new object is returned. Behavior is defined provided 'newMin, newMax'
    // and implied new capacity satisfiy the contract for Node256's copy-creator.
    // If there's no enough free memory 0 is returned.

  u_int32_t newRoot();
    // Allocate memory and construct the root of a CRadix tree object holding
    // zero offset values for all children in the range '[0, k_MAX_CHILDREN)'.
    // If there's no enough free memory 0 is returned.
};

// INLINE DEFINITONS
// CREATORS

inline
MemManager::MemManager(u_int8_t *ptr, u_int64_t size, u_int64_t alignment)
: d_basePtr(ptr)
, d_size(size)
, d_offset(0)
, d_alignment(alignment)
, d_alignMask(alignment-1UL)
, d_owned(false)
{
  assert(ptr);
  assert(d_size>=k_MEMMANAGER_MIN_MEMORY);
  assert(d_size<k_MEMMANAGER_MAX_MEMORY);
  assert(d_alignment>=k_MEMMANAGER_MIN_ALIGN);
  assert(d_alignment>0 && (d_alignment & (d_alignment - 1))==0);

  // Move offset up to next 'alignment' byte after min offset
  d_offset = k_MEMMANAGER_MIN_OFFSET;
  while ((d_baseVal+d_offset) & d_alignMask) {
    ++d_offset;
  }

  assert(d_offset);
  assert(d_offset>=k_MEMMANAGER_MIN_OFFSET);
  assert(((d_baseVal+d_offset) & d_alignMask)==0);
  assert((d_offset&k_NODE256_IS_LEAF)==0);
  assert((d_offset&k_NODE256_IS_TERMINAL)==0);
  assert(((d_baseVal+d_offset) & k_NODE256_IS_LEAF)==0);
  assert(((d_baseVal+d_offset) & k_NODE256_IS_TERMINAL)==0);
  assert(d_offset<=d_size);

  d_deadOffsets.reserve(CRadix::k_MEMMANAGER_DEADMEMORY_RESERVE);
}

inline
MemManager::MemManager(u_int64_t size, u_int64_t alignment)
: d_baseVal(0)
, d_size(size)
, d_offset(0)
, d_alignment(alignment)
, d_alignMask(alignment-1UL)
, d_owned(true)
{
  assert(d_size>=k_MEMMANAGER_MIN_MEMORY);
  assert(d_size<k_MEMMANAGER_MAX_MEMORY);
  assert(d_alignment>=k_MEMMANAGER_MIN_ALIGN);
  assert(d_alignment>0 && (d_alignment & (d_alignment - 1))==0);

  // Allocate requested memory
  d_basePtr = (u_int8_t*)malloc(d_size);

  // Move offset up to next 'alignment' byte after min offset
  d_offset = k_MEMMANAGER_MIN_OFFSET;
  while ((d_baseVal+d_offset) & d_alignMask) {
    ++d_offset;
  }

  assert(d_offset);
  assert(d_offset>=k_MEMMANAGER_MIN_OFFSET);
  assert(((d_baseVal+d_offset) & d_alignMask)==0);
  assert((d_offset&k_NODE256_IS_LEAF)==0);
  assert((d_offset&k_NODE256_IS_TERMINAL)==0);
  assert(((d_baseVal+d_offset) & k_NODE256_IS_LEAF)==0);
  assert(((d_baseVal+d_offset) & k_NODE256_IS_TERMINAL)==0);
  assert(d_offset<=d_size);
}

inline
MemManager::~MemManager() {
  if (d_owned && d_basePtr) {
    free(d_basePtr);
    d_basePtr = 0;
  }
}

// ACCESSORS
inline
const u_int8_t *MemManager::basePtr() const {
  return d_basePtr;
}

inline
void MemManager::statistics(MemStats *stats) const {                                                            
  *stats = d_stats;
  stats->d_sizeBytes = d_size;
}

// MANIUPULATORS
inline
Node256* MemManager::ptr(u_int32_t offset) {
  assert(offset>=k_MEMMANAGER_MIN_OFFSET);
  return (Node256*)(d_basePtr+offset);
}

inline
u_int32_t MemManager::newNode256(u_int32_t capacity, u_int32_t index, u_int32_t offset) {
  assert(index<k_MAX_CHILDREN);
  assert(capacity>0 && capacity<=k_MAX_CHILDREN);

  // Memory request in bytes
  u_int64_t sz = sizeof(Node256)+(capacity<<2);

  // Make sure we have memory
  if ((d_offset+sz)>d_size) {
    assert(false);
    return 0;
  }

#ifdef CRADIX_MEMMANAGER_RUNTIME_STATISTICS
  ++d_stats.d_allocCount;
  d_stats.d_currentSizeBytes += sz;
  d_stats.d_requestedBytes += sz;
  if (d_stats.d_currentSizeBytes>d_stats.d_maximumSizeBytes) {
    d_stats.d_maximumSizeBytes = d_stats.d_currentSizeBytes;
  } 
#endif

  // Construct the memory
  const u_int32_t ret = d_offset;
  new(d_basePtr+d_offset) Node256(index, offset, capacity);

  // Adjust d_offset to next 'alignment' boundary
  d_offset += sz;
  u_int64_t rem = d_offset & d_alignMask;
  if (rem) {
    d_offset += (d_alignment - rem);
    if (d_offset>d_size) {
      d_offset = d_size;
    }
  }  

  assert((d_offset&d_alignMask)==0);
  assert((d_offset&k_NODE256_IS_LEAF)==0);
  assert((d_offset&k_NODE256_IS_TERMINAL)==0);
  assert(((d_baseVal+d_offset) & k_NODE256_IS_LEAF)==0);
  assert(((d_baseVal+d_offset) & k_NODE256_IS_TERMINAL)==0);
  assert(d_offset<=d_size);

  return ret;
}

inline
u_int32_t MemManager::copyAllocateNode256(u_int32_t newMin, u_int32_t newMax, u_int32_t oldObject) {
  assert(oldObject>=k_MEMMANAGER_MIN_OFFSET);
  assert(newMax>newMin);

  Node256 *oldNode = ptr(oldObject & k_NODE256_NO_TAG_MASK);

  // Memory request in bytes
  u_int64_t sz = sizeof(Node256)+((newMax-newMin+1)<<2);

  // Make sure we have memory
  if ((d_offset+sz)>d_size) {
    assert(false);
    return 0;
  }

  d_deadOffsets.push_back(oldObject & k_NODE256_NO_TAG_MASK);

#ifdef CRADIX_MEMMANAGER_RUNTIME_STATISTICS
  ++d_stats.d_allocCount;
  d_stats.d_currentSizeBytes += sz;
  d_stats.d_requestedBytes += sz;
  if (d_stats.d_currentSizeBytes>d_stats.d_maximumSizeBytes) {
    d_stats.d_maximumSizeBytes = d_stats.d_currentSizeBytes;
  } 
#endif

  // Copy-construct the memory
  const u_int32_t ret = d_offset;
  new(d_basePtr+d_offset) Node256(newMin, newMax, oldNode);

  // Mark old memory dead
  oldNode->markDead();
#ifdef CRADIX_MEMMANAGER_RUNTIME_STATISTICS
  ++d_stats.d_deadCount;
  d_stats.d_deadBytes += sizeof(Node256)+(oldNode->capacity()<<2);
#endif

  // Adjust d_offset to next 'alignment' boundary
  d_offset += sz;
  u_int64_t rem = d_offset & d_alignMask;
  if (rem) {
    d_offset += (d_alignment - rem);
    if (d_offset>d_size) {
      d_offset = d_size;
    }
  }  

  assert((d_offset&d_alignMask)==0);
  assert((d_offset&k_NODE256_IS_LEAF)==0);
  assert((d_offset&k_NODE256_IS_TERMINAL)==0);
  assert(((d_baseVal+d_offset) & k_NODE256_IS_LEAF)==0);
  assert(((d_baseVal+d_offset) & k_NODE256_IS_TERMINAL)==0);
  assert(d_offset<=d_size);

  return ret;
}

inline
u_int32_t MemManager::newRoot() {
  // Memory request in bytes
  u_int64_t sz = sizeof(Node256)+(k_MAX_CHILDREN<<2);

  // Make sure we have memory
  if ((d_offset+sz)>d_size) {
    assert(false);
    return 0;
  }

#ifdef CRADIX_MEMMANAGER_RUNTIME_STATISTICS
  ++d_stats.d_allocCount;
  d_stats.d_currentSizeBytes += sz;
  d_stats.d_requestedBytes += sz;
  if (d_stats.d_currentSizeBytes>d_stats.d_maximumSizeBytes) {
    d_stats.d_maximumSizeBytes = d_stats.d_currentSizeBytes;
  } 
#endif

  // Construct the memory
  const u_int32_t ret = d_offset;
  new(d_basePtr+d_offset) Node256;

  // Adjust d_offset to next 'alignment' boundary
  d_offset += sz;
  u_int64_t rem = d_offset & d_alignMask;
  if (rem) {
    d_offset += (d_alignment - rem);
    if (d_offset>d_size) {
      d_offset = d_size;
    }
  }  

  assert((d_offset&d_alignMask)==0);
  assert((d_offset&k_NODE256_IS_LEAF)==0);
  assert((d_offset&k_NODE256_IS_TERMINAL)==0);
  assert(((d_baseVal+d_offset) & k_NODE256_IS_LEAF)==0);
  assert(((d_baseVal+d_offset) & k_NODE256_IS_TERMINAL)==0);
  assert(d_offset<=d_size);

  return ret;
}

} // namespace CRadix
