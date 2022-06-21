#include <trie/benchmark_htrie_bitstring.h>
#include <gtest/gtest.h>
#include <random>

static bool verbose=false;
static bool seedrng=false;
static std::mt19937_64 rng;
static unsigned RANDOM_TESTS = 1000000;

static void seedRng() {
  if (seedrng) {
    auto seed = std::random_device{}();
    rng.seed(seed);
  }
}

static void resetBitStringCallCounters() {
  Benchmark::HTrie::BitStringStats::d_byteSuffixCalls = 0;
  Benchmark::HTrie::BitStringStats::d_bytePrefixCalls = 0;
  Benchmark::HTrie::BitStringStats::d_byteSubstringCalls = 0;
}

static bool testBitStringCallCounters(unsigned int expectedByteSuffixCalls,
    unsigned int expectedBytePrefixCalls, unsigned int expectedByteSubstringCalls) {

  EXPECT_EQ(expectedByteSuffixCalls, Benchmark::HTrie::BitStringStats::d_byteSuffixCalls);
  EXPECT_EQ(expectedBytePrefixCalls, Benchmark::HTrie::BitStringStats::d_bytePrefixCalls);
  EXPECT_EQ(expectedSubstringCalls, Benchmark::HTrie::BitStringStats::d_byteSubstringCalls);

  return (expectedByteSuffixCalls==Benchmark::HTrie::BitStringStats::d_byteSuffixCalls &&
      expectedBytePrefixCalls==Benchmark::HTrie::BitStringStats::d_bytePrefixCalls &&
      expectedSubstringCalls==Benchmark::HTrie::BitStringStats::d_byteSubstringCalls);
}

static void randomSpan(const Benchmark::HTrie::htrie_uint64 max) {
  seedRng();
  std::uniform_int_distribution<uint64_t> distribution(0, 0xffffffffffffffff);
  auto dice = bind(distribution, rng);
  for (unsigned i=0; i<RANDOM_TESTS; ++i) {
    const Benchmark::HTrie::htrie_uint64 b = dice();
    Benchmark::HTrie::BitString<8> bs(b);
    const Benchmark::HTrie::htrie_uint64 start = dice() & 63;
    for (Benchmark::HTrie::htrie_uint64 delta = 0; (delta<max) && (start+delta)<=63; ++delta) {
      const Benchmark::HTrie::htrie_index end = start+delta;
      const Benchmark::HTrie::htrie_uint64 actual = bs.nextWord(start, end);
      Benchmark::HTrie::htrie_uint64 tmp(0);
      for (Benchmark::HTrie::htrie_uint64 t=start; t<=end; ++t) {
        tmp |= (b & (1ULL<<t));
      }
      const Benchmark::HTrie::htrie_uint64 expected = (tmp >> start);
      // Inspect result
      if (actual!=expected) {
        printf("ERROR: line %d: bs.nextWord_random_substring(%lu, %u) on %lu failed\n", __LINE__, start, end, b);
        EXPECT_EQ(actual, expected);
      } else if (verbose) {
        printf("OK   : line %d: bs.nextWord_random_substring(%lu, %u) on %lu: actual: %lu, expected: %lu\n",
          __LINE__, start, end, b, actual, expected);
      }
      const Benchmark::HTrie::htrie_uint64 optActual = bs.optNextWord(start, end);
      if (optActual!=expected) {
        printf("ERROR: line %d: bs.nextWord_random_substring_opt(%lu, %u) on %lu failed\n", __LINE__, start, end, b);
        EXPECT_EQ(actual, expected);
      } else if (verbose) {
        printf("OK   : line %d: bs.nextWord_random_substring_opt(%lu, %u) on %lu: actual: %lu, expected: %lu\n",
          __LINE__, start, end, b, actual, expected);
      }
    }
  }
  printf("randomSpan: max %lu tests %d done\n", max, RANDOM_TESTS);
}

