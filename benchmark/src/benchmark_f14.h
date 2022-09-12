#pragma once

// PURPOSE: Benchmark Facebook's F14 hashmap
//
// CLASSES:
//  Benchmark::FacebookF14: Benchmark Facebooks F14 hash map
//                          https://github.com/facebook/folly/blob/main/folly/container/F14.md
//                          https://news.ycombinator.com/item?id=19759630

#include <benchmark_report.h>

namespace Benchmark {

class FacebookF14: public Report {
public:
  // CREATORS                                                                                                           
  FacebookF14(const Config& config, const LoadFile& file, const std::string& description)                                     
  : Report(config, file, description)                                                                                   
  {                                                                                                                     
  }                                                                                                                     
                                                                                                                        
  virtual ~FacebookF14() = default;                                                                                                   
    // Destory this object 

  // MANIPULATORS
  int start();
    // Return 0 if all benchmarks were run and non-zero otherwise. Note a non-zero code usually indicates
    // bad configuration.
};

} // namespace Benchmark
