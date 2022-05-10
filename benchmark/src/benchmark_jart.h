#pragma once

// PURPOSE: Benchmark Justin A's Adaptive Radix tree
//
// CLASSES:
//  Benchmark::JArt: Benchmark ADT

#include <benchmark_config.h>
#include <benchmark_loadfile.h>
#include <benchmark_stats.h>

namespace Benchmark {

class JArt {
  // DATA
  const LoadFile& d_file;
  Stats           d_stats;

public:
  // CREATORS
  explicit JArt(const Config& config, const LoadFile& file);
    // Create JArt benchmark object with specified 'config, file'

  JArt(const JArt& other) = delete;
    // Copy constructor not provided

  ~JArt() = default;
    // Destory this object

  // MANIPULATORS
  int start();
    // Return 0 if all benchmarks were run and non-zero otherwise. Note a non-zero code usually indicates
    // bad configuration.

  void report();
    // Emit to stdout collected benchmark statistics

  JArt& operator=(const JArt& rhs) = delete;
    // Assignment operator not provided
};

inline JArt::JArt(const Config& config, const LoadFile& file)
: d_file(file)
, d_stats(config)
{
}

} // namespace Benchmark
