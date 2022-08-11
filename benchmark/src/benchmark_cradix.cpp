#include <benchmark_cradix.h>
#include <benchmark_textscan.h>

#include <cradix_tree.h>
#include <cradix_memmanager.h>

#include <intel_skylake_pmu.h>

template<typename T>
static int cradix_test_text_insert(unsigned runNumber, T* map, Benchmark::Stats& stats, const Benchmark::LoadFile& file) {
  Benchmark::Slice<unsigned char> word;

  Benchmark::TextScan scanner(file);

  Intel::SkyLake::PMU pmu(false, Intel::SkyLake::PMU::ProgCounterSetConfig::k_DEFAULT_SKYLAKE_CONFIG_0);

  timespec startTime;
  timespec endTime;
  pmu.reset();
  timespec_get(&startTime, TIME_UTC);
  pmu.start();
  
  // Benchmark running: do insert
  u_int32_t count=0;
  for (scanner.next(word); !scanner.eof(); scanner.next(word)) {
    map->insert(word);
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
static int cradix_test_text_find(unsigned runNumber, T* map, Benchmark::Stats& stats, const Benchmark::LoadFile& file) {
  Benchmark::Slice<unsigned char> word;

  Benchmark::TextScan scanner(file);

  Intel::SkyLake::PMU pmu(false, Intel::SkyLake::PMU::ProgCounterSetConfig::k_DEFAULT_SKYLAKE_CONFIG_0);

  unsigned int errors(0);

  timespec startTime;
  timespec endTime;
  pmu.reset();
  timespec_get(&startTime, TIME_UTC);
  pmu.start();

  // Benchmark running: do find
  for (scanner.next(word); !scanner.eof(); scanner.next(word)) {
    auto val = map->find(word);
    if (val!=CRadix::e_EXISTS) {
      ++errors;
    }
  }

  u_int64_t f0 = pmu.fixedCounterValue(0);
  u_int64_t f1 = pmu.fixedCounterValue(1);
  u_int64_t f2 = pmu.fixedCounterValue(2);

  u_int64_t p0 = pmu.programmableCounterValue(0);
  u_int64_t p1 = pmu.programmableCounterValue(1);
  u_int64_t p2 = pmu.programmableCounterValue(2);
  u_int64_t p3 = pmu.programmableCounterValue(3);

  timespec_get(&endTime, TIME_UTC);

  if (errors) {
    printf("searchErrors: %u\n", errors);
  }

  // Benchmark done: take stats
  if (stats.config().d_runs-runNumber<=stats.config().d_recordRuns) {
    char label[128];
    snprintf(label, sizeof(label), "find run %u", runNumber);
    stats.addResultSet(label, scanner.count(), startTime, endTime, f0, f1, f2, p0, p1, p2, p3);
  }
  return 0;
}

int Benchmark::cradix::start() {
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
        CRadix::MemManager mem(0xFFFFFFFFU, 4);;
        CRadix::Tree cradixTree(&mem);
        cradix_test_text_insert(i, &cradixTree, d_stats, d_file);
        cradix_test_text_find(i, &cradixTree, d_stats, d_file);

        CRadix::MemStats mstats;
        mem.statistics(&mstats);
        mstats.print(std::cout);

        CRadix::TreeStats tstats;
        cradixTree.statistics(&tstats);
        tstats.print(std::cout);

/*
        CRadix::NodeStats nstats;
        CRadix::Node256::runtimeStatistics(&nstats);
        nstats.print(std::cout);
*/
      }
    }
  }
  return rc;
}

void Benchmark::cradix::report() {
  Intel::SkyLake::PMU pmu(false, Intel::SkyLake::PMU::ProgCounterSetConfig::k_DEFAULT_SKYLAKE_CONFIG_0);
  d_stats.print(pmu);
}