// Determine if 'byteSuffix' is able to extract all bits from specified bit i
// to the end of the byte in which i occurs. i may not be on an byte boundary.
// All 256 value in a 1-byte bitstring are tested bits 1-7, 2,-7, ... 7-7
TEST(bitstring, byteSuffix) {
  for (unsigned b=0; b<=255; ++b) {
    for (unsigned i=1; i<8; ++i) {
      resetBitStringCallCounters();
      const Benchmark::HTrie::htrie_byte byte = (Benchmark::HTrie::htrie_byte)(b);
      Benchmark::HTrie::BitString<1> bs(byte);
      // Get all bits from i onwards in i's byte
      const Benchmark::HTrie::htrie_uint64 actual = bs.byteSuffix(i);
      const Benchmark::HTrie::htrie_uint64 expected = (b&254)>>i;
      // Inspect result
      if (actual!=expected || !testBitStringCallCounters(1, 0, 0)) {
        printf("ERROR: line %d: bs.byteSuffix(%u) on %u failed\n", __LINE__, i, b);
        EXPECT_EQ(actual, expected);
      } else if (verbose) {
        printf("OK   : line %d: bs.byteSuffix(%u) on %u: actual: %lu, expected: %lu\n", __LINE__, i, b, actual, expected);
      }
    }
  }
}

// Determine if 'bytePrefix' is able to extract bits from start of a byte and
// left shift that result onto a preset number and check for correctness.
//
// To test a 1-byte bitstring will be constructed to hold all possible 256 
// values. All prefixes 0-6, 0-5, 0-4, 0-0 will be extracted and left shifted
// onto to a preset value at all possible shift values. The final resulted is
// tested for correctness.
TEST(bitstring, bytePrefix) {
  for (unsigned b=0; b<=255; ++b) {
    for (unsigned end=0; end<=6; ++end) {
      const Benchmark::HTrie::htrie_byte byte = (Benchmark::HTrie::htrie_byte)(b);
      Benchmark::HTrie::BitString<1> bs(byte);
      // Prefix extracts at most 7 bits. That means they can be left shifted
      // at most <(63-7)=56 bits assuming sizeof(htrie_word)=8 bytes or 64 bits
      for (unsigned shift=0; shift<(63-7); shift++) {
        resetBitStringCallCounters();
        const Benchmark::HTrie::htrie_uint64 actual = bs.bytePrefix(0, end, shift);
        // Construct a mask with loop unlike bytePrefix
        Benchmark::HTrie::htrie_uint64 mask(0);
        for (unsigned t=0; t<=end; ++t) {
          mask |= (1<<t);
        }
        // bytePrefix never grabs last bit so make sure expected cannot have it
        EXPECT_EQ(unsigned(0), mask&128);
        const Benchmark::HTrie::htrie_uint64 expected = (byte & mask) << shift;
        // Inspect result
        if (actual!=expected || !testBitStringCallCounters(0, 1, 0)) {
          printf("ERROR: line %d: bs.bytePrefix(%u, %u, %u) on %u failed\n", __LINE__, 0, end, shift, b);
          EXPECT_EQ(actual, expected);
        } else if (verbose) {
          printf("OK   : line %d: bs.bytePrefix(%u, %u, %u) on %u: actual: %lu, expected: %lu\n",
              __LINE__, 0, end, shift, b, actual, expected);
        }
      }
    }
  }
}

