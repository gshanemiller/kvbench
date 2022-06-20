#include <trie/benchmark_htrie_bitstring.h>

unsigned int Benchmark::HTrie::BitStringStats::d_byteSuffixCalls = 0;                                                              
unsigned int Benchmark::HTrie::BitStringStats::d_bytePrefixCalls = 0;                                                              
unsigned int Benchmark::HTrie::BitStringStats::d_substringCalls = 0; 
unsigned int Benchmark::HTrie::BitStringStats::d_3byteEndCalls = 0;
unsigned int Benchmark::HTrie::BitStringStats::d_2byteEndCalls = 0;
unsigned int Benchmark::HTrie::BitStringStats::d_1byteEndCalls = 0;

Benchmark::HTrie::htrie_word byteMove8(const Benchmark::HTrie::htrie_byte* startByte) {
  return *(reinterpret_cast<const Benchmark::HTrie::htrie_word*>(startByte));
}

Benchmark::HTrie::htrie_word byteMove7(const Benchmark::HTrie::htrie_byte* startByte) {
  Benchmark::HTrie::ByteMoveHelper retHelper = {0};
  retHelper.uint[0] = *reinterpret_cast<const Benchmark::HTrie::htrie_uint*>(startByte);
  retHelper.byte[4] = startByte[4];
  retHelper.byte[5] = startByte[5];
  return retHelper.word;
}

Benchmark::HTrie::htrie_word byteMove6(const Benchmark::HTrie::htrie_byte* startByte) {
  Benchmark::HTrie::ByteMoveHelper retHelper = {0};
  retHelper.uint[0] = *reinterpret_cast<const Benchmark::HTrie::htrie_uint*>(startByte);
  retHelper.byte[4] = startByte[4];
  retHelper.byte[5] = startByte[5];
  return retHelper.word;
}

Benchmark::HTrie::htrie_word byteMove5(const Benchmark::HTrie::htrie_byte* startByte) {
  Benchmark::HTrie::ByteMoveHelper retHelper = {0};
  retHelper.uint[0] = *reinterpret_cast<const Benchmark::HTrie::htrie_uint*>(startByte);
  retHelper.byte[4] = startByte[4];
  return retHelper.word;
}

Benchmark::HTrie::htrie_word byteMove4(const Benchmark::HTrie::htrie_byte* startByte) {
  return *(reinterpret_cast<const Benchmark::HTrie::htrie_uint*>(startByte));
}

Benchmark::HTrie::htrie_word byteMove3(const Benchmark::HTrie::htrie_byte* startByte) {
  Benchmark::HTrie::ByteMoveHelper retHelper = {0};
  retHelper.sword[0] = *reinterpret_cast<const Benchmark::HTrie::htrie_sword*>(startByte);
  retHelper.byte[2] = startByte[2];
  return retHelper.word;
}

Benchmark::HTrie::htrie_word byteMove2(const Benchmark::HTrie::htrie_byte* startByte) {
  return *(reinterpret_cast<const Benchmark::HTrie::htrie_sword*>(startByte));
}

Benchmark::HTrie::htrie_word byteMove1(const Benchmark::HTrie::htrie_byte* startByte) {
  return *startByte;
}

Benchmark::HTrie::htrie_word (*Benchmark::HTrie::benchmark_htrie_byteMove[9])(const Benchmark::HTrie::htrie_byte*) = {
  0,
  byteMove1,
  byteMove2,
  byteMove3,
  byteMove4,
  byteMove5,
  byteMove6,
  byteMove7,
  byteMove8
};

Benchmark::HTrie::htrie_word byteMove7AndPrefix(const Benchmark::HTrie::htrie_byte* startByte, Benchmark::HTrie::htrie_byte endBit) {
  Benchmark::HTrie::ByteMoveHelper retHelper = {0};                                                                     
  retHelper.uint[0] = *(reinterpret_cast<const Benchmark::HTrie::htrie_uint*>(startByte));
  retHelper.sword[2] = *(reinterpret_cast<const Benchmark::HTrie::htrie_sword*>(startByte+4));
  retHelper.byte[6] = startByte[6];
  retHelper.byte[7] = startByte[7]&(~(0xff<<endBit));
  return retHelper.word;
}

