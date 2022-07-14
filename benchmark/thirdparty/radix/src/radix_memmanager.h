#pragma once

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

  ~MemManagerStats();
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

class MemManager {
public:
  // CREATORS
  MemManager();
    // Create a memory manager object

  ~MemManager();
    // Destory this object and all memory it manages. Note that all Radix
    // trees must be destoyed before this call

  MemManager(const MemManager& other) = delete;
    // Copy constructor not provided

  // ACCESSORS
  void statistics(MemManagerStats *stats);
    // Write into specified 'stats' memory statistics

  // MANIPULATORS
  Node256 *mallocNode256();
    // Allocate 'Node256' object
 
  MemManager& operator=(const MemManager& rhs) = delete;
    // Assignment operator not provided
};

} // nsmaespace Radix
