#pragma once

// PURPOSE: Base class for collecting stats

#include <benchmark_config.h>
#include <benchmark_loadfile.h>
#include <intel_pmu_stats.h>

namespace Benchmark {

class Report {
public:
  // DATA
  const LoadFile&     d_file;
  const Config&       d_config;                                                                                             
  const std::string   d_description;
  Intel::Stats        d_findStats;
  Intel::Stats        d_insertStats;

  // CREATORS
  explicit Report(const Config& config, const LoadFile& file, const std::string& description);
    // Create Report benchmark object with specified 'config, file'

  Report(const Report& other) = delete;
    // Copy constructor not provided

  virtual ~Report() = default;
    // Destory this object

  // MANIPULATORS
  virtual int start();
    // Return 0 if all benchmarks were run and non-zero otherwise. Note a non-zero code usually indicates
    // bad configuration.

  std::ostream& rusage(std::ostream& stream);
    // Print to specified 'stream' select rusage stats take at time of call returning stream

  virtual void report();
    // Emit to stdout collected benchmark statistics

  Report& operator=(const Report& rhs) = delete;
    // Assignment operator not provided
};

inline
Report::Report(const Config& config, const LoadFile& file, const std::string& description)
: d_file(file)
, d_config(config)
, d_description(description)
{
}

} // namespace Benchmark
