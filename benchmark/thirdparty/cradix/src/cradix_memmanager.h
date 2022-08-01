#include <assert.h>
#include <stdlib.h>
#include <sys/types.h>
#include <iostream>

#include <cradix_constants.h>
#include <cradix_memstats.h>
#include <cradix_node256.h>

namespace CRadix {

struct MemManager {
  // DATA
  union {
    u_int8_t *d_basePtr;          // memory mananged by this object
    u_int64_t d_baseVal;          // 'd_basePtr' as uint64 
  };
  u_int64_t   d_size;             // size in bytes of memory at basePtr
  u_int64_t   d_offset;           // first free byte after 'd_basePtr' not in use
  u_int64_t   d_alignment;        // alignment in bytes of all addresses returned
  u_int64_t   d_alignMask;        // bit mask used to check for correct alignment
#ifdef CRADIX_MEMMANAGER_RUNTIME_STATISTICS
  u_int64_t   d_allocCount;       // number of Node256s allocated
  u_int64_t   d_deadCount;        // number of Node255s marked dead
  u_int64_t   d_currentSizeBytes; // current amount of allocated memory
  u_int64_t   d_maximumSizeBytes; // max 'currentSizeBytes' seen so far
  u_int64_t   d_requestedBytes;   // sum of sizes to all malloc calls
  u_int64_t   d_freeBytes;        // sum of sizes to all free calls
  u_int64_t   d_deadBytes;        // total freed memory not yet reclaimed
#endif
  bool        d_owned;            // true if memory freed in destructor

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

  u_int64_t size() const;
    // Return 'size' in bytes specified at construction time

  u_int64_t freeMemory() const;
    // Return the size in bytes of free memory. This does not include freed
    // memory not yet reclaimed.

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
    
  u_int32_t copyAllocateNode256(const u_int32_t oldObject, u_int32_t newCapacity);
    // Allocate memory with specified 'newCapacity' then copy-construct 'oldObject'
    // into it. The memory at 'oldObject' is freed (marked dead). The offset to
    // the new object is returned. Behavior is defined provided 'newCapacity'
    // is larger than old object's capacity and less than or equal to k_MAX_CHILDREN.
    // If there's no enough free memory 0 is returned.

  u_int32_t newRoot(u_int32_t minIndex, u_int32_t maxIndex);
    // Allocate memory and construct the root of a CRadix tree object holding
    // zero offset values for all children in the range '[minIndex, maxIndex]'.
    // Behavior is defined provided '0<=minIndex<=maxIndex<k_MAX_CHILDREN'.
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
#ifdef CRADIX_MEMMANAGER_RUNTIME_STATISTICS
, d_allocCount(0)
, d_deadCount(0)
, d_currentSizeBytes(0)
, d_maximumSizeBytes(0)
, d_requestedBytes(0)
, d_freeBytes(0)
, d_deadBytes(0)
#endif
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
}

inline
MemManager::MemManager(u_int64_t size, u_int64_t alignment)
: d_baseVal(0)
, d_size(size)
, d_offset(0)
, d_alignment(alignment)
, d_alignMask(alignment-1UL)
#ifdef CRADIX_MEMMANAGER_RUNTIME_STATISTICS
, d_allocCount(0)
, d_deadCount(0)
, d_currentSizeBytes(0)
, d_maximumSizeBytes(0)
, d_requestedBytes(0)
, d_freeBytes(0)
, d_deadBytes(0)
#endif
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
u_int64_t MemManager::size() const {
  return d_size;
}

inline
u_int64_t MemManager::freeMemory() const {
  return d_size - d_offset;
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
    return 0;
  }

  // Construct the memory
  const u_int32_t ret = d_offset;
  Node256 *ptr = new(d_basePtr+d_offset) Node256(capacity, index, offset);

  // Adjust d_offset to next 'alignment' boundary
  d_offset += sz;
  u_int64_t rem = d_offset & d_alignMask;
  if (rem) {
    d_offset += (d_alignment - rem);
    if (d_offset>d_size) {
      d_offset = d_size;
    }
  }  

  assert(((d_baseVal+d_offset) & d_alignMask)==0);
  assert(d_offset<=d_size);

  return ret;
}

inline
u_int32_t MemManager::copyAllocateNode256(const u_int32_t object, u_int32_t newCapacity) {
  assert(object>=k_MEMMANAGER_MIN_OFFSET);
  return 0;
}

inline
u_int32_t MemManager::newRoot(u_int32_t minIndex, u_int32_t maxIndex) {
  assert(minIndex<=maxIndex);
  assert(maxIndex<k_MAX_CHILDREN);

  // Memory request in bytes
  u_int64_t sz = sizeof(Node256)+((maxIndex-minIndex+1)<<2);

  // Make sure we have memory
  if ((d_offset+sz)>d_size) {
    return 0;
  }

  // Construct the memory
  const u_int32_t ret = d_offset;
  Node256 *ptr = new(d_basePtr+d_offset) Node256(maxIndex-minIndex+1, minIndex, 0);
  for (u_int16_t i = minIndex; i<=maxIndex; ++i) {
    ptr->setOffset(i, 0);
  }

  // Adjust d_offset to next 'alignment' boundary
  d_offset += sz;
  u_int64_t rem = d_offset & d_alignMask;
  if (rem) {
    d_offset += (d_alignment - rem);
    if (d_offset>d_size) {
      d_offset = d_size;
    }
  }  

  assert(((d_baseVal+d_offset) & d_alignMask)==0);
  assert(d_offset<=d_size);

  return ret;
}

} // namespace CRadix
