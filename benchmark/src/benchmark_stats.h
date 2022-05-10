#pragma once

// PURPOSE: Report collected statistics
//
// CLASSES:
//  Benchmark::Stats: Holds raw statistics from each test and reports them to standard out.

#include <benchmark_config.h>
#include <intel_skylake_pmu.h>

#include <string>
#include <vector>
#include <iostream>
#include <inttypes.h>
#include <time.h>

namespace Benchmark {

class Stats {
  // DATA
  const Config&               d_config;       // config benchmark run with
  std::vector<std::string>    d_description;  // per result set: description e.g. 'insert in cuckoo hashmap'
  std::vector<unsigned long>  d_itertions;    // per result set: number of iterations run
  std::vector<u_int64_t>      d_fixedCntr0;   // per result set: value of fixed counter 0 at test end
  std::vector<u_int64_t>      d_fixedCntr1;   // per result set: value of fixed counter 1 at test end
  std::vector<u_int64_t>      d_fixedCntr2;   // per result set: value of fixed counter 2 at test end
  std::vector<u_int64_t>      d_progmCntr0;   // per result set: value of programmable counter 0 at test end
  std::vector<u_int64_t>      d_progmCntr1;   // per result set: value of programmable counter 1 at test end
  std::vector<u_int64_t>      d_progmCntr2;   // per result set: value of programmable counter 2 at test end
  std::vector<u_int64_t>      d_progmCntr3;   // per result set: value of programmable counter 3 at test end
  std::vector<double>         d_elapsedNs;    // per result set: elapsed time in nanoseconds

  // CREATORS
public:
  Stats();
    // Default constructor not defined

  explicit Stats(const Config& config);
    // Create Stats object to hold the benchmark data for 0 or more result sets run with specified 'config'

  Stats(const Stats& other);
    // Copy-constructor not defined

  // ACCESSORS
  const Config& config() const;
    // Return non-modifiable reference to configuration provided at construction time.

  // MANIPULATORS
  void addResultSet(const char *desc,
                    unsigned long iterations,
                    timespec start,
                    timespec end,
                    u_int64_t pmuFixedCounter0Value,
                    u_int64_t pmuFixedCounter1Value,
                    u_int64_t pmuFixedCounter2Value,
                    u_int64_t pmuProgrammableCounter0Value,
                    u_int64_t pmuProgrammableCounter1Value,
                    u_int64_t pmuProgrammableCounter2Value,
                    u_int64_t pmuProgrammableCounter3Value);
    // Record specified benchmark data for a single run set. 'desc' is a description e.g. 'insert into cuckoo hashmap'.
    // Specified 'iterations' is the number of times 'desc' was run. 'start, end' record the timestamp at beginning and
    // end of the run. 'end-start' is the elapsed time. The seven 'u_int64_t' specified arguments give the current 
    // value of the name PMU counter at test termination. Note this method assumes no PMU counter overflowed. Also note
    // the PMU counter values must correspond to 'pmu' provided at construction time.

private:
  void summarize(const Intel::SkyLake::PMU& pmu, unsigned i) const;
    // Summarize ith result set to stdout.

  void legend(const Intel::SkyLake::PMU& pmu) const;
    // Print programmable counter legend

  // ASPECTS
public:
  void print(const Intel::SkyLake::PMU& pmu) const;
    // Pretty-print a human readable synopsis of collected result with counters described by 'pmu'
};

// INLINE DEFINITIONS

// CREATORS
inline
Stats::Stats(const Config& config)
: d_config(config)
{
}

// ACCESSORS
inline
const Config& Stats::config() const {
  return d_config;
}

} // namespace Benchmark
