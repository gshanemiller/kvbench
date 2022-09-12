#pragma once

// PURPOSE: Benchmark cuckoo hashmap
//
// CLASSES:
//  Benchmark::Cuckoo: Benchmark the Cuckoo hashing algorithm

#include <benchmark_report.h>

namespace Benchmark {

class Cuckoo: public Report {
public:
  // CREATORS                                                                                                           
  Cuckoo(const Config& config, const LoadFile& file, const std::string& description)                                     
  : Report(config, file, description)                                                                                   
  {                                                                                                                     
  }                                                                                                                     
                                                                                                                        
  virtual ~Cuckoo() = default;                                                                                                   
    // Destory this object 

  // MANIPULATORS
  virtual int start();
    // Return 0 if all benchmarks were run and non-zero otherwise. Note a non-zero code usually indicates
    // bad configuration.
};

} // namespace Benchmark
