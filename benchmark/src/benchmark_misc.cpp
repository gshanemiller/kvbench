#include <benchmark_misc.h>
#include <benchmark_hashable_keys.h>
#include <benchmark_textscan.h>

#include <intel_skylake_pmu.h>

void misc_test_addresses(const Benchmark::LoadFile& file) {
  Benchmark::Slice<char> word;

  bool firstWord(true);
  intptr_t baseUpper = 0;
  const char *base = file.data();

  Benchmark::TextScan scanner(file);

  printf("sizeof(unsigned long)=%lu\n", sizeof(unsigned long));
  printf("sizeof(intptr_t)=%lu\n", sizeof(intptr_t));

  for (scanner.next(word); !scanner.eof(); scanner.next(word)) {
    if (firstWord) {
      // upper 16 bits
      baseUpper = reinterpret_cast<intptr_t>(word.data()) & ~(0xFFFFFFFFFFFFUL);
      firstWord = false;
      printf("baseUpper %lx mask %lx\n", baseUpper, ~(0xFFFFFFFFFFFFUL));
      continue;
    }

    intptr_t upper = reinterpret_cast<intptr_t>(word.data()) & ~(0xFFFFFFFFFFFFUL);
    if (upper!=baseUpper) {
      printf("ERROR: baseUpper %lx upper %lx mask %lx\n", baseUpper, upper,  ~(0xFFFFFFFFFFFFUL));
    }

    unsigned long diff = static_cast<unsigned long>(word.data() - base);
    if (diff >= 0xFFFFFFFFUL) {
      printf("ERROR: overflow base %p word %p diff %lx\n", (void*)base, (void*)word.data(), diff);
    }
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

  misc_test_addresses(d_file);

  return rc;
}

void Benchmark::Misc::report() {
  Intel::SkyLake::PMU pmu(false, Intel::SkyLake::PMU::ProgCounterSetConfig::k_DEFAULT_SKYLAKE_CONFIG_0);
  d_stats.print(pmu);
}
