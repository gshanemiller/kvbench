#pragma once

#include <iostream>
#include <sys/types.h>

namespace CRadix {

struct NodeStats {
  // DATA
  u_int64_t d_copyAllocationCount;
  u_int64_t d_offsetCount;
  u_int64_t d_tryOffsetCount;
  u_int64_t d_canSetOffsetCount;
  u_int64_t d_failedCanSetOffsetCount;
  u_int64_t d_setOffsetCount;
  u_int64_t d_trySetOffsetCount;
  u_int64_t d_trySetOffsetCase1Count;
  u_int64_t d_trySetOffsetCase2Count;
  u_int64_t d_trySetOffsetCase3Count;
  u_int64_t d_bytesCopied;
  u_int64_t d_bytesCleared;

  // CREATORS
  NodeStats();
    // Create stats object with all attributes initialized zero

  ~NodeStats() = default;
    // Destroy this object

  NodeStats(const NodeStats& other) = default;
    // Create stats object s.t. all attributes equal to specified 'other'

  // MANIPULATORS
  void reset();
    // Reset all attributes to 0

  NodeStats& operator=(const NodeStats&rhs) = default;
    // Create and return a copy of specified 'rhs' s.t. all attributes equal.

  // ASPECTS
  std::ostream& print(std::ostream& stream) const;
    // Pretty print into specified 'stream' a human readable dump of attributes
    // returning 'stream'
};

// INLINE DEFINITIONS
// CREATORS
inline
NodeStats::NodeStats()
{
  reset();
}

// ASPECTS
inline 
std::ostream& NodeStats::print(std::ostream& stream) const {
  stream
    << "copyAllocationCount: "      << d_copyAllocationCount
    << " offsetCount: "              << d_offsetCount
    << " tryOffsetCount: "           << d_tryOffsetCount
    << " canSetOffsetCount: "        << d_canSetOffsetCount
    << " failedCanSetOffsetCount: "  << d_failedCanSetOffsetCount
    << " setOffsetCount: "           << d_setOffsetCount
    << " trySetOffsetCount: "        << d_trySetOffsetCount
    << " trySetOffsetCase1Count: "   << d_trySetOffsetCase1Count
    << " trySetOffsetCase2Count: "   << d_trySetOffsetCase2Count
    << " trySetOffsetCase3Count: "   << d_trySetOffsetCase3Count
    << " bytesCopied: "              << d_bytesCopied
    << " bytesCleared: "             << d_bytesCleared
    << std::endl;
  return stream;
}

inline
void NodeStats::reset(void) {
  d_copyAllocationCount = 0;
  d_offsetCount = 0;
  d_tryOffsetCount = 0;
  d_canSetOffsetCount = 0;
  d_failedCanSetOffsetCount = 0;
  d_setOffsetCount = 0;
  d_trySetOffsetCount = 0;
  d_trySetOffsetCase1Count = 0;
  d_trySetOffsetCase2Count = 0;
  d_trySetOffsetCase3Count = 0;
  d_bytesCopied = 0;
  d_bytesCleared = 0;
}

} // namespace CRadix
