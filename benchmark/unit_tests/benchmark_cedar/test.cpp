#include <benchmark_textscan.h>
#include <cedar.h>
#include <gtest/gtest.h>

#include <string>

cedar::da <int> da;

TEST(cedar, insert) {
  const char *fn = "test.bin.char";
  Benchmark::LoadFile file;
  if (file.load(fn)!=0) {
    printf("load failed\n");
    return;
  }
  Benchmark::TextScan<char> scanner(file);
  Benchmark::Slice<char> word;
  for (unsigned i=0; !scanner.eof() && i<20000000; ++i) {
    scanner.next(word);
    da.update(word.data(), word.size(), scanner.index());
  }
}

TEST(cedar, exactmatch) {
  const char *fn = "test.bin.char";
  Benchmark::LoadFile file;
  if (file.load(fn)!=0) {
    printf("load failed\n");
    return;
  }
  Benchmark::TextScan<char> scanner(file);
  Benchmark::Slice<char> word;
  volatile int val = 0;
  for (unsigned i=0; !scanner.eof() && i<20000000; ++i) {
    scanner.next(word);
    val = da.exactMatchSearch<int>(word.data(), word.size());
  }
}