Benchmark::HTrie::htrie_word byteMove6AndPrefix(const Benchmark::HTrie::htrie_byte* startByte, Benchmark::HTrie::htrie_byte endBit) {
  Benchmark::HTrie::ByteMoveHelper retHelper = {0};                                                                     
  retHelper.uint[0] = *(reinterpret_cast<const Benchmark::HTrie::htrie_uint*>(startByte));
  retHelper.sword[2] = *(reinterpret_cast<const Benchmark::HTrie::htrie_sword*>(startByte+4));
  retHelper.byte[6] = startByte[6]&(~(0xff<<endBit));
  return retHelper.word;
}

Benchmark::HTrie::htrie_word byteMove5AndPrefix(const Benchmark::HTrie::htrie_byte* startByte, Benchmark::HTrie::htrie_byte endBit) {
  Benchmark::HTrie::ByteMoveHelper retHelper = {0};                                                                     
  retHelper.uint[0] = *(reinterpret_cast<const Benchmark::HTrie::htrie_uint*>(startByte));
  retHelper.byte[4] = startByte[4];
  retHelper.byte[5] = startByte[5]&(~(0xff<<endBit)); 
  return retHelper.word;
}

Benchmark::HTrie::htrie_word byteMove4AndPrefix(const Benchmark::HTrie::htrie_byte* startByte, Benchmark::HTrie::htrie_byte endBit) {
  Benchmark::HTrie::ByteMoveHelper retHelper = {0};                                                                     
  retHelper.uint[0] = *(reinterpret_cast<const Benchmark::HTrie::htrie_uint*>(startByte));
  retHelper.byte[4] = startByte[4]&(~(0xff<<endBit)); 
  return retHelper.word;
}

Benchmark::HTrie::htrie_word byteMove3AndPrefix(const Benchmark::HTrie::htrie_byte* startByte, Benchmark::HTrie::htrie_byte endBit) {
  Benchmark::HTrie::ByteMoveHelper retHelper = {0};                                                                     
  retHelper.sword[0] = *(reinterpret_cast<const Benchmark::HTrie::htrie_sword*>(startByte));
  retHelper.byte[2] = startByte[2];
  retHelper.byte[3] = startByte[3]&(~(0xff<<endBit));
  return retHelper.word;
}

Benchmark::HTrie::htrie_word byteMove2AndPrefix(const Benchmark::HTrie::htrie_byte* startByte, Benchmark::HTrie::htrie_byte endBit) {
  Benchmark::HTrie::ByteMoveHelper retHelper = {0};                                                                     
  retHelper.sword[0] = *(reinterpret_cast<const Benchmark::HTrie::htrie_sword*>(startByte));
  retHelper.byte[2] = startByte[2]&(~(0xff<<endBit));
  return retHelper.word;
}

Benchmark::HTrie::htrie_word byteMove1AndPrefix(const Benchmark::HTrie::htrie_byte* startByte, Benchmark::HTrie::htrie_byte endBit) {
  Benchmark::HTrie::ByteMoveHelper retHelper = {0};                                                                     
  retHelper.byte[0] = *startByte;
  retHelper.byte[1] = startByte[1]&(~(0xff<<endBit));
  return retHelper.word;
}

Benchmark::HTrie::htrie_word (*Benchmark::HTrie::benchmark_htrie_byteMovePrefix[8])(const Benchmark::HTrie::htrie_byte*, Benchmark::HTrie::htrie_byte) = {
  0,
  byteMove1AndPrefix,
  byteMove2AndPrefix,
  byteMove3AndPrefix,
  byteMove4AndPrefix,
  byteMove5AndPrefix,
  byteMove6AndPrefix,
  byteMove7AndPrefix,
};

template<Benchmark::HTrie::htrie_size N>
inline 
Benchmark::HTrie::htrie_index Benchmark::HTrie::BitString<N>::longestCommonPrefix(const BitString<N> &rhs, bool *eos) {              
  assert(eos);
  *eos = false;
  return 0;
}  
