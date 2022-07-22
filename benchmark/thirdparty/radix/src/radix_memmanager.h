#pragma once

#include <radix_enums.h>

#include <mimalloc.h>

#include <iostream>

namespace Radix {

class Node256;

struct MemManagerStats {
  // DATA
  u_int64_t d_allocCount;           // number of times alloc called
  u_int64_t d_freeCount;            // number of times free called
  u_int64_t d_currentSizeBytes;     // current amount of allocated memory
  u_int64_t d_maximumSizeBytes;     // max 'currentSizeBytes' seen so far
  u_int64_t d_requestedBytes;       // sum of sizes to all malloc calls
  u_int64_t d_freedBytes;           // sum of sizes to all free calls
  u_int64_t d_limitSizeBytes;       // size in bytes of memory pool

  // CREATORS
  MemManagerStats();
    // Create MemManagerStats object with all atrributes initialized zero

  MemManagerStats(const MemManagerStats& other) = default;
    // Create MemManagerStats object with attributes equal to 'other'

  ~MemManagerStats() = default;
    // Destory this object

  // MANIPULATORS
  void reset();
    // Reset all attributes to zero

  MemManagerStats& operator=(const MemManagerStats& rhs) = default;
    // Create and return MemManagerStats object with attributes equal to 'rhs'

  // ASPECTS
  std::ostream& print(std::ostream& stream);
    // Print into specified stream a human readable dump of this' attributes
    // returning stream
};

// INLINE DEFINITIONS
// CREATORS
inline
MemManagerStats::MemManagerStats()
: d_allocCount(0)
, d_freeCount(0)
, d_currentSizeBytes(0)
, d_maximumSizeBytes(0)
, d_requestedBytes(0)
, d_freedBytes(0)
, d_limitSizeBytes(0)
{
}

// ASPECTS
inline
std::ostream& MemManagerStats::print(std::ostream& stream) {
  stream << "allocCount: "        << d_allocCount
         << " freeCount: "        << d_freeCount
         << " currentSizeBytes: " << d_currentSizeBytes
         << " maximumSizeBytes: " << d_maximumSizeBytes
         << " requestedBytes: "   << d_requestedBytes
         << " freedBytes: "       << d_freedBytes
         << " limitSizeBytes: "   << d_limitSizeBytes
         << std::endl;
  return stream;
}

class MemManager {
  // DATA
  MemManagerStats d_stats;

public:
  // CREATORS
  MemManager() = default;
    // Create a memory manager object

  ~MemManager() = default;
    // Destory this object and all memory it manages. Note that all Radix
    // trees must be destoyed before this call

  MemManager(const MemManager& other) = delete;
    // Copy constructor not provided

  // ACCESSORS
  u_int64_t sizeOfUncompressedNode256() const;
    // Return size in bytes of a Node256 object

  void statistics(MemManagerStats *stats);
    // Write into specified 'stats' memory statistics

  // MANIPULATORS
  Node256 *mallocNode256();
    // Allocate 'Node256' object

  void freeNode256(const Node256 *node);
    // Deallocate specified 'node'
 
  MemManager& operator=(const MemManager& rhs) = delete;
    // Assignment operator not provided
};

// INLINE DEFINITIONS
// CREATORS

// ACCESSORS
inline
u_int64_t MemManager::sizeOfUncompressedNode256() const {
  return sizeof(Node256*)*k_MAX_CHILDREN256;
}

inline
void MemManager::statistics(MemManagerStats *stats) {
  *stats = d_stats;
}

// MANIPULATORS
inline                                                                                                                  
Node256 *MemManager::mallocNode256() {                                                                    
  d_stats.d_requestedBytes += sizeOfUncompressedNode256();                                                              
  d_stats.d_currentSizeBytes += sizeOfUncompressedNode256();                                                            
  if (d_stats.d_currentSizeBytes > d_stats.d_maximumSizeBytes) {                                                        
    d_stats.d_maximumSizeBytes = d_stats.d_currentSizeBytes;                                                            
  }                                                                                                                     
  ++d_stats.d_allocCount;                                                                                               
                                                                                                                        
  // Ask for zeroed 'sizeOfUncompressedNode256()' bytes on 2-byte alignment                                                    
  return (Node256*)mi_zalloc_aligned(sizeOfUncompressedNode256(), 2);
}

inline
void MemManager::freeNode256(const Node256 *node) {
  d_stats.d_currentSizeBytes -= sizeOfUncompressedNode256();
  d_stats.d_freedBytes += sizeOfUncompressedNode256();
  ++d_stats.d_freeCount;
  mi_free((void*)node);
}

} // nsmaespace Radix
