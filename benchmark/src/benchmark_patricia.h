#pragma once

#include <benchmark_config.h>
#include <benchmark_loadfile.h>
#include <intel_pmu_stats.h>

#include <patricia_tree.h>

namespace Benchmark {

class patricia {
  // DATA
  const LoadFile& d_file;
  const Config&   d_config;                                                                                             
  Intel::Stats    d_stats;

public:
  // CREATORS
  explicit patricia(const Config& config, const LoadFile& file);
    // Create patricia benchmark object with specified 'config, file'

  patricia(const patricia& other) = delete;
    // Copy constructor not provided

  ~patricia() = default;
    // Destory this object

  // MANIPULATORS
  int start();
    // Return 0 if all benchmarks were run and non-zero otherwise. Note a non-zero code usually indicates
    // bad configuration.

  void report();
    // Emit to stdout collected benchmark statistics

  patricia& operator=(const patricia& rhs) = delete;
    // Assignment operator not provided
};

inline patricia::patricia(const Config& config, const LoadFile& file)
: d_file(file)
, d_config(config)
{
}

} // namespace Benchmark