// Determine if 'substring' is able to extract bits '[start,end]' where start,
// end are in the same byte not on bit positions 0 or 7.
// 
// To test a 1-byte bit string is contructed over all possible 256 values, then
// every substring in that byte are extracted and checked for correctness.
TEST(bitstring, substring) {
  for (unsigned b=0; b<=255; ++b) {
    for (unsigned start=1; start<7; ++start) {
      for (unsigned end=start; end<7; ++end) {
        resetBitStringCallCounters();
        const Benchmark::HTrie::htrie_byte byte = (Benchmark::HTrie::htrie_byte)(b);
        Benchmark::HTrie::BitString<1> bs(byte);
        // Get bits in [start, end]
        const Benchmark::HTrie::htrie_uint64 actual = bs.byteSubstring(start, end);
        // Unlike 'substring' construct expected result with loops
        Benchmark::HTrie::htrie_uint64 tmp(0);
        for (unsigned t=0; t<7; ++t) {
          if (t>=start && t<=end) {
            tmp |= ((1<<t)&byte); // copy bit 't' into tmp
          }
        }
        const Benchmark::HTrie::htrie_uint64 expected(tmp >> start);
        // Inspect result
        if (actual!=expected || !testBitStringCallCounters(0, 0, 1)) {
          printf("ERROR: line %d: bs.byteSubstring(%u, %u) on %u failed\n", __LINE__, start, end, b);
          EXPECT_EQ(actual, expected);
        } else if (verbose) {
          printf("OK   : line %d: bs.byteSubstring(%u, %u) on %u: actual: %lu, expected: %lu\n",
              __LINE__, start, end, b, actual, expected);
        }
      }
    }
  }
}

// Determine if 'nextWord' is able to extract a byte prefix
TEST(bitstring, nextWord_bytePrefix) {
  for (unsigned b=0; b<=255; ++b) {
    for (unsigned end=0; end<=6; ++end) {
      const Benchmark::HTrie::htrie_byte byte = (Benchmark::HTrie::htrie_byte)(b);
      Benchmark::HTrie::BitString<1> bs(byte);
      resetBitStringCallCounters();
      const Benchmark::HTrie::htrie_uint64 actual = bs.nextWord(0, end);
      // Construct a mask with loop unlike bytePrefix
      Benchmark::HTrie::htrie_uint64 mask(0);
      for (unsigned t=0; t<=end; ++t) {
        mask |= (1<<t);
      }
      // bytePrefix never grabs last bit so make sure expected cannot have it
      EXPECT_EQ(unsigned(0), mask&128);
      const Benchmark::HTrie::htrie_uint64 expected = (byte & mask);
      // Inspect result
      if (actual!=expected || !testBitStringCallCounters(0, 1, 0)) {
        printf("ERROR: line %d: bs.nextWord_bytePrefix(%u, %u) on %u failed\n", __LINE__, 0, end, b);
        EXPECT_EQ(actual, expected);
      } else if (verbose) {
        printf("OK   : line %d: bs.bytePrefix(%u, %u) on %u: actual: %lu, expected: %lu\n",
            __LINE__, 0, end, b, actual, expected);
      }
    }
  }
}

// Determine if 'nextWord' is able to extract a byte substring
TEST(bitstring, nextWord_substring) {
  for (unsigned b=0; b<=255; ++b) {
    for (unsigned start=1; start<7; ++start) {
      for (unsigned end=start; end<7; ++end) {
        resetBitStringCallCounters();
        const Benchmark::HTrie::htrie_byte byte = (Benchmark::HTrie::htrie_byte)(b);
        Benchmark::HTrie::BitString<1> bs(byte);
        // Get bits in [start, end]
        const Benchmark::HTrie::htrie_uint64 actual = bs.nextWord(start, end);
        // Unlike 'substring' construct expected result with loops
        Benchmark::HTrie::htrie_uint64 tmp(0);
        for (unsigned t=0; t<7; ++t) {
          if (t>=start && t<=end) {
            tmp |= ((1<<t)&byte); // copy bit 't' into tmp
          }
        }
        const Benchmark::HTrie::htrie_uint64 expected(tmp >> start);
        // Inspect result
        if (actual!=expected || !testBitStringCallCounters(0, 0, 1)) {
          printf("ERROR: line %d: bs.nextWord_substring(%u, %u) on %u failed\n", __LINE__, start, end, b);
          EXPECT_EQ(actual, expected);
        } else if (verbose) {
          printf("OK   : line %d: bs.nextWord_substring(%u, %u) on %u: actual: %lu, expected: %lu\n",
              __LINE__, start, end, b, actual, expected);
        }
      }
    }
  }
}

