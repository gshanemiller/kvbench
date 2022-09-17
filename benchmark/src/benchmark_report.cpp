#include <benchmark_report.h>

#include <intel_skylake_pmu.h>

#include <sys/time.h>
#include <sys/resource.h>

int Benchmark::Report::start() {
  return loadFile(d_config.d_filename.c_str());
}

std::ostream& Benchmark::Report::rusage(std::ostream& stream, const char *label) {
  struct rusage rusage;
  getrusage(RUSAGE_SELF, &rusage);
  if (label) {
    stream  << label
            << std::endl
            << "-------------------------------------------------------------"
            << std::endl;
  }
  stream    << "maxRssKb: "             << rusage.ru_maxrss
            << " maxRssGb: "            << (double)rusage.ru_maxrss/1024.0/1024.0
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

int Benchmark::Report::loadFile(const char *path) {
  assert(path);                                                                              
  printf("loading '%s'\n", path);
  int rc = d_file.load(path);
  if (rc!=0) {                                                                                                          
    printf("error: cannot load '%s': %s (errno=%d)\n", path, strerror(rc), rc);                           
    exit(1);                                                                                                            
  }                                                                                                                     
  char buffer[1024];
  snprintf(buffer, sizeof(buffer), "After Loading '%s'", path);
  rusage(std::cout, buffer);
  return 0;
}
