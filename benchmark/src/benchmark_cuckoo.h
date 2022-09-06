#pragma once

// PURPOSE: Benchmark cuckoo hashmap
//
// CLASSES:
//  Benchmark::Cuckoo: Benchmark the Cuckoo hashing algorithm

#include <benchmark_config.h>
#include <benchmark_loadfile.h>
#include <intel_pmu_stats.h>

namespace Benchmark {

class Cuckoo {
  // DATA
  const LoadFile& d_file;
  const Config&   d_config;
  Intel::Stats    d_stats;

public:
  // CREATORS
  explicit Cuckoo(const Config& config, const LoadFile& file);
    // Create Cuckoo benchmark object with specified 'config, file'

  Cuckoo(const Cuckoo& other) = delete;
    // Copy constructor not provided

  ~Cuckoo() = default;
    // Destory this object

  // MANIPULATORS
  int start();
    // Return 0 if all benchmarks were run and non-zero otherwise. Note a non-zero code usually indicates
    // bad configuration.

  void report();
    // Emit to stdout collected benchmark statistics

  Cuckoo& operator=(const Cuckoo& rhs) = delete;
    // Assignment operator not provided
};

inline Cuckoo::Cuckoo(const Config& config, const LoadFile& file)
: d_file(file)
, d_config(config)
{
}

} // namespace Benchmark