// Determine if 'nextWord' is able to extract bits over 1 byte such that the
// start bit is on a byte boundary (bb)
//
// To test make a 1-byte bitstring populated with all possible 0xff values
// then every substring over 1-bytes is extracted and tested for correctness.
TEST(bitstring, nextWord_bb_1byteend) {
  for (unsigned b=0; b<256; ++b) {
    resetBitStringCallCounters();
    const Benchmark::HTrie::htrie_byte byte = (Benchmark::HTrie::htrie_byte)(b);
    Benchmark::HTrie::BitString<1> bs(byte);
    const Benchmark::HTrie::htrie_index start = 0;
    const Benchmark::HTrie::htrie_index end = 7;
    // Get bits in [start, end]
    const Benchmark::HTrie::htrie_uint64 actual = bs.nextWord(start, end);
    const Benchmark::HTrie::htrie_uint64 expected = Benchmark::HTrie::htrie_uint64(b);
    // Inspect result
    if (actual!=expected || !testBitStringCallCounters(0, 0, 0)) {
      printf("ERROR: line %d: bs.nextWord_2byteend(%u, %u) on %u failed\n", __LINE__, start, end, b);
      EXPECT_EQ(actual, expected);
    } else if (verbose) {
      printf("OK   : line %d: bs.nextWord_2byteend(%u, %u) on %u: actual: %lu, expected: %lu\n",
        __LINE__, start, end, b, actual, expected);
      }
  }
}

// Determine if 'nextWord' is able to extract a byte suffix, prefix combo.
// In this case we start on a non-byte-boundary and end before bit-7 in the
// very next byte. This is like substring but over 2-bytes not within 1-byte
//
// To test make a 2-byte bitstring populated with all possible 0xffff values
// then every substring over 2-bytes is extracted and tested for correctness.
TEST(bitstring, nextWord_2bytesubstring) {
  for (unsigned b=0; b<65536; ++b) {
    for (unsigned start=1; start<=7; ++start) {           // w.r.t. to byte 0
      for (unsigned endDelta=0; endDelta<7; ++endDelta) { // w.r.t. to byte 1 
        resetBitStringCallCounters();
        const Benchmark::HTrie::htrie_uint16 sword = (Benchmark::HTrie::htrie_uint16)(b);
        Benchmark::HTrie::BitString<2> bs(sword);
        const Benchmark::HTrie::htrie_index end = 8+endDelta; 
        // Get bits in [start, end]
        const Benchmark::HTrie::htrie_uint64 actual = bs.nextWord(start, end);
        // Mask for byte-0 (suffix part)
        Benchmark::HTrie::htrie_uint64 loMask(0);
        for (unsigned t=start; t<=7; ++t) {
          loMask |= (1<<t);
        }
        // Mask for byte-1 (prefix part)
        Benchmark::HTrie::htrie_uint64 hiMask(0);
        for (unsigned t=0; t<=endDelta; ++t) {
          hiMask |= (1<<t);
        }
        const Benchmark::HTrie::htrie_uint64 expected = ((sword&loMask) | (sword&(hiMask<<8))) >> start;
        // Inspect result
        if (actual!=expected || !testBitStringCallCounters(1, 1, 0)) {
          printf("ERROR: line %d: bs.nextWord_2bytesubstring(%u, %u) on %u failed\n", __LINE__, start, end, b);
          EXPECT_EQ(actual, expected);
        } else if (verbose) {
          printf("OK   : line %d: bs.nextWord_2bytesubstring(%u, %u) on %u: actual: %lu, expected: %lu\n",
            __LINE__, start, end, b, actual, expected);
        }
      }
    }
  }
}

