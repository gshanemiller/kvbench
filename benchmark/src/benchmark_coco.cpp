#include <benchmark_coco.h>
#include <benchmark_textscan.h>

#include <uncompacted_trie.hpp>
#include <utils.hpp>
#include <CoCo-trie_fast.hpp>

#include <intel_skylake_pmu.h>

static CoCo_fast<> *coco = 0;
static std::vector<std::string*> coco_data;

static int coco_testtext_insert(unsigned runNumber, Intel::Stats& stats, const Benchmark::LoadFile& file) {
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
  
  coco = new CoCo_fast<>(coco_data);

  timespec_get(&endTime, TIME_UTC);
  stats.record(label, coco_data.size(), startTime, endTime, pmu);

  return 0;
}

static int coco_testtext_find(unsigned runNumber, Intel::Stats& stats, const Benchmark::LoadFile& file) {
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
  for (unsigned i=0; i<coco_data.size(); ++i) {
    auto rank = coco->look_up(*coco_data[i]);
    if (rank>=0xffffffffUL) {
      ++errors;
    }
  }

  timespec_get(&endTime, TIME_UTC);
  stats.record(label, coco_data.size(), startTime, endTime, pmu);

  if (errors) {
    printf("searchErrors: %u\n", errors);
  }

  return 0;
}

int Benchmark::Coco::start() {
  int rc = 0;
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

        // Load data
        rc = Benchmark::Report::start();                                                                                  
        if (rc!=0) {                                                                                                          
          return rc;                                                                                                          
        }

        // For CoCo we need strings as vector
        Benchmark::TextScan<char> scanner(d_file);
        scanner.exportAsVector(coco_data);
        // No longer needed
        d_file.free();

        // Benchmark it
        coco_testtext_insert(i, d_insertStats, d_file);
        coco_testtext_find(i, d_findStats, d_file);
        rusage(std::cout, "After All CoCo Benchmarks");

        // Cleanup
        coco_data.clear();
        delete coco;
        coco = 0;
        rusage(std::cout, "After CoCo Cleanup");
      }
    }
  }
  return rc;
}
