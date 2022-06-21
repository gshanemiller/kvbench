#include <benchmark_misc.h>
#include <benchmark_hashable_keys.h>
#include <benchmark_textscan.h>
#include <trie/benchmark_htrie_bitstring.h>

#include <intel_skylake_pmu.h>

void misc_test_bitstring_nextword(unsigned int runNumber, Benchmark::Stats& stats) {
  Benchmark::HTrie::BitString<8> bs(4728986788662773602UL);

  Intel::SkyLake::PMU pmu(false, Intel::SkyLake::PMU::ProgCounterSetConfig::k_DEFAULT_SKYLAKE_CONFIG_0);

  timespec startTime;
  timespec endTime;
  pmu.reset();
  timespec_get(&startTime, TIME_UTC);
  pmu.start();

  for (unsigned int i=0; i<=63; ++i) {
    for (unsigned int j=i; j<=63; ++j) {
      auto word = bs.nextWord(i,j);
      Intel::DoNotOptimize(word);
    }
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
    snprintf(label, sizeof(label), "bitstring nextword run %u", runNumber);
    stats.addResultSet(label, 63*63, startTime, endTime, f0, f1, f2, p0, p1, p2, p3);
  }
}

void misc_test_bitstring_opt_nextword(unsigned int runNumber, Benchmark::Stats& stats) {
  Benchmark::HTrie::BitString<8> bs(4728986788662773602UL);

  Intel::SkyLake::PMU pmu(false, Intel::SkyLake::PMU::ProgCounterSetConfig::k_DEFAULT_SKYLAKE_CONFIG_0);

  timespec startTime;
  timespec endTime;
  pmu.reset();
  timespec_get(&startTime, TIME_UTC);
  pmu.start();

  for (unsigned int i=0; i<=63; ++i) {
    for (unsigned int j=i; j<=63; ++j) {
      auto word = bs.nextWord(i,j);
      Intel::DoNotOptimize(word);
    }
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
    snprintf(label, sizeof(label), "bitstring nextword run %u", runNumber);
    stats.addResultSet(label, 63*63, startTime, endTime, f0, f1, f2, p0, p1, p2, p3);
  }
}

void misc_test_scanner(unsigned int runNumber, Benchmark::Stats& stats, const Benchmark::LoadFile& file) {
  Benchmark::Slice<char> word;

  Benchmark::TextScan scanner(file);

  Intel::SkyLake::PMU pmu(false, Intel::SkyLake::PMU::ProgCounterSetConfig::k_DEFAULT_SKYLAKE_CONFIG_0);

  timespec startTime;
  timespec endTime;
  pmu.reset();
  timespec_get(&startTime, TIME_UTC);
  pmu.start();

  // Benchmark running: do insert
  int rc;
  for (scanner.next(word); !scanner.eof(); scanner.next(word)) {
    Intel::DoNotOptimize(rc);
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
    snprintf(label, sizeof(label), "benchmark-overhead run %u", runNumber);
    stats.addResultSet(label, scanner.count(), startTime, endTime, f0, f1, f2, p0, p1, p2, p3);
  }
}

int Benchmark::Misc::start() {
  int rc(0);

  for (unsigned i=0; i<d_stats.config().d_runs; ++i) {
    if (d_stats.config().d_verbosity>0) {
      printf("execute scanner overhead run set %u...\n", i);
    }
    misc_test_scanner(i, d_stats, d_file);
  }

  for (unsigned i=0; i<d_stats.config().d_runs; ++i) {
    misc_test_bitstring_nextword(i, d_stats);
  }

  for (unsigned i=0; i<d_stats.config().d_runs; ++i) {
    misc_test_bitstring_opt_nextword(i, d_stats);
  }

  return rc;
}

void Benchmark::Misc::report() {
  Intel::SkyLake::PMU pmu(false, Intel::SkyLake::PMU::ProgCounterSetConfig::k_DEFAULT_SKYLAKE_CONFIG_0);
  d_stats.print(pmu);
}
