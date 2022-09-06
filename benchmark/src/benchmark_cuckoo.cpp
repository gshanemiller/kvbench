#include <benchmark_cuckoo.h>
#include <benchmark_hashable_keys.h>
#include <benchmark_textscan.h>

#include <intel_skylake_pmu.h>

#include <mimalloc.h>

#include <cuckoohash_map.hh>

// +-----------------------------------------+-----------------------------------------------------------------------+
// | Typedef                                 | Comment                                                               |
// +-----------------------------------------+-----------------------------------------------------------------------+
// | CuckooXXhash_SliceBool_XX3_64BITS       | Cuckoo hash map Key=Slice<char>, Value=bool on std::allocator         |
// |                                         | using xxhash variant XX3_64BITS                                       |
// +-----------------------------------------+-----------------------------------------------------------------------+
// | CuckooXXhash_MIM_SliceBool_XX3_64BITS   | Cuckoo hash map Key=Slice<char>, Value=bool on Microsoft MIM allocator|
// |                                         | using xxhash variant XX3_64BITS                                       |
// +-----------------------------------------+-----------------------------------------------------------------------+
// | CuckooT1ha_SliceBool                    | Cuckoo hash map Key=Slice<char>, Value=bool on std::allocator         |
// |                                         | using hash t1ha variant t1ha()                                        |
// +-----------------------------------------+-----------------------------------------------------------------------+
// | CuckooT1ha_MIM_SliceBool                | Cuckoo hash map Key=Slice<char>, Value=bool on Microsoft MIM allocator|
// |                                         | using hash t1ha variant t1ha()                                        |
// +-----------------------------------------+-----------------------------------------------------------------------+
// | CuckooCity_SliceBool_CityHash64         | Cuckoo hash map Key=Slice<char>, Value=bool on std::allocator         |
// |                                         | using hash city variant CityHash64()                                  |
// +-----------------------------------------+-----------------------------------------------------------------------+
// | CuckooCity_MIM_SliceBool_CityHash64     | Cuckoo hash map Key=Slice<char>, Value=bool on Microsoft MIM allocator|
// |                                         | using hash city variant CityHash64()                                  |
// +-----------------------------------------+-----------------------------------------------------------------------+

typedef libcuckoo::cuckoohash_map<Benchmark::Slice<char>, bool, Benchmark::char_slice_xxhash_xx3_64bits,
  Benchmark::SliceEqual<Benchmark::Slice<char>>> CuckooXXhash_SliceBool_XX3_64BITS;
typedef libcuckoo::cuckoohash_map<Benchmark::Slice<char>, bool, Benchmark::char_slice_xxhash_xx3_64bits,
  Benchmark::SliceEqual<Benchmark::Slice<char>>, mi_stl_allocator<std::pair<const Benchmark::Slice<char>,bool>>> CuckooXXhash_MIM_SliceBool_XX3_64BITS;

typedef libcuckoo::cuckoohash_map<Benchmark::Slice<char>, bool, Benchmark::char_slice_t1ha,
  Benchmark::SliceEqual<Benchmark::Slice<char>>> CuckooT1ha_SliceBool;
typedef libcuckoo::cuckoohash_map<Benchmark::Slice<char>, bool, Benchmark::char_slice_t1ha,
  Benchmark::SliceEqual<Benchmark::Slice<char>>, mi_stl_allocator<std::pair<const Benchmark::Slice<char>,bool>>> CuckooT1ha_MIM_SliceBool;

typedef libcuckoo::cuckoohash_map<Benchmark::Slice<char>, bool, Benchmark::char_slice_city_cityhash64,
  Benchmark::SliceEqual<Benchmark::Slice<char>>> CuckooCity_SliceBool_CityHash64;
typedef libcuckoo::cuckoohash_map<Benchmark::Slice<char>, bool, Benchmark::char_slice_city_cityhash64,
  Benchmark::SliceEqual<Benchmark::Slice<char>>, mi_stl_allocator<std::pair<const Benchmark::Slice<char>,bool>>> CuckooCity_MIM_SliceBool_CityHash64;

template<typename T>
static int cuckoo_test_text_insert(unsigned runNumber, T& map, Intel::Stats& stats, const Benchmark::LoadFile& file) {
  Benchmark::Slice<char> word;
  Benchmark::TextScan scanner(file);
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
    map.insert(word, false);
  }

  timespec_get(&endTime, TIME_UTC);
  stats.record(label, scanner.count(), startTime, endTime, pmu); 

  return 0;
}

