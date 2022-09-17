#include <benchmark_wormhole.h>
#include <benchmark_hashable_keys.h>
#include <benchmark_textscan.h>

#include "lib.h"
#include "kv.h"
#include <wh.h>

#include <intel_skylake_pmu.h>

template<typename T>
static int wormhole_test_text_insert(unsigned runNumber, T* map, Intel::Stats& stats, const Benchmark::LoadFile& file) {
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
    wh_put(map, word.data(), word.size(), 0, 0); 
  }

  timespec_get(&endTime, TIME_UTC);
  stats.record(label, scanner.index(), startTime, endTime, pmu);

  return 0;
}

template<typename T>
static int wormhole_test_text_find(unsigned runNumber, T* map, Intel::Stats& stats, const Benchmark::LoadFile& file) {
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
    auto val = wh_probe(map, word.data(), word.size());
    if (val==0) {
      ++errors;
    }
  }

  timespec_get(&endTime, TIME_UTC);
  stats.record(label, scanner.index(), startTime, endTime, pmu);

  if (errors) {
    printf("searchErrors: %u\n", errors);
  }

  return 0;
}

int Benchmark::WormHole::start() {
  // Default start is to load file                                                                                      
  int rc = Benchmark::Report::start();                                                                                  
  if (rc!=0) {                                                                                                          
    return rc;                                                                                                          
  }

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
        struct wormhole * const wh = wh_create();
        struct wormref * const ref = wh_ref(wh);
        wormhole_test_text_insert(i, ref, d_insertStats, d_file);
        wormhole_test_text_find(i, ref, d_findStats, d_file);
        rusage(std::cout);
        wh_clean(wh);
        wh_destroy(wh);
      }
    }
  }
  return rc;
}