// Determine if 'nextWord' is able to bits over two bytes such that the start
// bit is on non-byte boundary (nbb) and end-bit is on a byte boundary
//
// To test make a 2-byte bitstring populated with all possible 0xffff values
// then every substring over 2-bytes is extracted and tested for correctness.
TEST(bitstring, nextWord_nbb_2byteend) {
  for (unsigned b=0; b<65536; ++b) {
    for (unsigned start=1; start<=7; ++start) {           // w.r.t. to byte 0
      resetBitStringCallCounters();
      const Benchmark::HTrie::htrie_uint16 sword = (Benchmark::HTrie::htrie_uint16)(b);
      Benchmark::HTrie::BitString<2> bs(sword);
      const Benchmark::HTrie::htrie_index end = 15;
      // Get bits in [start, end]
      const Benchmark::HTrie::htrie_uint64 actual = bs.nextWord(start, end);
      // Mask for byte-0 (suffix part)
      Benchmark::HTrie::htrie_uint64 loMask(0);
      for (unsigned t=start; t<=7; ++t) {
        loMask |= (1<<t);
      }
      const Benchmark::HTrie::htrie_uint64 hiMask = 255;
      const Benchmark::HTrie::htrie_uint64 expected = ((sword&loMask) | (sword&(hiMask<<8))) >> start;
      // Inspect result
      if (actual!=expected || !testBitStringCallCounters(1, 0, 0)) {
        printf("ERROR: line %d: bs.nextWord_nbb_2byteend(%u, %u) on %u failed\n", __LINE__, start, end, b);
        EXPECT_EQ(actual, expected);
      } else if (verbose) {
        printf("OK   : line %d: bs.nextWord_nbb_2byteend(%u, %u) on %u: actual: %lu, expected: %lu\n",
          __LINE__, start, end, b, actual, expected);
      }
    }
  }
}

// Determine if 'nextWord' is able to extract bits over two bytes such that the
// start bit is on a byte boundary (bb)
//
// To test make a 2-byte bitstring populated with all possible 0xffff values
// then every substring over 2-bytes is extracted and tested for correctness.
TEST(bitstring, nextWord_bb_2byteend) {
  for (unsigned b=0; b<65536; ++b) {
    resetBitStringCallCounters();
    const Benchmark::HTrie::htrie_uint16 sword = (Benchmark::HTrie::htrie_uint16)(b);
    Benchmark::HTrie::BitString<2> bs(sword);
    const Benchmark::HTrie::htrie_index start = 0;
    const Benchmark::HTrie::htrie_index end = 15;
    // Get bits in [start, end]
    const Benchmark::HTrie::htrie_uint64 actual = bs.nextWord(start, end);
    // Mask for byte-0 (suffix part)
    Benchmark::HTrie::htrie_uint64 loMask(0);
    for (unsigned t=start; t<=7; ++t) {
      loMask |= (1<<t);
    }
    const Benchmark::HTrie::htrie_uint64 hiMask = 255;
    const Benchmark::HTrie::htrie_uint64 expected = ((sword&loMask) | (sword&(hiMask<<8))) >> start;
    // Inspect result
    if (actual!=expected || !testBitStringCallCounters(0, 0, 0)) {
      printf("ERROR: line %d: bs.nextWord_bb_2byteend(%u, %u) on %u failed\n", __LINE__, start, end, b);
      EXPECT_EQ(actual, expected);
    } else if (verbose) {
      printf("OK   : line %d: bs.nextWord_bb_2byteend(%u, %u) on %u: actual: %lu, expected: %lu\n",
        __LINE__, start, end, b, actual, expected);
      }
  }
}

// Determine if 'nextWord' is able to extract bits over three bytes such that the
// start bit is on a byte boundary (bb)
//
// To test make a 3-byte bitstring populated with all possible 0xfff values
// then every substring over 2-bytes is extracted and tested for correctness.
TEST(bitstring, nextWord_bb_3byteend) {
  for (unsigned b=0; b<=0xfff; ++b) {
    Benchmark::HTrie::BitString<3> bs;
    for (unsigned i=0; i<24; ++i) {
      bs.append(b&(1<<i));
    }
    const Benchmark::HTrie::htrie_index start = 0;
    const Benchmark::HTrie::htrie_index end = 23;
    resetBitStringCallCounters();
    // Get bits in [start, end]
    const Benchmark::HTrie::htrie_uint64 actual = bs.nextWord(start, end);
    const Benchmark::HTrie::htrie_uint64 expected = Benchmark::HTrie::htrie_uint64(b);
    // Inspect result
    if (actual!=expected || !testBitStringCallCounters(0, 0, 0)) {
      printf("ERROR: line %d: bs.nextWord_bb_3byteend(%u, %u) on %u failed\n", __LINE__, start, end, b);
      EXPECT_EQ(actual, expected);
    } else if (verbose) {
      printf("OK   : line %d: bs.nextWord_bb_3byteend(%u, %u) on %u: actual: %lu, expected: %lu\n",
        __LINE__, start, end, b, actual, expected);
    }
  }
}

