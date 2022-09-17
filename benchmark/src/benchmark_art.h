#pragma once

// PURPOSE: Benchmark ART: Adaptive Radix Trie

#include <benchmark_report.h>

namespace Benchmark {

class ART: public Report {
public:
  // CREATORS
  ART(const Config& config, const std::string& description)
  : Report(config, description)
  {
  }

  virtual ~ART() = default;

  // MANIPULATORS
  virtual int start();
    // Return 0 if all benchmarks were run and non-zero otherwise. Note a non-zero code usually indicates
    // bad configuration.
};

} // namespace Benchmark
