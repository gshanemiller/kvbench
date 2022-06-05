#include <benchmark_hot.h>
#include <benchmark_hashable_keys.h>
#include <benchmark_textscan.h>

#include <intel_skylake_pmu.h>

#include <mimalloc.h>

#pragma GCC diagnostic push                                                                                             
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#pragma GCC diagnostic ignored "-Wall"
#pragma GCC diagnostic ignored "-Wextra"
#pragma GCC diagnostic ignored "-Wpedantic"
#include <HOTSingleThreaded.hpp>
#include <IdentityKeyExtractor.hpp>
#pragma GCC diagnostic pop 

// +--------------------------------------------+----------------------------------------------------------------------------+
// | Typedef                                    | Comment                                                                    |
// +--------------------------------------------+----------------------------------------------------------------------------+
// | HOTXXhash_SliceBool_XX3_64BITS     | HOT hash map Key=Slice<char>, Value=bool on std::allocator         |
// |                                            | using xxhash variant XX3_64BITS                                            |
// +--------------------------------------------+----------------------------------------------------------------------------+
// | HOTXXhash_MIM_SliceBool_XX3_64BITS | HOT hash map Key=Slice<char>, Value=bool on Microsoft MIM allocator|
// |                                            | using xxhash variant XX3_64BITS                                            |
// +--------------------------------------------+----------------------------------------------------------------------------+
// | HOTT1ha_SliceBool                  | HOT hash map Key=Slice<char>, Value=bool on std::allocator         |
// |                                            | using hash t1ha variant t1ha()                                             |
// +--------------------------------------------+----------------------------------------------------------------------------+
// | HOTT1ha_MIM_SliceBool              | HOT hash map Key=Slice<char>, Value=bool on Microsoft MIM allocator|
// |                                            | using hash t1ha variant t1ha()                                             |
// +--------------------------------------------+----------------------------------------------------------------------------+
// | HOTCity_SliceBool_CityHash64       | HOT hash map Key=Slice<char>, Value=bool on std::allocator         |
// |                                            | using hash city variant CityHash64()                                       |
// +--------------------------------------------+----------------------------------------------------------------------------+
// | HOTCity_MIM_SliceBool_CityHash64   | HOT hash map Key=Slice<char>, Value=bool on Microsoft MIM allocator|
// |                                            | using hash city variant CityHash64()                                       |
// +--------------------------------------------+----------------------------------------------------------------------------+

template<typename ValueType>
struct SliceExtractor {
  typedef ValueType KeyType;
  inline KeyType operator()(ValueType const &value) const {
    return (ValueType)(value->data());
  }
};

namespace idx {
namespace contenthelpers {

template<> inline size_t getKeyLength<const Benchmark::Slice<char> *>(const Benchmark::Slice<char> * const & key) {
  return std::min<size_t>(key->size(), MAX_STRING_KEY_LENGTH);
}

} // contenthelpers
} // idx

typedef hot::singlethreaded::HOTSingleThreaded<const Benchmark::Slice<char>*, SliceExtractor> HOTTrie;

template<typename T>
static int hot_test_text_insert(unsigned runNumber, T& map, Benchmark::Stats& stats, const Benchmark::LoadFile& file) {
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
    map.insert(&word);
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
static int hot_test_text_find(unsigned runNumber, T& map, Benchmark::Stats& stats, const Benchmark::LoadFile& file) {
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
    auto iter = map.find(&word);
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

int Benchmark::HOT::start() {
  int rc(0);

  if (d_stats.config().d_format == "bin-text-kv") {
    // We have KV pairs to play with
    // Not implemented yet
    return rc;
  } else if (d_stats.config().d_format=="bin-text") {
    // We have a text file therefore we can only benchamrk key ins/upd/fnd/del on keys.
    // Make a cuckoo map with the smallest possible value type (bool) and set it to a 
    // constant value throughout all tests.
    if (d_stats.config().d_customAllocator) {
      return rc;
    } else {
      for (unsigned i=0; i<d_stats.config().d_runs; ++i) {
        if (d_stats.config().d_verbosity>0) {
          printf("execute run set %u...\n", i);
        }
        HOTTrie hotTrie;
        hot_test_text_insert(i, hotTrie, d_stats, d_file);
        hot_test_text_find(i, hotTrie, d_stats, d_file);
        std::cout << "height:" << hotTrie.getHeight() << std::endl;
        std::pair<size_t, std::map<std::string, double>> stats = hotTrie.getStatistics();
        auto iter = stats.second.begin();
        auto eiter = stats.second.end();
        for(; iter!=eiter; ++iter) {
          std::cout << iter->first << " " << iter->second << std::endl;
        }
      }
    }
  }
  return rc;
}

void Benchmark::HOT::report() {
  Intel::SkyLake::PMU pmu(false, Intel::SkyLake::PMU::ProgCounterSetConfig::k_DEFAULT_SKYLAKE_CONFIG_0);
  d_stats.print(pmu);
}
