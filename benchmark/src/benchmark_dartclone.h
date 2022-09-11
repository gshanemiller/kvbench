#pragma once

// PURPOSE: Benchmark Dart Clone

#include <benchmark_config.h>
#include <benchmark_loadfile.h>
#include <intel_pmu_stats.h>

namespace Benchmark {

class DartClone {
  // DATA
  const LoadFile& d_file;
  const Config&   d_config;                                                                                             
  Intel::Stats    d_findStats;
  Intel::Stats    d_insertStats;

public:
  // CREATORS
  explicit DartClone(const Config& config, const LoadFile& file);
    // Create DartClone benchmark object with specified 'config, file'

  DartClone(const DartClone& other) = delete;
    // Copy constructor not provided

  ~DartClone() = default;
    // Destory this object

  // MANIPULATORS
  int start();
    // Return 0 if all benchmarks were run and non-zero otherwise. Note a non-zero code usually indicates
    // bad configuration.

  void report();
    // Emit to stdout collected benchmark statistics

  DartClone& operator=(const DartClone& rhs) = delete;
    // Assignment operator not provided
};

inline DartClone::DartClone(const Config& config, const LoadFile& file)
: d_file(file)
, d_config(config)
{
}

} // namespace Benchmark
