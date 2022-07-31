#pragma once

#include <iostream>
#include <sys/types.h>

namespace CRadix {

struct TreeStats {
  // DATA
  u_int64_t d_innerNodeCount;
  u_int64_t d_leafCount;
  u_int64_t d_emptyChildCount;
  u_int64_t d_maxDepth;
  u_int64_t d_totalSizeBytes;
  u_int64_t d_totalCompressedSizeBytes;
  u_int64_t d_totalUncompressedSizeBytes;

  // CREATORS
  TreeStats();
    // Create stats object with all attributes initialized zero

  ~TreeStats() = default;
    // Destroy this object

  TreeStats(const TreeStats& other) = default;
    // Create stats object s.t. all attributes equal to specified 'other'

  // MANIPULATORS
  void reset();
    // Reset all attributes to 0

  TreeStats& operator=(const TreeStats&rhs) = default;
    // Create and return a copy of specified 'rhs' s.t. all attributes equal.

  // ASPECTS
  std::ostream& print(std::ostream& stream) const;
    // Pretty print into specified 'stream' a human readable dump of attributes
    // returning 'stream'
};

// INLINE DEFINITIONS
// CREATORS
inline
TreeStats::TreeStats()
{
  reset();
}

// ASPECTS
inline 
std::ostream& TreeStats::print(std::ostream& stream) const {
  double ratio(0);

  if (d_totalUncompressedSizeBytes!=0) {
    ratio = static_cast<double>(d_totalCompressedSizeBytes) /
            static_cast<double>(d_totalUncompressedSizeBytes);
  }

  double bytesPerInnerNode = static_cast<double>(d_totalSizeBytes)/static_cast<double>(d_innerNodeCount);

  stream  << "innerNodeCount: "               << d_innerNodeCount
          << " leafCount: "                   << d_leafCount
          << " emptyChildCount: "             << d_emptyChildCount
          << " maxDepth: "                    << d_maxDepth
          << " totalSizeBytes: "              << d_totalSizeBytes
          << " totalCompressedSizeBytes: "    << d_totalCompressedSizeBytes
          << " totalUncompressedSizeBytes: "  << d_totalUncompressedSizeBytes
          << " compressionRatio: "            << ratio
          << " totalSizeBytes-per-innerNode: "<< bytesPerInnerNode
          << std::endl;
  return stream;
}

inline
void TreeStats::reset(void) {
  d_innerNodeCount = 0;
  d_leafCount = 0;
  d_emptyChildCount = 0;
  d_maxDepth = 0;
  d_totalSizeBytes = 0;
  d_totalCompressedSizeBytes = 0;
  d_totalUncompressedSizeBytes = 0;
}

} // namespace CRadix
