#include <benchmark_cedar.h>
#include <benchmark_hashable_keys.h>
#include <benchmark_textscan.h>

#include <cedarpp.h>                                                                                                    

#include <intel_skylake_pmu.h>

#include <sys/time.h>
#include <sys/resource.h>

static int cedar_test_text_insert(unsigned runNumber, cedar::da<int>& map, Intel::Stats& stats, Benchmark::LoadFile& file) {
  // file.load("dist.bin.char");
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
    map.update(word.data(), word.size(), scanner.index());
  }

  timespec_get(&endTime, TIME_UTC);
  stats.record(label, scanner.index(), startTime, endTime, pmu);

  return 0;
}

static int cedar_test_text_find(unsigned runNumber, cedar::da<int>& map, Intel::Stats& stats, Benchmark::LoadFile& file) {
  // file.load("skew.bin.char");
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
    auto val = map.exactMatchSearch<int>(word.data(), word.size());
    if (val!=scanner.index()) {
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

int Benchmark::Cedar::start() {
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
        cedar::da<int> map;
        Benchmark::LoadFile& file = const_cast<Benchmark::LoadFile&>(d_file);
        cedar_test_text_insert(i, map, d_insertStats, file);
        cedar_test_text_find(i, map, d_findStats, file);
        rusage(std::cout);
      }
    }
  }
  return rc;
}
