#pragma once

#include <benchmark_report.h>

namespace Benchmark {

class cradix: public Report {
public:
  // CREATORS                                                                                                           
  cradix(const Config& config, const std::string& description)
  : Report(config, description)
  {
  }
                                                                                                                        
  virtual ~cradix() = default;                                                                                                   
    // Destory this object 

  // MANIPULATORS
  virtual int start();
    // Return 0 if all benchmarks were run and non-zero otherwise. Note a non-zero code usually indicates
    // bad configuration.
};

} // namespace Benchmark
