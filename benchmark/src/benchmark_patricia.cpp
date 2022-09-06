#include <benchmark_patricia.h>
#include <benchmark_textscan.h>

#include <patricia_tree.h>

#include <intel_skylake_pmu.h>

#pragma GCC diagnostic push                                                                                             
#pragma GCC diagnostic ignored "-Wpedantic"                                                                             
#include <art.h>
#pragma GCC diagnostic pop 

extern Patricia::MemoryManager memManager;

template<typename T>
static int patricia_test_text_insert(unsigned runNumber, T* map, Intel::Stats& stats, const Benchmark::LoadFile& file) {
  Benchmark::Slice<unsigned char> word;
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
    Patricia::insertKey(map, word);
  }

  timespec_get(&endTime, TIME_UTC);
  stats.record(label, scanner.count(), startTime, endTime, pmu);

  return 0;
}

template<typename T>
static int patricia_test_text_find(unsigned runNumber, T* map, Intel::Stats& stats, const Benchmark::LoadFile& file) {
  Benchmark::Slice<unsigned char> word;
  Benchmark::TextScan scanner(file);
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
    auto val = Patricia::findKey(map, word);
    if (val!=0) {
      ++errors;
    }
  }

  timespec_get(&endTime, TIME_UTC);
  stats.record(label, scanner.count(), startTime, endTime, pmu);

  if (errors) {
    printf("searchErrors: %u\n", errors);
  }

  return 0;
}

int Benchmark::patricia::start() {
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
        Patricia::Tree *patriciaTree = memManager.allocTree();
        patricia_test_text_insert(i, patriciaTree, d_insertStats, d_file);
        patricia_test_text_find(i, patriciaTree, d_findStats, d_file);
        memManager.print();
      }
    }
  }
  return rc;
}

void Benchmark::patricia::report() {
  Intel::SkyLake::PMU pmu(false, Intel::SkyLake::PMU::ProgCounterSetConfig::k_DEFAULT_SKYLAKE_CONFIG_0);
  d_config.print();
  d_insertStats.summary("Patricia Insert", pmu);
  d_findStats.summary("Patricia Find", pmu);
}
