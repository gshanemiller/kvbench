#pragma once

// PURPOSE: Benchmark Wormhole

#include <benchmark_report.h>

namespace Benchmark {

class WormHole: public Report {
public:
  // CREATORS
  WormHole(const Config& config, const std::string& description)
  : Report(config, description)
  {
  }

  virtual ~WormHole() = default;

  // MANIPULATORS
  virtual int start();
    // Return 0 if all benchmarks were run and non-zero otherwise. Note a non-zero code usually indicates
    // bad configuration.
};

} // namespace Benchmark
