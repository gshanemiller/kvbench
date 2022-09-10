#include <benchmark_textscan.h>
#include <gtest/gtest.h>

#include <string>

TEST(textscan, scanchar) {
  const char *fn = "test.bin.char";
  Benchmark::LoadFile file;
  if (file.load(fn)!=0) {
    printf("load failed\n");
    return;
  }
  Benchmark::TextScan<char> scanner(file);
  Benchmark::Slice<char> word;
  while (!scanner.eof()) {
    scanner.next(word);
  }
}

TEST(textscan, scanwide) {
  const char *fn = "test.bin.wide";
  Benchmark::LoadFile file;
  if (file.load(fn)!=0) {
    printf("load failed\n");
    return;
  }
  Benchmark::TextScan<int> scanner(file);
  Benchmark::Slice<int> word;
  while (!scanner.eof()) {
    scanner.next(word);
  }
}
