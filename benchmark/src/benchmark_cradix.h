#pragma once

#include <benchmark_config.h>
#include <benchmark_loadfile.h>
#include <benchmark_stats.h>

#include <cradix_tree.h>

namespace Benchmark {

class cradix {
  // DATA
  const LoadFile& d_file;
  Stats           d_stats;

public:
  // CREATORS
  explicit cradix(const Config& config, const LoadFile& file);
    // Create cradix benchmark object with specified 'config, file'

  cradix(const cradix& other) = delete;
    // Copy constructor not provided

  ~cradix() = default;
    // Destory this object

  // MANIPULATORS
  int start();
    // Return 0 if all benchmarks were run and non-zero otherwise. Note a non-zero code usually indicates
    // bad configuration.

  void report();
    // Emit to stdout collected benchmark statistics

  cradix& operator=(const cradix& rhs) = delete;
    // Assignment operator not provided
};

inline cradix::cradix(const Config& config, const LoadFile& file)
: d_file(file)
, d_stats(config)
{
}

} // namespace Benchmark