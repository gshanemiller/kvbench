#pragma once

// PURPOSE: Benchmark configuration
//
// CLASSES:
//  Benchmark::Config: Holds all the values which combine to specify what is to be benchmarked and reported

#include <string>
#include <iostream>

namespace Benchmark {

struct Config {
  // DATA
  std::string   d_filename;         // file containing data for benchmarking
  unsigned long d_fileSizeBytes;    // size of input file in bytes
  std::string   d_format;           // file format of 'd_filename'
  std::string   d_dataStructure;    // data structure to benchmark
  std::string   d_hashAlgo;         // required for hashmap algos
  std::string   d_allocator;        // name of custom allocator
  bool          d_needHashAlgo;     // True if 'd_dataStructure' requires hash algo
  bool          d_customAllocator;  // True if a custom allocator not default malloc/free or std::allocator used
  unsigned      d_runs;             // How many timees to run each test
  unsigned      d_recordRuns;       // The last 'd_recordRuns' of 'd_runs' total are reported
  unsigned      d_verbosity;        // higher verbosity level gives more output

  // CREATORS
  Config();
    // Create Config object with default values

  // ASPECTS
  void print() const;
    // Pretty-print configuration to stdout.
};

// INLINE DEFINITIONS
inline
Config::Config()
: d_needHashAlgo(false)
, d_customAllocator(false)
, d_runs(10)
, d_recordRuns(1)
, d_verbosity(0)
{
}

// ASPECTS
inline
void Config::print() const {
  printf("config: {\n");
  printf("  filename     : \"%s\"\n", d_filename.c_str());
  printf("  fileSizeBytes: %lu,\n", d_fileSizeBytes);
  printf("  format       : \"%s\"\n", d_format.c_str());
  printf("  dataStructure: \"%s\"\n", d_dataStructure.c_str());
  printf("  hashAlgorithm: \"%s\"\n", d_hashAlgo.c_str());
  printf("  allocator    : \"%s\"\n", !d_allocator.empty() ? d_allocator.c_str() : "vanilla malloc or std::allocator");
  printf("  needsHashAlgo: %s,\n",  d_needHashAlgo ? "true": "false" );
  printf("  customAlloc  : %s,\n", d_customAllocator ? "true": "false" );
  printf("  runs         : %u,\n", d_runs);
  printf("  recordRuns   : %u,\n", d_recordRuns);
  printf("  verbosity    : %u\n", d_verbosity);
  printf("}\n");
}

} // namespace Benchmark