// Determine if 'nextWord' is able to extract bits over three bytes such that the
// start bit is on a non byte boundary (nbb)
//
// To test make a 3-byte bitstring populated with all possible 0xfff values
// then every substring over 3-bytes is extracted and tested for correctness.
TEST(bitstring, nextWord_nbb_3byteend) {
  for (unsigned b=0; b<=0xfff; ++b) {
    Benchmark::HTrie::BitString<3> bs;
    for (unsigned i=0; i<24; ++i) {
      bs.append(b&(1<<i));
    }
    const Benchmark::HTrie::htrie_index end = 23;
    for (unsigned start=1; start<7; ++start) {
      resetBitStringCallCounters();
      // Get bits in [start, end]
      const Benchmark::HTrie::htrie_uint64 actual = bs.nextWord(start, end);
      const Benchmark::HTrie::htrie_uint64 expected = Benchmark::HTrie::htrie_uint64(b>>start);
      // Inspect result
      if (actual!=expected || !testBitStringCallCounters(1, 0, 0)) {
        printf("ERROR: line %d: bs.nextWord_nbb_3byteend(%u, %u) on %u failed\n", __LINE__, start, end, b);
        EXPECT_EQ(actual, expected);
      } else if (verbose) {
        printf("OK   : line %d: bs.nextWord_nbb_3byteend(%u, %u) on %u: actual: %lu, expected: %lu\n",
          __LINE__, start, end, b, actual, expected);
      }
    }
  }
}

