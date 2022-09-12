#include <benchmark_art.h>
#include <benchmark_hashable_keys.h>
#include <benchmark_textscan.h>

#include <art_mem.h>                                                                                                    

#include <intel_skylake_pmu.h>

#pragma GCC diagnostic push                                                                                             
#pragma GCC diagnostic ignored "-Wpedantic"                                                                             
#include <art.h>
#pragma GCC diagnostic pop 

template<typename T>
static int art_test_text_insert(unsigned runNumber, T& map, Intel::Stats& stats, const Benchmark::LoadFile& file) {
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
    art_insert(&map, (unsigned char*)word.data(), word.size()-1, (void*)word.data()); 
  }

  timespec_get(&endTime, TIME_UTC);
  stats.record(label, scanner.index(), startTime, endTime, pmu);

  return 0;
}

template<typename T>
static int art_test_text_find(unsigned runNumber, T& map, Intel::Stats& stats, const Benchmark::LoadFile& file) {
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
    auto val = art_search(&map, (unsigned char*)word.data(), word.size()-1);
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

extern "C" {
int art_test_text_iter(void *data, const unsigned char *key, unsigned int key_len, void *value) {
  printf("key: %p, key_len: %u, key: '%s', value: %p\n", key, key_len, (const char*)(key), value);
  return 0;
}
}

int Benchmark::ART::start() {
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
        art_mem_stats_reset();
        art_tree artTrie;
        art_tree_init(&artTrie);
        art_test_text_insert(i, artTrie, d_insertStats, d_file);
        art_test_text_find(i, artTrie, d_findStats, d_file);
        rusage(std::cout);
        art_tree_destroy(&artTrie);
        art_print_memory();
        art_mem_stats_reset();
      }
    }
  }
  return rc;
}
