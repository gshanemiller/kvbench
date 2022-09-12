#pragma once

// PURPOSE: Benchmark HOT: A Height Optimized Trie Index for Main-Memory Database Systems
//                         by Binna, Zangerle, Pichl, Specht and Leis
//
// CLASSES:
//  Benchmark::Hot:        Benchmark HOT (height optimized trie)
//                         https://github.com/speedskater/hot

#include <benchmark_report.h>

namespace Benchmark {

class HOT: public Report {
public:
  // CREATORS                                                                                                           
  HOT(const Config& config, const LoadFile& file, const std::string& description)                                     
  : Report(config, file, description)                                                                                   
  {                                                                                                                     
  }                                                                                                                     
                                                                                                                        
  virtual ~HOT() = default;                                                                                                   
    // Destory this object 

  // MANIPULATORS
  int start();
    // Return 0 if all benchmarks were run and non-zero otherwise. Note a non-zero code usually indicates
    // bad configuration.
};

} // namespace Benchmark
