#include <trie/benchmark_htrie_bitstring.h>
#include <gtest/gtest.h>

static bool verbose=false;

static void resetBitStringCallCounters() {
  Benchmark::HTrie::BitStringStats::d_byteSuffixCalls = 0;
  Benchmark::HTrie::BitStringStats::d_bytePrefixCalls = 0;
  Benchmark::HTrie::BitStringStats::d_substringCalls = 0;
}

static bool testBitStringCallCounters(unsigned int expectedByteSuffixCalls,
    unsigned int expectedBytePrefixCalls, unsigned int expectedSubstringCalls) {
  EXPECT_EQ(expectedByteSuffixCalls, Benchmark::HTrie::BitStringStats::d_byteSuffixCalls);
  EXPECT_EQ(expectedBytePrefixCalls, Benchmark::HTrie::BitStringStats::d_bytePrefixCalls);
  EXPECT_EQ(expectedSubstringCalls, Benchmark::HTrie::BitStringStats::d_substringCalls);
  return (expectedByteSuffixCalls==Benchmark::HTrie::BitStringStats::d_byteSuffixCalls &&
      expectedBytePrefixCalls==Benchmark::HTrie::BitStringStats::d_bytePrefixCalls &&
      expectedSubstringCalls==Benchmark::HTrie::BitStringStats::d_substringCalls);
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
      const Benchmark::HTrie::htrie_word actual = bs.byteSuffix(i);
      const Benchmark::HTrie::htrie_word expected = (b&254)>>i;
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
        const Benchmark::HTrie::htrie_word actual = bs.bytePrefix(0, end, shift);
        // Construct a mask with loop unlike bytePrefix
        Benchmark::HTrie::htrie_word mask(0);
        for (unsigned t=0; t<=end; ++t) {
          mask |= (1<<t);
        }
        // bytePrefix never grabs last bit so make sure expected cannot have it
        EXPECT_EQ(unsigned(0), mask&128);
        const Benchmark::HTrie::htrie_word expected = (byte & mask) << shift;
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
        const Benchmark::HTrie::htrie_word actual = bs.substring(start, end);
        // Unlike 'substring' construct expected result with loops
        Benchmark::HTrie::htrie_word tmp(0);
        for (unsigned t=0; t<7; ++t) {
          if (t>=start && t<=end) {
            tmp |= ((1<<t)&byte); // copy bit 't' into tmp
          }
        }
        const Benchmark::HTrie::htrie_word expected(tmp >> start);
        // Inspect result
        if (actual!=expected || !testBitStringCallCounters(0, 0, 1)) {
          printf("ERROR: line %d: bs.substring(%u, %u) on %u failed\n", __LINE__, start, end, b);
          EXPECT_EQ(actual, expected);
        } else if (verbose) {
          printf("OK   : line %d: bs.substring(%u, %u) on %u: actual: %lu, expected: %lu\n",
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
      const Benchmark::HTrie::htrie_word actual = bs.nextWord(0, end);
      // Construct a mask with loop unlike bytePrefix
      Benchmark::HTrie::htrie_word mask(0);
      for (unsigned t=0; t<=end; ++t) {
        mask |= (1<<t);
      }
      // bytePrefix never grabs last bit so make sure expected cannot have it
      EXPECT_EQ(unsigned(0), mask&128);
      const Benchmark::HTrie::htrie_word expected = (byte & mask);
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
        const Benchmark::HTrie::htrie_word actual = bs.nextWord(start, end);
        // Unlike 'substring' construct expected result with loops
        Benchmark::HTrie::htrie_word tmp(0);
        for (unsigned t=0; t<7; ++t) {
          if (t>=start && t<=end) {
            tmp |= ((1<<t)&byte); // copy bit 't' into tmp
          }
        }
        const Benchmark::HTrie::htrie_word expected(tmp >> start);
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
        const Benchmark::HTrie::htrie_sword sword = (Benchmark::HTrie::htrie_sword)(b);
        Benchmark::HTrie::BitString<2> bs(sword);
        const Benchmark::HTrie::htrie_index end = 8+endDelta; 
        // Get bits in [start, end]
        const Benchmark::HTrie::htrie_word actual = bs.nextWord(start, end);
        // Mask for byte-0 (suffix part)
        Benchmark::HTrie::htrie_word loMask(0);
        for (unsigned t=start; t<=7; ++t) {
          loMask |= (1<<t);
        }
        // Mask for byte-1 (prefix part)
        Benchmark::HTrie::htrie_word hiMask(0);
        for (unsigned t=0; t<=endDelta; ++t) {
          hiMask |= (1<<t);
        }
        const Benchmark::HTrie::htrie_word expected = ((sword&loMask) | (sword&(hiMask<<8))) >> start;
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
