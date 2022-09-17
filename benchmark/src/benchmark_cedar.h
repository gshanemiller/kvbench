#pragma once

#include <benchmark_report.h>

namespace Benchmark {

class Cedar: public Report {
public:
  // CREATORS
  Cedar(const Config& config, const std::string& description)
  : Report(config, description)
  {
  }

  virtual ~Cedar() = default;
    // Destroy this object

  // MANIPULATORS
  virtual int start();
    // Return 0 if all benchmarks were run and non-zero otherwise. Note a non-zero code usually indicates
    // bad configuration.
};

} // namespace Benchmark
