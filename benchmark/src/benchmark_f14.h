#pragma once

// PURPOSE: Benchmark Facebook's F14 hashmap
//
// CLASSES:
//  Benchmark::FacebookF14: Benchmark Facebooks F14 hash map
//                          https://github.com/facebook/folly/blob/main/folly/container/F14.md
//                          https://news.ycombinator.com/item?id=19759630

#include <benchmark_config.h>
#include <benchmark_loadfile.h>
#include <intel_pmu_stats.h>

namespace Benchmark {

class FacebookF14 {
  // DATA
  const LoadFile& d_file;
  const Config&   d_config;                                                                                             
  Intel::Stats    d_findStats;
  Intel::Stats    d_insertStats;

public:
  // CREATORS
  explicit FacebookF14(const Config& config, const LoadFile& file);
    // Create FacebookF14 benchmark object with specified 'config, file'

  FacebookF14(const FacebookF14& other) = delete;
    // Copy constructor not provided

  ~FacebookF14() = default;
    // Destory this object

  // MANIPULATORS
  int start();
    // Return 0 if all benchmarks were run and non-zero otherwise. Note a non-zero code usually indicates
    // bad configuration.

  void report();
    // Emit to stdout collected benchmark statistics

  FacebookF14& operator=(const FacebookF14& rhs) = delete;
    // Assignment operator not provided
};

inline FacebookF14::FacebookF14(const Config& config, const LoadFile& file)
: d_file(file)
, d_config(config)
{
}

} // namespace Benchmark
