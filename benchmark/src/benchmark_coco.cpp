#include <benchmark_coco.h>
#include <benchmark_hashable_keys.h>
#include <benchmark_textscan.h>

#include <intel_skylake_pmu.h>

template<typename T>
static int coco_testtext_insert(unsigned runNumber, T& map, Intel::Stats& stats, const Benchmark::LoadFile& file) {
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
    // map.insert_ks(word.const_data(), word.size(), scanner.index());
  }

  timespec_get(&endTime, TIME_UTC);
  stats.record(label, scanner.index(), startTime, endTime, pmu);

  return 0;
}

template<typename T>
static int coco_testtext_find(unsigned runNumber, T& map, Intel::Stats& stats, const Benchmark::LoadFile& file) {
  Benchmark::Slice<char> word;
  Benchmark::TextScan<char> scanner(file);
  Intel::SkyLake::PMU pmu(false, Intel::SkyLake::PMU::ProgCounterSetConfig::k_DEFAULT_SKYLAKE_CONFIG_0);

  unsigned int errors(0);
  char label[128];
  snprintf(label, sizeof(label), "find run %u", runNumber);

  timespec startTime;
  timespec endTime;
  pmu.reset();
  timespec_get(&startTime, TIME_UTC);
  pmu.start();

  // Benchmark running: do find
  for (scanner.next(word); !scanner.eof(); scanner.next(word)) {
    // auto iter = map.find_ks(word.const_data(), word.size());
    // if (iter==map.end()) {
    //   ++errors;
    // }
  }

  timespec_get(&endTime, TIME_UTC);
  stats.record(label, scanner.index(), startTime, endTime, pmu);

  if (errors) {
    printf("searchErrors: %u\n", errors);
  }

  return 0;
}

int Benchmark::Coco::start() {
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
      return rc;
    } else {
      for (unsigned i=0; i<d_config.d_runs; ++i) {
        if (d_config.d_verbosity>0) {
          printf("execute run set %u...\n", i);
        }
        // coco_testtext_insert(i, map, d_insertStats, d_file);
        // coco_testtext_find(i, map, d_findStats, d_file);
        rusage(std::cout);
      }
    }
  }
  return rc;
}
