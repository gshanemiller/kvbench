#include <benchmark_textscan.h>
#include <gtest/gtest.h>

#include <iostream>

#include <sys/time.h>
#include <sys/resource.h>

std::ostream& rusage(std::ostream& stream, const char *label) {
  struct rusage rusage;
  getrusage(RUSAGE_SELF, &rusage);
  stream    << label
            << std::endl
            << "-------------------------------------------"
            << std::endl
            << "maxrssKb: "             << rusage.ru_maxrss
            << " maxrssGb: "            << (double)rusage.ru_maxrss/1024.0/1024.0
            << " swaps: "               << rusage.ru_nswap
            << " minorPageFaults: "     << rusage.ru_minflt
            << " majorPageFaults: "     << rusage.ru_majflt
            << " volContextSwitches: "  << rusage.ru_nvcsw
            << " frcdContextSwitches: " << rusage.ru_nivcsw
            << " signalsReceived: "     << rusage.ru_nsignals
            << std::endl
            << std::endl; 
  return stream;
}

TEST(textscan, scanchar) {
  const char *fn = "url.bin";
  rusage(std::cout, "Before load");
  Benchmark::LoadFile file;
  if (file.load(fn)!=0) {
    printf("load failed\n");
    return;
  }
  rusage(std::cout, "After load");
  Benchmark::TextScan<char> scanner(file);
  Benchmark::Slice<char> word;
  while (!scanner.eof()) {
    scanner.next(word);
  }
  rusage(std::cout, "After scan");
}
