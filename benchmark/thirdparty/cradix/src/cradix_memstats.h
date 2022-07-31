#pragma once

#include <iostream>

namespace CRadix {

struct MemStats {
  // DATA
  u_int64_t d_allocCount;           // number of Node256s allocated
  u_int64_t d_freeCount;            // number of Node255s freed
  u_int64_t d_currentSizeBytes;     // current amount of allocated memory
  u_int64_t d_maximumSizeBytes;     // max 'currentSizeBytes' seen so far
  u_int64_t d_requestedBytes;       // sum of sizes to all malloc calls
  u_int64_t d_freedBytes;           // sum of sizes to all free calls
  u_int64_t d_deadBytes;            // total freed memory not reclaimed
  u_int64_t d_sizeBytes;            // size in bytes of memory under management

  // CREATORS
  MemStats();
    // Create MemStats object with all atrributes initialized zero

  MemStats(const MemStats& other) = default;
    // Create MemStats object with attributes equal to 'other'

  ~MemStats() = default;
    // Destory this object

  // MANIPULATORS
  void reset();
    // Reset all attributes to zero

  MemStats& operator=(const MemStats& rhs) = default;
    // Create and return MemStats object with attributes equal to 'rhs'

  // ASPECTS
  std::ostream& print(std::ostream& stream) const;
    // Print into specified stream a human readable dump of this' attributes
    // returning stream
};

// INLINE DEFINITIONS
// CREATORS
inline
MemStats::MemStats()
: d_allocCount(0)
, d_freeCount(0)
, d_currentSizeBytes(0)
, d_maximumSizeBytes(0)
, d_requestedBytes(0)
, d_freedBytes(0)
, d_deadBytes(0)
, d_sizeBytes(0)
{
}

// MANIPULATORS
inline
void MemStats::reset() {
  d_allocCount =
  d_freeCount =
  d_currentSizeBytes =
  d_maximumSizeBytes =
  d_requestedBytes =
  d_freedBytes =
  d_deadBytes = 
  d_sizeBytes = 0;
}

// ASPECTS
inline
std::ostream& MemStats::print(std::ostream& stream) const {
  stream << "allocCount: "        << d_allocCount
         << " freeCount: "        << d_freeCount
         << " currentSizeBytes: " << d_currentSizeBytes
         << " maximumSizeBytes: " << d_maximumSizeBytes
         << " requestedBytes: "   << d_requestedBytes
         << " freedBytes: "       << d_freedBytes
         << " deadBytes: "        << d_deadBytes
         << " sizeBytes: "        << d_sizeBytes
         << std::endl;
  return stream;
}

} // namespace CRadix
