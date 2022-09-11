#include <benchmark_datrie.h>
#include <benchmark_textscan.h>

#include <datrie/trie.h>

#include <intel_skylake_pmu.h>

template<typename T>
static int datrie_test_text_insert(unsigned runNumber, T* map, Intel::Stats& stats, const Benchmark::LoadFile& file) {
  char label[128];
  snprintf(label, sizeof(label), "insert run %u", runNumber);

  Intel::SkyLake::PMU pmu(false, Intel::SkyLake::PMU::ProgCounterSetConfig::k_DEFAULT_SKYLAKE_CONFIG_0);

  timespec startTime, endTime;
  Benchmark::TextScan<int> scanner(file);

  pmu.reset();
  timespec_get(&startTime, TIME_UTC);
  pmu.start();

  // Benchmark running: do insert
  TrieData value;
  Benchmark::Slice<int> word;
  for (scanner.next(word); !scanner.eof(); scanner.next(word)) {
    trie_store(map, (const AlphaChar*)word.data(), scanner.index());
    if (trie_retrieve(map, (const AlphaChar*)word.data(), &value)==0 || value!=scanner.index()) {
      printf("problem: ");
      word.print();
    }
  }

  timespec_get(&endTime, TIME_UTC);
  stats.record(label, scanner.index(), startTime, endTime, pmu);

  return 0;
}

template<typename T>
static int datrie_test_text_find(unsigned runNumber, T* map, Intel::Stats& stats, const Benchmark::LoadFile& file) {
  char label[128];
  snprintf(label, sizeof(label), "find run %u", runNumber);
  
  Intel::SkyLake::PMU pmu(false, Intel::SkyLake::PMU::ProgCounterSetConfig::k_DEFAULT_SKYLAKE_CONFIG_0);

  timespec startTime, endTime;
  Benchmark::TextScan<int> scanner(file);

  pmu.reset();
  timespec_get(&startTime, TIME_UTC);
  pmu.start();

  // Benchmark running: do find
  TrieData value;
  Benchmark::Slice<int> word;
  for (scanner.next(word); !scanner.eof(); scanner.next(word)) {
    trie_retrieve(map, (const AlphaChar*)word.data(), &value);
  }

  timespec_get(&endTime, TIME_UTC);
  stats.record(label, scanner.index(), startTime, endTime, pmu);

  return 0;
}

int Benchmark::datrie::start() {
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
        AlphaMap *alpha_map;
        Trie     *map;                                                                                             

        alpha_map = alpha_map_new();                                                                                       
        alpha_map_add_range(alpha_map, 0x00, 0xff);
        map = trie_new (alpha_map);                                                                                   

        datrie_test_text_insert(i, map, d_insertStats, d_file);
        datrie_test_text_find(i, map, d_findStats, d_file);

        alpha_map_free(alpha_map);                                                                                         
        trie_free(map);                                                                                              
      }
    }
  }
  return rc;
}

void Benchmark::datrie::report() {
  Intel::SkyLake::PMU pmu(false, Intel::SkyLake::PMU::ProgCounterSetConfig::k_DEFAULT_SKYLAKE_CONFIG_0);
  d_config.print();
  d_insertStats.summary("datrie Insert", pmu);
  d_findStats.summary("datrie Find", pmu);
}
