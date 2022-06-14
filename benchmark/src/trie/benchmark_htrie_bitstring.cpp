#include <trie/benchmark_htrie_bitstring.h>

template<Benchmark::HTrie::htrie_size N>                                                                                                  
inline 
Benchmark::HTrie::htrie_index Benchmark::HTrie::BitString<N>::longestCommonPrefix(const BitString<N> &rhs, bool *eos) {              
  assert(eos);
  *eos = false;
  return 0;
}  
