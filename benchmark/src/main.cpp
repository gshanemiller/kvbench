#include <stdio.h>
#include <unistd.h>
#include <string.h>

// #include <benchmark_config.h>
// #include <benchmark_loadfile.h>
// #include <benchmark_cuckoo.h>
// #include <benchmark_misc.h>
// #include <benchmark_f14.h>
// #include <benchmark_hot.h>

#include <trie/benchmark_htrie_root.h>
#include <trie/benchmark_htrie_bitstring.h>
#include <trie/benchmark_htrie_bititerator.h>
#include <benchmark_slice.h>

int main(int argc, char **argv) {
  Benchmark::HTrie::BitString<10> b0 = {0, 1, 1, 0, 1, 0, 0, 1, 0, 1};
  Benchmark::HTrie::BitString<10> b1 = {0, 1, 1, 0, 1, 0, 0, 1, 1, 0};
  Benchmark::HTrie::BitString<10> b2 = {0, 1, 1, 0, 1, 0, 1, 0, 1, 0};
  Benchmark::HTrie::BitString<10> b3 = {0, 1, 1, 0, 1, 0, 1, 0, 1, 1};
  Benchmark::HTrie::BitString<10> b4 = {0, 1, 1, 1, 0, 1, 0, 1, 1, 0};
  Benchmark::HTrie::BitString<10> b5 = {0, 1, 1, 1, 1, 0, 1, 0, 0, 1};
  Benchmark::HTrie::BitString<10> b6 = {0, 1, 1, 1, 1, 0, 1, 0, 1, 1};


  Benchmark::HTrie::Root root;

  Benchmark::HTrie::BitIterator iter = b0.iterator();
  root.insert(iter);

  iter = b1.iterator();
  root.insert(iter);

  iter = b2.iterator();
  root.insert(iter);

  iter = b3.iterator();
  root.insert(iter);

  iter = b4.iterator();
  root.insert(iter);

  iter = b5.iterator();
  root.insert(iter);

  iter = b6.iterator();
  root.insert(iter);

  root.print(std::cout);

  std::string str0("abc 123");
  Benchmark::Slice<Benchmark::HTrie::htrie_byte> s0(str0);
  iter = s0.iterator();
  iter.dump(std::cout);
  
  return 0;
}
