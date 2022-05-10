#include <benchmark_jart.h>
#include <benchmark_hashable_keys.h>
#include <benchmark_textscan.h>

#include <intel_skylake_pmu.h>

#include <mimalloc.h>

#include <art_map.h>

// +--------------------------------------------+----------------------------------------------------------------------------+
// | Typedef                                    | Comment                                                                    |
// +--------------------------------------------+----------------------------------------------------------------------------+
// | JArtXXhash_SliceBool_XX3_64BITS     | JArt hash map Key=Slice<char>, Value=bool on std::allocator         |
// |                                            | using xxhash variant XX3_64BITS                                            |
// +--------------------------------------------+----------------------------------------------------------------------------+
// | JArtXXhash_MIM_SliceBool_XX3_64BITS | JArt hash map Key=Slice<char>, Value=bool on Microsoft MIM allocator|
// |                                            | using xxhash variant XX3_64BITS                                            |
// +--------------------------------------------+----------------------------------------------------------------------------+
// | JArtT1ha_SliceBool                  | JArt hash map Key=Slice<char>, Value=bool on std::allocator         |
// |                                            | using hash t1ha variant t1ha()                                             |
// +--------------------------------------------+----------------------------------------------------------------------------+
// | JArtT1ha_MIM_SliceBool              | JArt hash map Key=Slice<char>, Value=bool on Microsoft MIM allocator|
// |                                            | using hash t1ha variant t1ha()                                             |
// +--------------------------------------------+----------------------------------------------------------------------------+
// | JArtCity_SliceBool_CityHash64       | JArt hash map Key=Slice<char>, Value=bool on std::allocator         |
// |                                            | using hash city variant CityHash64()                                       |
// +--------------------------------------------+----------------------------------------------------------------------------+
// | JArtCity_MIM_SliceBool_CityHash64   | JArt hash map Key=Slice<char>, Value=bool on Microsoft MIM allocator|
// |                                            | using hash city variant CityHash64()                                       |
// +--------------------------------------------+----------------------------------------------------------------------------+

typedef art::map<Benchmark::Slice<const char*>, bool> jart_map;

template<typename T>
static int jart_test_text_insert(unsigned runNumber, T& map, Benchmark::Stats& stats, const Benchmark::LoadFile& file) {
  Benchmark::Slice<char> word;

  Benchmark::TextScan scanner(file);

  Intel::SkyLake::PMU pmu(false, Intel::SkyLake::PMU::ProgCounterSetConfig::k_DEFAULT_SKYLAKE_CONFIG_0);

  timespec startTime;
  timespec endTime;
  pmu.reset();
  timespec_get(&startTime, TIME_UTC);
  pmu.start();

  // Benchmark running: do insert
  while (0==scanner.next(word)) {
    map.insert(std::pair(word, false));
  }

  // Benchmark done: take stats
  u_int64_t f0 = pmu.fixedCounterValue(0);
  u_int64_t f1 = pmu.fixedCounterValue(1);
  u_int64_t f2 = pmu.fixedCounterValue(2);

  u_int64_t p0 = pmu.programmableCounterValue(0);
  u_int64_t p1 = pmu.programmableCounterValue(1);
  u_int64_t p2 = pmu.programmableCounterValue(2);
  u_int64_t p3 = pmu.programmableCounterValue(3);

  timespec_get(&endTime, TIME_UTC);

  if (stats.config().d_runs-runNumber<=stats.config().d_recordRuns) {
    char label[128];
    snprintf(label, sizeof(label), "insert run %u", runNumber);
    stats.addResultSet(label, scanner.count(), startTime, endTime, f0, f1, f2, p0, p1, p2, p3);
  }
  return 0;
}

template<typename T>
static int jart_test_text_find(unsigned runNumber, T& map, Benchmark::Stats& stats, const Benchmark::LoadFile& file) {
  Benchmark::Slice<char> word;

  Benchmark::TextScan scanner(file);

  Intel::SkyLake::PMU pmu(false, Intel::SkyLake::PMU::ProgCounterSetConfig::k_DEFAULT_SKYLAKE_CONFIG_0);

  timespec startTime;
  timespec endTime;
  pmu.reset();
  timespec_get(&startTime, TIME_UTC);
  pmu.start();

  // Benchmark running: do find
  while (0==scanner.next(word)) {
    auto iter = map.find(word);
    Intel::DoNotOptimize(iter);
  }

  // Benchmark done: take stats
  u_int64_t f0 = pmu.fixedCounterValue(0);
  u_int64_t f1 = pmu.fixedCounterValue(1);
  u_int64_t f2 = pmu.fixedCounterValue(2);

  u_int64_t p0 = pmu.programmableCounterValue(0);
  u_int64_t p1 = pmu.programmableCounterValue(1);
  u_int64_t p2 = pmu.programmableCounterValue(2);
  u_int64_t p3 = pmu.programmableCounterValue(3);

  timespec_get(&endTime, TIME_UTC);

  if (stats.config().d_runs-runNumber<=stats.config().d_recordRuns) {
    char label[128];
    snprintf(label, sizeof(label), "find run %u", runNumber);
    stats.addResultSet(label, scanner.count(), startTime, endTime, f0, f1, f2, p0, p1, p2, p3);
  }
  return 0;
}

int Benchmark::JArt::start() {
  int rc(0);

  if (d_stats.config().d_format == "bin-text-kv") {
    // We have KV pairs to play with
    // Not implemented yet
    return rc;
  } else if (d_stats.config().d_format=="bin-text") {
    // We have a text file therefore we can only benchamrk key ins/upd/fnd/del on keys.
    // Make a ART map with the smallest possible value type (bool) and set it to a 
    // constant value throughout all tests.
    if (d_stats.config().d_customAllocator) {
      return rc; 
    } else {
      for (unsigned i=0; i<d_stats.config().d_runs; ++i) {
        if (d_stats.config().d_verbosity>0) {
          printf("execute run set %u...\n", i);
        }
        jart_map map;
        jart_test_text_insert(i, map, d_stats, d_file);
        jart_test_text_find(i, map, d_stats, d_file);
      }
    }
  }
  return rc;
}

void Benchmark::JArt::report() {
  Intel::SkyLake::PMU pmu(false, Intel::SkyLake::PMU::ProgCounterSetConfig::k_DEFAULT_SKYLAKE_CONFIG_0);
  d_stats.print(pmu);
}
