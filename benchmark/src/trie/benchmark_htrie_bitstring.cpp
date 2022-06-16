#include <trie/benchmark_htrie_bitstring.h>

unsigned int Benchmark::HTrie::BitStringStats::d_byteSuffixCalls = 0;                                                              
unsigned int Benchmark::HTrie::BitStringStats::d_bytePrefixCalls = 0;                                                              
unsigned int Benchmark::HTrie::BitStringStats::d_substringCalls = 0; 

template<Benchmark::HTrie::htrie_size N>                                                                                                  
inline 
Benchmark::HTrie::htrie_index Benchmark::HTrie::BitString<N>::longestCommonPrefix(const BitString<N> &rhs, bool *eos) {              
  assert(eos);
  *eos = false;
  return 0;
}  
