#include <benchmark_f14.h>
#include <benchmark_hashable_keys.h>
#include <benchmark_textscan.h>

#include <intel_skylake_pmu.h>

#include <mimalloc.h>

#include <F14Map.h>

// +--------------------------------------------+----------------------------------------------------------------------------+
// | Typedef                                    | Comment                                                                    |
// +--------------------------------------------+----------------------------------------------------------------------------+
// | FacebookF14XXhash_SliceBool_XX3_64BITS     | FacebookF14 hash map Key=Slice<char>, Value=bool on std::allocator         |
// |                                            | using xxhash variant XX3_64BITS                                            |
// +--------------------------------------------+----------------------------------------------------------------------------+
// | FacebookF14XXhash_MIM_SliceBool_XX3_64BITS | FacebookF14 hash map Key=Slice<char>, Value=bool on Microsoft MIM allocator|
// |                                            | using xxhash variant XX3_64BITS                                            |
// +--------------------------------------------+----------------------------------------------------------------------------+
// | FacebookF14T1ha_SliceBool                  | FacebookF14 hash map Key=Slice<char>, Value=bool on std::allocator         |
// |                                            | using hash t1ha variant t1ha()                                             |
// +--------------------------------------------+----------------------------------------------------------------------------+
// | FacebookF14T1ha_MIM_SliceBool              | FacebookF14 hash map Key=Slice<char>, Value=bool on Microsoft MIM allocator|
// |                                            | using hash t1ha variant t1ha()                                             |
// +--------------------------------------------+----------------------------------------------------------------------------+
// | FacebookF14City_SliceBool_CityHash64       | FacebookF14 hash map Key=Slice<char>, Value=bool on std::allocator         |
// |                                            | using hash city variant CityHash64()                                       |
// +--------------------------------------------+----------------------------------------------------------------------------+
// | FacebookF14City_MIM_SliceBool_CityHash64   | FacebookF14 hash map Key=Slice<char>, Value=bool on Microsoft MIM allocator|
// |                                            | using hash city variant CityHash64()                                       |
// +--------------------------------------------+----------------------------------------------------------------------------+

typedef folly::F14ValueMap<Benchmark::Slice<char>, bool, Benchmark::char_slice_xxhash_xx3_64bits,
  Benchmark::SliceEqual<Benchmark::Slice<char>>> FacebookF14XXhash_SliceBool_XX3_64BITS;
typedef folly::F14ValueMap<Benchmark::Slice<char>, bool, Benchmark::char_slice_xxhash_xx3_64bits,
  Benchmark::SliceEqual<Benchmark::Slice<char>>, mi_stl_allocator<std::pair<const Benchmark::Slice<char>,bool>>> FacebookF14XXhash_MIM_SliceBool_XX3_64BITS;

typedef folly::F14ValueMap<Benchmark::Slice<char>, bool, Benchmark::char_slice_t1ha,
  Benchmark::SliceEqual<Benchmark::Slice<char>>> FacebookF14T1ha_SliceBool;
typedef folly::F14ValueMap<Benchmark::Slice<char>, bool, Benchmark::char_slice_t1ha,
  Benchmark::SliceEqual<Benchmark::Slice<char>>, mi_stl_allocator<std::pair<const Benchmark::Slice<char>,bool>>> FacebookF14T1ha_MIM_SliceBool;

typedef folly::F14ValueMap<Benchmark::Slice<char>, bool, Benchmark::char_slice_city_cityhash64,
  Benchmark::SliceEqual<Benchmark::Slice<char>>> FacebookF14City_SliceBool_CityHash64;
typedef folly::F14ValueMap<Benchmark::Slice<char>, bool, Benchmark::char_slice_city_cityhash64,
  Benchmark::SliceEqual<Benchmark::Slice<char>>, mi_stl_allocator<std::pair<const Benchmark::Slice<char>,bool>>> FacebookF14City_MIM_SliceBool_CityHash64;

template<typename T>
static int f14_test_text_insert(unsigned runNumber, T& map, Intel::Stats& stats, const Benchmark::LoadFile& file) {
  Benchmark::Slice<char> word;
  Benchmark::TextScan<char> scanner(file);
  Intel::SkyLake::PMU pmu(false, Intel::SkyLake::PMU::ProgCounterSetConfig::k_DEFAULT_SKYLAKE_CONFIG_0);

  char label[128];
  snprintf(label, sizeof(label), "insert run %u", runNumber);

  timespec startTime;
  timespec endTime;
  pmu.reset();
  timespec_get(&startTime, TIME_UTC);
  pmu.start();

  // Benchmark running: do insert
  for (scanner.next(word); !scanner.eof(); scanner.next(word)) {
    map.insert(std::pair(word, false));
  }

  timespec_get(&endTime, TIME_UTC);
  stats.record(label, scanner.index(), startTime, endTime, pmu);

  return 0;
}

