#pragma once

// PURPOSE: Benchmark cedar trie

#include <benchmark_config.h>
#include <benchmark_loadfile.h>
#include <intel_pmu_stats.h>

namespace Benchmark {

class Cedar {
  // DATA
  const LoadFile& d_file;
  const Config&   d_config;                                                                                             
  Intel::Stats    d_findStats;
  Intel::Stats    d_insertStats;

public:
  // CREATORS
  explicit Cedar(const Config& config, const LoadFile& file);
    // Create Cedar benchmark object with specified 'config, file'

  Cedar(const Cedar& other) = delete;
    // Copy constructor not provided

  ~Cedar() = default;
    // Destory this object

  // MANIPULATORS
  int start();
    // Return 0 if all benchmarks were run and non-zero otherwise. Note a non-zero code usually indicates
    // bad configuration.

  void report();
    // Emit to stdout collected benchmark statistics

  Cedar& operator=(const Cedar& rhs) = delete;
    // Assignment operator not provided
};

inline Cedar::Cedar(const Config& config, const LoadFile& file)
: d_file(file)
, d_config(config)
{
}

} // namespace Benchmark