template<typename T>
static int cuckoo_test_text_find(unsigned runNumber, T& map, Intel::Stats& stats, const Benchmark::LoadFile& file) {
  Benchmark::Slice<char> word;
  Benchmark::TextScan scanner(file);
  Intel::SkyLake::PMU pmu(false, Intel::SkyLake::PMU::ProgCounterSetConfig::k_DEFAULT_SKYLAKE_CONFIG_0);

  char label[128];
  snprintf(label, sizeof(label), "find run %u", runNumber);

  timespec startTime;
  timespec endTime;
  pmu.reset();
  timespec_get(&startTime, TIME_UTC);
  pmu.start();

  // Benchmark running: do find
  bool value;
  for (scanner.next(word); !scanner.eof(); scanner.next(word)) {
    value = map.find(word);
    Intel::DoNotOptimize(value);
  }

  timespec_get(&endTime, TIME_UTC);
  stats.record(label, scanner.count(), startTime, endTime, pmu); 

  return 0;
}

int Benchmark::Cuckoo::start() {
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
          CuckooXXhash_MIM_SliceBool_XX3_64BITS map;
          cuckoo_test_text_insert(i, map, d_insertStats, d_file);
          cuckoo_test_text_find(i, map, d_findStats, d_file);
        }
      } else if (d_config.d_hashAlgo=="t1ha::t1ha") {
        printf("made it\n");
        // MIM alloc + t1ha
        for (unsigned i=0; i<d_config.d_runs; ++i) {
          if (d_config.d_verbosity>0) {
            printf("execute run set %u...\n", i);
          }
          CuckooT1ha_MIM_SliceBool map;
          cuckoo_test_text_insert(i, map, d_insertStats, d_file);
          cuckoo_test_text_find(i, map, d_findStats, d_file);
        }
      } else if (d_config.d_hashAlgo=="city::cityhash64") {
        // MIM alloc + cityhash64
        for (unsigned i=0; i<d_config.d_runs; ++i) {
          if (d_config.d_verbosity>0) {
            printf("execute run set %u...\n", i);
          }
          CuckooCity_MIM_SliceBool_CityHash64 map;
          cuckoo_test_text_insert(i, map, d_insertStats, d_file);
          cuckoo_test_text_find(i, map, d_findStats, d_file);
        }
      }
    } else {
      if (d_config.d_hashAlgo=="xxhash:XX3_64bits") {
        // std alloc + xxhash
        for (unsigned i=0; i<d_config.d_runs; ++i) {
          if (d_config.d_verbosity>0) {
            printf("execute run set %u...\n", i);
          }
          CuckooXXhash_SliceBool_XX3_64BITS map;
          cuckoo_test_text_insert(i, map, d_insertStats, d_file);
          cuckoo_test_text_find(i, map, d_findStats, d_file);
        }
      } else if (d_config.d_hashAlgo=="t1ha::t1ha") {
        printf("made it std\n");
        // std alloc + t1ha
        for (unsigned i=0; i<d_config.d_runs; ++i) {
          if (d_config.d_verbosity>0) {
            printf("execute run set %u...\n", i);
          }
          CuckooT1ha_SliceBool map;
          cuckoo_test_text_insert(i, map, d_insertStats, d_file);
          cuckoo_test_text_find(i, map, d_findStats, d_file);
        }
      } else if (d_config.d_hashAlgo=="city::cityhash64") {
        // std alloc + cityhash64
        for (unsigned i=0; i<d_config.d_runs; ++i) {
          if (d_config.d_verbosity>0) {
            printf("execute run set %u...\n", i);
          }
          CuckooCity_SliceBool_CityHash64 map;
          cuckoo_test_text_insert(i, map, d_insertStats, d_file);
          cuckoo_test_text_find(i, map, d_findStats, d_file);
        }
      }
    }
  }
  return rc;
}

void Benchmark::Cuckoo::report() {
  Intel::SkyLake::PMU pmu(false, Intel::SkyLake::PMU::ProgCounterSetConfig::k_DEFAULT_SKYLAKE_CONFIG_0);
  d_config.print();
  d_insertStats.summary("Cuckoo Insert", pmu);                                                                          
  d_findStats.summary("Cuckoo Find", pmu); 
}