template<typename T>
static int f14_test_text_find(unsigned runNumber, T& map, Intel::Stats& stats, const Benchmark::LoadFile& file) {
  Benchmark::Slice<char> word;
  Benchmark::TextScan<char> scanner(file);
  Intel::SkyLake::PMU pmu(false, Intel::SkyLake::PMU::ProgCounterSetConfig::k_DEFAULT_SKYLAKE_CONFIG_0);

  char label[128];
  snprintf(label, sizeof(label), "find run %u", runNumber);

  timespec startTime;
  timespec endTime;
  pmu.reset();
  timespec_get(&startTime, TIME_UTC);
  pmu.start();

  // Benchmark running: do find
  for (scanner.next(word); !scanner.eof(); scanner.next(word)) {
    auto iter = map.find(word);
    Intel::DoNotOptimize(iter);
  }

  timespec_get(&endTime, TIME_UTC);
  stats.record(label, scanner.index(), startTime, endTime, pmu);

  return 0;
}

int Benchmark::FacebookF14::start() {
  int rc(0);

  if (d_config.d_format == "bin-text-kv") {
    // We have KV pairs to play with
    // Not implemented yet
    return rc;
  } else if (d_config.d_format=="bin-text") {
    // We have a text file therefore we can only benchamrk key ins/upd/fnd/del on keys.
    // Make a cuckoo map with the smallest possible value type (bool) and set it to a 
    // constant value throughout all tests.
    if (d_config.d_customAllocator) {
      if (d_config.d_hashAlgo=="xxhash:XX3_64bits") {
        // MIM alloc + xxhash
        for (unsigned i=0; i<d_config.d_runs; ++i) {
          if (d_config.d_verbosity>0) {
            printf("execute run set %u...\n", i);
          }
          FacebookF14XXhash_MIM_SliceBool_XX3_64BITS map;
          f14_test_text_insert(i, map, d_insertStats, d_file);
          f14_test_text_find(i, map, d_findStats, d_file);
        }
      } else if (d_config.d_hashAlgo=="t1ha::t1ha") {
        // MIM alloc + t1ha
        for (unsigned i=0; i<d_config.d_runs; ++i) {
          if (d_config.d_verbosity>0) {
            printf("execute run set %u...\n", i);
          }
          FacebookF14T1ha_MIM_SliceBool map;
          f14_test_text_insert(i, map, d_insertStats, d_file);
          f14_test_text_find(i, map, d_findStats, d_file);
        }
      } else if (d_config.d_hashAlgo=="city::cityhash64") {
        // MIM alloc + cityhash64
        for (unsigned i=0; i<d_config.d_runs; ++i) {
          if (d_config.d_verbosity>0) {
            printf("execute run set %u...\n", i);
          }
          FacebookF14City_MIM_SliceBool_CityHash64 map;
          f14_test_text_insert(i, map, d_insertStats, d_file);
          f14_test_text_find(i, map, d_findStats, d_file);
        }
      }
    } else {
      if (d_config.d_hashAlgo=="xxhash:XX3_64bits") {
        // std alloc + xxhash
        for (unsigned i=0; i<d_config.d_runs; ++i) {
          if (d_config.d_verbosity>0) {
            printf("execute run set %u...\n", i);
          }
          FacebookF14XXhash_SliceBool_XX3_64BITS map;
          f14_test_text_insert(i, map, d_insertStats, d_file);
          f14_test_text_find(i, map, d_findStats, d_file);
        }
      } else if (d_config.d_hashAlgo=="t1ha::t1ha") {
        // std alloc + t1ha
        for (unsigned i=0; i<d_config.d_runs; ++i) {
          if (d_config.d_verbosity>0) {
            printf("execute run set %u...\n", i);
          }
          FacebookF14T1ha_SliceBool map;
          f14_test_text_insert(i, map, d_insertStats, d_file);
          f14_test_text_find(i, map, d_findStats, d_file);
        }
      } else if (d_config.d_hashAlgo=="city::cityhash64") {
        // std alloc + cityhash64
        for (unsigned i=0; i<d_config.d_runs; ++i) {
          if (d_config.d_verbosity>0) {
            printf("execute run set %u...\n", i);
          }
          FacebookF14City_SliceBool_CityHash64 map;
          f14_test_text_insert(i, map, d_insertStats, d_file);
          f14_test_text_find(i, map, d_findStats, d_file);
        }
      }
    }
  }
  return rc;
}

void Benchmark::FacebookF14::report() {
  Intel::SkyLake::PMU pmu(false, Intel::SkyLake::PMU::ProgCounterSetConfig::k_DEFAULT_SKYLAKE_CONFIG_0);
  d_config.print();
  d_insertStats.summary("F14 Insert", pmu);
  d_findStats.summary("F14 Find", pmu);
}