// Determine if 'nextWord' is able to extract bits over four bytes such that the
// start bit is on a byte boundary (bb)
//
// To test make an 4-byte bitstring populated with a selected subset of all
// possible values then a substring over 8-bytes is extracted and tested for
// correctness.
TEST(bitstring, nextWord_bb_4byteend) {
  // Test all values for byte 0
  for (Benchmark::HTrie::htrie_uint64 b=0; b<=0xff; ++b) {
    resetBitStringCallCounters();
    const Benchmark::HTrie::htrie_uint64 expected(b);
    const Benchmark::HTrie::htrie_uint32 val((Benchmark::HTrie::htrie_uint)b);
    Benchmark::HTrie::BitString<4> bs(val);
    const Benchmark::HTrie::htrie_index start = 0;
    const Benchmark::HTrie::htrie_index end = 31;
    // Get bits in [start, end]
    const Benchmark::HTrie::htrie_uint64 actual = bs.nextWord(start, end);
    // Inspect result
    if (actual!=expected || !testBitStringCallCounters(0, 0, 0)) {
      printf("ERROR: line %d: bs.nextWord_bb_4byteend(%u, %u) on %lu failed\n", __LINE__, start, end, b);
      EXPECT_EQ(actual, expected);
    } else if (verbose) {
      printf("OK   : line %d: bs.nextWord_bb_4byteend(%u, %u) on %lu: actual: %lu, expected: %lu\n",
        __LINE__, start, end, b, actual, expected);
    }
  }

  // Test all values for byte1
  for (Benchmark::HTrie::htrie_uint64 b=0; b<=0xff; ++b) {
    resetBitStringCallCounters();
    const Benchmark::HTrie::htrie_uint64 expected(b<<8);
    const Benchmark::HTrie::htrie_uint32 val((Benchmark::HTrie::htrie_uint)(b<<8));
    Benchmark::HTrie::BitString<4> bs(val);
    const Benchmark::HTrie::htrie_index start = 0;
    const Benchmark::HTrie::htrie_index end = 31;
    // Get bits in [start, end]
    const Benchmark::HTrie::htrie_uint64 actual = bs.nextWord(start, end);
    // Inspect result
    if (actual!=expected || !testBitStringCallCounters(0, 0, 0)) {
      printf("ERROR: line %d: bs.nextWord_bb_4byteend(%u, %u) on %lu failed\n", __LINE__, start, end, b);
      EXPECT_EQ(actual, expected);
    } else if (verbose) {
      printf("OK   : line %d: bs.nextWord_bb_4byteend(%u, %u) on %lu: actual: %lu, expected: %lu\n",
        __LINE__, start, end, b, actual, expected);
    }
  }

  // Test all values for byte2
  for (Benchmark::HTrie::htrie_uint64 b=0; b<=0xff; ++b) {
    resetBitStringCallCounters();
    const Benchmark::HTrie::htrie_uint64 expected(b<<16);
    const Benchmark::HTrie::htrie_uint32 val((Benchmark::HTrie::htrie_uint)(b<<16));
    Benchmark::HTrie::BitString<4> bs(val);
    const Benchmark::HTrie::htrie_index start = 0;
    const Benchmark::HTrie::htrie_index end = 31;
    // Get bits in [start, end]
    const Benchmark::HTrie::htrie_uint64 actual = bs.nextWord(start, end);
    // Inspect result
    if (actual!=expected || !testBitStringCallCounters(0, 0, 0)) {
      printf("ERROR: line %d: bs.nextWord_bb_4byteend(%u, %u) on %lu failed\n", __LINE__, start, end, b);
      EXPECT_EQ(actual, expected);
    } else if (verbose) {
      printf("OK   : line %d: bs.nextWord_bb_4byteend(%u, %u) on %lu: actual: %lu, expected: %lu\n",
        __LINE__, start, end, b, actual, expected);
    }
  }

  // Test all values for byte3
  for (Benchmark::HTrie::htrie_uint64 b=0; b<=0xff; ++b) {
    resetBitStringCallCounters();
    const Benchmark::HTrie::htrie_uint64 expected(b<<24);
    const Benchmark::HTrie::htrie_uint32 val((Benchmark::HTrie::htrie_uint)(b<<24));
    Benchmark::HTrie::BitString<4> bs(val);
    const Benchmark::HTrie::htrie_index start = 0;
    const Benchmark::HTrie::htrie_index end = 31;
    // Get bits in [start, end]
    const Benchmark::HTrie::htrie_uint64 actual = bs.nextWord(start, end);
    // Inspect result
    if (actual!=expected || !testBitStringCallCounters(0, 0, 0)) {
      printf("ERROR: line %d: bs.nextWord_bb_4byteend(%u, %u) on %lu failed\n", __LINE__, start, end, b);
      EXPECT_EQ(actual, expected);
    } else if (verbose) {
      printf("OK   : line %d: bs.nextWord_bb_4byteend(%u, %u) on %lu: actual: %lu, expected: %lu\n",
        __LINE__, start, end, b, actual, expected);
    }
  }
}

TEST(bitstring, nextword_debug) {
  const Benchmark::HTrie::htrie_uint64 b = 4728986788662773602UL;
  Benchmark::HTrie::BitString<8> bs(b);
  const Benchmark::HTrie::htrie_uint64 start = 8;
  const Benchmark::HTrie::htrie_index end = 63;
  const Benchmark::HTrie::htrie_uint64 actual = bs.optNextWord(start, end);
}

TEST(bitstring, nextWord_random) {
  seedrng=false;
  for (Benchmark::HTrie::htrie_uint64 max=1; max<=64; ++max) {
    randomSpan(max);
  }
}

TEST(bitstring, nextWord_random_seed) {
  seedrng=true;
  for (Benchmark::HTrie::htrie_uint64 max=1; max<=64; ++max) {
    randomSpan(max);
  }
}
