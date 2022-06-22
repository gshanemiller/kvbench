#include <trie/benchmark_htrie_node.h>
#include <gtest/gtest.h>
#include <random>

static std::mt19937_64 rng;
static bool seedrng=false;
static std::uniform_int_distribution<uint64_t> distribution(0, 0xffffffffffffffff);

static void seedRng() {
  if (seedrng) {
    auto seed = std::random_device{}();
    rng.seed(seed);
  }
}

static Benchmark::HTrie::htrie_byte encodeSize(Benchmark::HTrie::htrie_byte byteSize, Benchmark::HTrie::htrie_byte bitLen) {
  assert(byteSize<=8);                                                                                                  
  assert(bitLen<=64);                                                                                                   
  return (byteSize<<8 | bitLen);                                                                                        
} 

void randomBits(Benchmark::HTrie::htrie_byte *len, Benchmark::HTrie::htrie_uint64 *partialKey) {
/*
  auto dice = bind(distribution, rng);

  auto rawValue = dice();
  auto bitLen = (dice() & 63UL) + 1UL;
  assert(bitLen>0&&bitLen<=64);

  Benchmark::HTrie::htrie_uint64 value(0);
  for (Benchmark::HTrie::htrie_uint64 b=0; b<=bitLen; ++b) {
    value |= (rawValue & (1ULL<<b));
  }

  *partialKey = value;

  if (bitLen<=8) {
    *len = encodeSize(1, bitLen);
  } else if (bitLen<=16) {
    *len = encodeSize(2, bitLen);
  } else if (bitLen<=24) {
    *len = encodeSize(3, bitLen);
  } else if (bitLen<=32) {
    *len = encodeSize(4, bitLen);
  } else if (bitLen<=40) {
    *len = encodeSize(5, bitLen);
  } else if (bitLen<=48) {
    *len = encodeSize(6, bitLen);
  } else if (bitLen<=56) {
    *len = encodeSize(7, bitLen);
  } else {
    *len = encodeSize(8, bitLen);
  }
*/
  return;
}

TEST(node, constructor) {
  if (seedrng) {
    seedRng();
  }

  Benchmark::HTrie::TestNode64 node;

/*
  for (unsigned i=0; i<=63; ++i) {
    randomBits(node.d_bitLen+i, node.d_partialKey+i);
  }
*/
}
