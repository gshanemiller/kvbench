#pragma once

// PURPOSE: Benchmark HatTrie

#include <benchmark_report.h>

namespace Benchmark {

class HatTrie: public Report {
public:
  // CREATORS
  HatTrie(const Config& config, const LoadFile& file, const std::string& description)
  : Report(config, file, description)
  {
  }

  virtual ~HatTrie() = default;

  // MANIPULATORS
  virtual int start();
    // Return 0 if all benchmarks were run and non-zero otherwise. Note a non-zero code usually indicates
    // bad configuration.
};

} // namespace Benchmark
