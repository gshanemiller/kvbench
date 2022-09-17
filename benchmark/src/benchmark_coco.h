#pragma once

// PURPOSE: Benchmark CoCo tries

#include <benchmark_report.h>

namespace Benchmark {

class Coco: public Report {
public:
  // CREATORS
  Coco(const Config& config, const std::string& description)
  : Report(config, description)
  {
  }

  virtual ~Coco() = default;

  // MANIPULATORS
  virtual int start();
    // Return 0 if all benchmarks were run and non-zero otherwise. Note a non-zero code usually indicates
    // bad configuration.
};

} // namespace Benchmark
