#pragma once

// PURPOSE: Benchmark overhead, and selected building block algos
//
// CLASSES:
//  Benchmark::Misc: Benchmark scanner over head and primitive algo build blocks

#include <benchmark_config.h>
#include <benchmark_loadfile.h>
#include <benchmark_stats.h>

namespace Benchmark {

class Misc {
  // DATA
  const LoadFile& d_file;
  Stats           d_stats;

public:
  // CREATORS
  explicit Misc(const Config& config, const LoadFile& file);
    // Create Misc benchmark object with specified 'config, file'

  Misc(const Misc& other) = delete;
    // Copy constructor not provided

  ~Misc() = default;
    // Destory this object

  // MANIPULATORS
  int start();
    // Return 0 if all benchmarks were run and non-zero otherwise. Note a non-zero code usually indicates
    // bad configuration.

  void report();
    // Emit to stdout collected benchmark statistics

  Misc& operator=(const Misc& rhs) = delete;
    // Assignment operator not provided
};

inline Misc::Misc(const Config& config, const LoadFile& file)
: d_file(file)
, d_stats(config)
{
}

} // namespace Benchmark
