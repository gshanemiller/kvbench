#pragma once

// PURPOSE: Base class for collecting stats

#include <benchmark_config.h>
#include <benchmark_loadfile.h>
#include <intel_pmu_stats.h>

namespace Benchmark {

class Report {
public:
  // DATA
  const Config&       d_config;                                                                                             
  const std::string   d_description;
  LoadFile            d_file;
  Intel::Stats        d_findStats;
  Intel::Stats        d_insertStats;

  // CREATORS
  explicit Report(const Config& config, const std::string& description);
    // Create Report benchmark object with specified 'config, file'

  Report(const Report& other) = delete;
    // Copy constructor not provided

  virtual ~Report() = default;
    // Destory this object

  // MANIPULATORS
  virtual int loadFile(const char *path);
    // Return 0 if specified file in 'path' was loaded into 'd_file' and non-zero otherwise

  virtual int start();
    // Return 0 if all benchmarks were run and non-zero otherwise. Note a non-zero code usually indicates
    // bad configuration.

  virtual void report();
    // Emit to stdout collected benchmark statistics

  Report& operator=(const Report& rhs) = delete;
    // Assignment operator not provided

  // STATIC FUNCTIONS
  static std::ostream& rusage(std::ostream& stream, const char *label=0);
    // Print to specified 'stream' selected rusage stats take at time of call returning stream
    // If 'label' is non-zero it's emitted to 'stream' before dumping rusage
};

inline
Report::Report(const Config& config, const std::string& description)
: d_config(config)
, d_description(description)
{
}

} // namespace Benchmark
