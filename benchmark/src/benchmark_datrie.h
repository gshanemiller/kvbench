#pragma once

#include <benchmark_config.h>
#include <benchmark_loadfile.h>
#include <intel_pmu_stats.h>

namespace Benchmark {

class datrie {
  // DATA
  const LoadFile& d_file;
  const Config&   d_config;                                                                                             
  Intel::Stats    d_findStats;
  Intel::Stats    d_insertStats;

public:
  // CREATORS
  explicit datrie(const Config& config, const LoadFile& file);
    // Create datrie benchmark object with specified 'config, file'

  datrie(const datrie& other) = delete;
    // Copy constructor not provided

  ~datrie() = default;
    // Destory this object

  // MANIPULATORS
  int start();
    // Return 0 if all benchmarks were run and non-zero otherwise. Note a non-zero code usually indicates
    // bad configuration.

  void report();
    // Emit to stdout collected benchmark statistics

  datrie& operator=(const datrie& rhs) = delete;
    // Assignment operator not provided
};

inline datrie::datrie(const Config& config, const LoadFile& file)
: d_file(file)
, d_config(config)
{
}

} // namespace Benchmark
