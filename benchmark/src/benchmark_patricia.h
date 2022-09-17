#pragma once

#include <benchmark_report.h>

namespace Benchmark {

class patricia: public Report {
public:
  // CREATORS
  patricia(const Config& config, const std::string& description)                                     
  : Report(config, description)                                                                                   
  {                                                                                                                     
  }                                                                                                                     
                                                                                                                        
  virtual ~patricia() = default;                                                                                                   
    // Destory this object 

  // MANIPULATORS
  int start();
    // Return 0 if all benchmarks were run and non-zero otherwise. Note a non-zero code usually indicates
    // bad configuration.
};

} // namespace Benchmark
