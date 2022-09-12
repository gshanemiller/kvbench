#include <benchmark_report.h>

#include <intel_skylake_pmu.h>

#include <sys/time.h>
#include <sys/resource.h>

int Benchmark::Report::start() {
  return 0;
}

std::ostream& Benchmark::Report::rusage(std::ostream& stream) {
  struct rusage rusage;
  getrusage(RUSAGE_SELF, &rusage);
  stream    << "maxrssKb: "             << rusage.ru_maxrss
            << " maxrssGb: "            << (double)rusage.ru_maxrss/1024.0/1024.0
            << " swaps: "               << rusage.ru_nswap
            << " minorPageFaults: "     << rusage.ru_minflt
            << " majorPageFaults: "     << rusage.ru_majflt
            << " volContextSwitches: "  << rusage.ru_nvcsw
            << " frcdContextSwitches: " << rusage.ru_nivcsw
            << " signalsReceived: "     << rusage.ru_nsignals
            << std::endl; 
  return stream;
}

void Benchmark::Report::report() {
  Intel::SkyLake::PMU pmu(false, Intel::SkyLake::PMU::ProgCounterSetConfig::k_DEFAULT_SKYLAKE_CONFIG_0);
  d_config.print();
  std::string desc = d_description;
  desc.append(" Insert");
  d_insertStats.summary(desc.c_str(), pmu);
  desc = d_description;
  desc.append(" ExactSearch");
  d_findStats.summary(desc.c_str(), pmu);
  rusage(std::cout);
}
