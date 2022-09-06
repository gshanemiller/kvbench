#pragma once

// PURPOSE: Benchmark HOT: A Height Optimized Trie Index for Main-Memory Database Systems
//                         by Binna, Zangerle, Pichl, Specht and Leis
//
// CLASSES:
//  Benchmark::Hot:        Benchmark HOT (height optimized trie)
//                         https://github.com/speedskater/hot

#include <benchmark_config.h>
#include <benchmark_loadfile.h>
#include <intel_pmu_stats.h>

namespace Benchmark {

class HOT {
  // DATA
  const LoadFile& d_file;
  const Config&   d_config;                                                                                             
  Intel::Stats    d_stats;

public:
  // CREATORS
  explicit HOT(const Config& config, const LoadFile& file);
    // Create HOT benchmark object with specified 'config, file'

  HOT(const HOT& other) = delete;
    // Copy constructor not provided

  ~HOT() = default;
    // Destory this object

  // MANIPULATORS
  int start();
    // Return 0 if all benchmarks were run and non-zero otherwise. Note a non-zero code usually indicates
    // bad configuration.

  void report();
    // Emit to stdout collected benchmark statistics

  HOT& operator=(const HOT& rhs) = delete;
    // Assignment operator not provided
};

inline HOT::HOT(const Config& config, const LoadFile& file)
: d_file(file)
, d_config(config)
{
}

} // namespace Benchmark
