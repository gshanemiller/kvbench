#pragma once

// PURPOSE: Benchmark ART: Adaptive Radix Trie

#include <benchmark_config.h>
#include <benchmark_loadfile.h>
#include <intel_pmu_stats.h>

namespace Benchmark {

class ART {
  // DATA
  const LoadFile& d_file;
  const Config&   d_config;                                                                                             
  Intel::Stats    d_stats;

public:
  // CREATORS
  explicit ART(const Config& config, const LoadFile& file);
    // Create ART benchmark object with specified 'config, file'

  ART(const ART& other) = delete;
    // Copy constructor not provided

  ~ART() = default;
    // Destory this object

  // MANIPULATORS
  int start();
    // Return 0 if all benchmarks were run and non-zero otherwise. Note a non-zero code usually indicates
    // bad configuration.

  void report();
    // Emit to stdout collected benchmark statistics

  ART& operator=(const ART& rhs) = delete;
    // Assignment operator not provided
};

inline ART::ART(const Config& config, const LoadFile& file)
: d_file(file)
, d_config(config)
{
}

} // namespace Benchmark
