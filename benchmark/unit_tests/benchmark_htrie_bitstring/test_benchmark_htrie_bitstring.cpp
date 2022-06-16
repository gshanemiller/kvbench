#include <trie/benchmark_htrie_bitstring.h>
#include <gtest/gtest.h>

static bool verbose=true;

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

// Bitstring tests:
// * Over 1-byte strings
//   - byteSuffix, 1-byte string (done)
//   - bytePrefix, 1-byte string (done)
//   - substring, 1-byte string  (done)
//   - nextWord deferring to byteSuffix, 1-byte string  (done)
//   - nextWord deferring to bytePrefix  1-byte string  (done)
//   - nextWord deferring to substring, 1-byte string   (done)
//   - nextWord deferring to bytePrefx+byteSuffix, 2-byte string (done)

// Determine if 'byteSuffix' is able to extract all bits from specified i to
// the end of the byte in which i occurs. i may not be on an eight byte boundary.
// Example: given a bitstring with 7 or more bytes we want to extract all the
// bits in positions 57-63 or 58-63 or 59-63 or ... 63-63 and confirm the result
// is correct regardless of what bits 56-63 contains. This test looks at one
// specific byte in the bitstring only. Other tests will test other positions.
//
// Byte 0          Byte 7
// +-----+        +-----+-----+-----+-----+-----+-----+-----+-----+
// |  0  |  ...   | 56  | 57  | 58  | 59  | 60  | 61  | 62  | 63  |  ...
// +-----+        +-----+-----+-----+-----+-----+-----+-----+-----+
//                 bit position-0                            pos-7
// 
// The possible values in bits 57-63 to are i in [0,255] & 254 since we do
// not care about bit 56 because that's on a byte boundary.
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

// Determine if 'bytePrefix' is able to extract bits '[i,i+bitLen)' where i
// is on a byte boundary and where bitLen in [1,7] and such that this bits are
// left shifted correctly to OR onto a previously obtained partial bitstring
// result. 
//
// To test a 1-byte bitstring will be constructed to hold all possible 256 
// values. All prefixes of length [1,7] will be extracted and left shifted
// onto to a preset value at all possible shift values. The final resulted is
// tested for correctness.
TEST(bitstring, bytePrefix) {
  for (unsigned b=0; b<=255; ++b) {
    for (unsigned l=1; l<8; ++l) {
      const Benchmark::HTrie::htrie_byte byte = (Benchmark::HTrie::htrie_byte)(b);
      Benchmark::HTrie::BitString<1> bs(byte);
      // Prefix extracts at most 7 bits. That means they can be left shifted
      // at most <(63-7)=56 bits assuming sizeof(htrie_word)=8 bytes or 64 bits
      for (unsigned shift=0; shift<(63-7); shift++) {
        resetBitStringCallCounters();
        const Benchmark::HTrie::htrie_word actual = bs.bytePrefix(0, l, shift);
        // Construct a mask with loop unlike bytePrefix
        Benchmark::HTrie::htrie_word mask(0);
        for (unsigned t=0; t<=l-1; ++t) {
          mask |= (1<<t);
        }
        // bytePrefix never grabs last bit so make sure expected cannot have it
        EXPECT_EQ(unsigned(0), mask&128);
        const Benchmark::HTrie::htrie_word expected = (byte & mask) << shift;
        // Inspect result
        if (actual!=expected || !testBitStringCallCounters(0, 1, 0)) {
          printf("ERROR: line %d: bs.bytePrefix(%u, %u, %u) on %u failed\n", __LINE__, 0, l, shift, b);
          EXPECT_EQ(actual, expected);
        } else if (verbose) {
          printf("OK   : line %d: bs.bytePrefix(%u, %u, %u) on %u: actual: %lu, expected: %lu\n",
            __LINE__, 0, l, shift, b, actual, expected);
        }
      }
    }
  }
}

// Determine if 'substring' is able to extract bits '[i,i+bitLen)' where i is
// on a non-byte boundary and 'i+bitLen' is in the same byte as i. Example:
// given a bitstring with 7 or more bytes we want to extract all the bits in
// positions 57-62 with i=57 and bitLen=6. This test looks at one specific byte
// in the bitstring only. Other tests will test other positions.
//
// Byte 0          Byte 7
// +-----+        +-----+-----+-----+-----+-----+-----+-----+-----+
// |  0  |  ...   | 56  | 57  | 58  | 59  | 60  | 61  | 62  | 63  |  ...
// +-----+        +-----+-----+-----+-----+-----+-----+-----+-----+
//                 bit position-0                            pos-7
// 
// The possible values in bits 57-63 to are i in [0,255] & 254 since we do
// not care about bit 56 because that's on a byte boundary.
TEST(bitstring, substring) {
  for (unsigned b=0; b<=255; ++b) {
    for (unsigned i=1; i<8; ++i) {
      for (unsigned l=1; (i+l)<=8; ++l) {
        resetBitStringCallCounters();
        const Benchmark::HTrie::htrie_byte byte = (Benchmark::HTrie::htrie_byte)(b);
        Benchmark::HTrie::BitString<1> bs(byte);
        // Get bits in [i,i+l)
        const Benchmark::HTrie::htrie_word actual = bs.substring(i, l);
        // Unlike 'substring' construct expected result with loops
        Benchmark::HTrie::htrie_word tmp(0);
        for (unsigned t=0; t<l; ++t) {
          tmp |= ((1<<(i+t))&byte);         // copy bit in [i,i+bitLen)
        }
        const Benchmark::HTrie::htrie_word expected = tmp>>i;
        // Inspect result
        if (actual!=expected || !testBitStringCallCounters(0, 0, 1)) {
          printf("ERROR: line %d: bs.substring(%u, %u) on %u failed\n", __LINE__, i, l, b);
          EXPECT_EQ(actual, expected);
        } else if (verbose) {
          printf("OK   : line %d: bs.substring(%u, %u) on %u: actual: %lu, expected: %lu\n", __LINE__, i, l, b, actual, expected);
        }
      }
    }
  }
}

// Determine if 'nextWord' is able to extract all bits from specified i to
// the end of the byte in which i occurs. i may not be on an eight byte boundary.
// If 'nextWord' works correctly it will delegate to 'byteSuffix' tested above
TEST(bitstring, nextWord_byteSuffix) {
  for (unsigned b=0; b<=255; ++b) {
    for (unsigned i=1; i<8; ++i) {
      resetBitStringCallCounters();
      const Benchmark::HTrie::htrie_byte byte = (Benchmark::HTrie::htrie_byte)(b);
      Benchmark::HTrie::BitString<1> bs(byte);
      // Get all bits from i onwards in i's byte
      const Benchmark::HTrie::htrie_word expected = (b&254)>>i;
      // Getting an actual value via nextWord requires index, and bitLen
      // Here bitLen 8-i
      const Benchmark::HTrie::htrie_word actual = bs.nextWord(i, 8-i);
      // Inspect result
      if (actual!=expected || !testBitStringCallCounters(1, 0, 0)) {
        printf("ERROR: line %d: bs.nextWord_byteSuffix(%u, %u) on %u failed\n", __LINE__, i, 8-i, b);
        EXPECT_EQ(actual, expected);
      } else if (verbose) {
        printf("OK   : line %d: bs.nextWord_byteSuffix(%u, %u) on %u: actual: %lu, expected: %lu\n", __LINE__, i, 8-i, b, actual, expected);
      }
    }
  }
}

// Determine if 'nextWord' is able to extract all bits in [i, i+bitLen] where
// is a on a 8-byte boundary and i in [1,7]. If nextWord is correct it will
// defer to bytePrefix called above
TEST(bitstring, nextWord_bytePrefix) {
  for (unsigned b=0; b<=255; ++b) {
    for (unsigned l=1; l<8; ++l) {
      resetBitStringCallCounters();
      const Benchmark::HTrie::htrie_byte byte = (Benchmark::HTrie::htrie_byte)(b);
      // Construct a mask with loop unlike bytePrefix
      Benchmark::HTrie::htrie_word mask(0);
      for (unsigned t=0; t<=l-1; ++t) {
        mask |= (1<<t);
      }
      // bytePrefix never grabs last bit so make sure expected cannot have it
      EXPECT_EQ(unsigned(0), mask&128);
      const Benchmark::HTrie::htrie_word expected = (b & mask);
      // Create actual value
      Benchmark::HTrie::BitString<1> bs(byte);
      const Benchmark::HTrie::htrie_word actual = bs.nextWord(0, l);
      // Inspect result
      if (actual!=expected || !testBitStringCallCounters(0, 1, 0)) {
        printf("ERROR: line %d: bs.nextWord_bytePrefix(%u, %u) on %u failed\n", __LINE__, 0, l, b);
        EXPECT_EQ(actual, expected);
      } else if (verbose) {
        printf("OK   : line %d: bs.nextWord_bytePrefix(%u, %u) on %u: actual: %lu, expected: %lu\n",
          __LINE__, 0, l, b, actual, expected);
      }
    }
  }
}

// Determine if 'nextWord' is able to extract bits '[i,i+bitLen)' where i is
// on a non-byte boundary and 'i+bitLen' is in the same byte as i. If nextWord
// is correct, it should defer to 'substring' which was tested immediately above.
TEST(bitstring, nextWord_substring) {
  for (unsigned b=0; b<=255; ++b) {
    for (unsigned i=1; i<8; ++i) {
      for (unsigned l=1; (i+l)<=8; ++l) {
        resetBitStringCallCounters();
        const Benchmark::HTrie::htrie_byte byte = (Benchmark::HTrie::htrie_byte)(b);
        Benchmark::HTrie::BitString<1> bs(byte);
        // Get bits in [i,i+l)
        const Benchmark::HTrie::htrie_word actual = bs.nextWord(i, l);
        // Unlike 'nextWord' construct expected result with loops
        Benchmark::HTrie::htrie_word tmp(0);
        for (unsigned t=0; t<l; ++t) {
          tmp |= ((1<<(i+t))&byte);         // copy bit in [i,i+bitLen)
        }
        const Benchmark::HTrie::htrie_word expected = tmp>>i;
        // Inspect result
        if (actual!=expected || !testBitStringCallCounters(0, 0, 1)) {
          printf("ERROR: line %d: bs.nextWord_substring(%u, %u) on %u failed\n", __LINE__, i, l, b);
          EXPECT_EQ(actual, expected);
        } else if (verbose) {
          printf("OK   : line %d: bs.nextWord_substring(%u, %u) on %u: actual: %lu, expected: %lu\n", __LINE__, i, l, b, actual, expected);
        }
      }
    }
  }
}

// Determine if 'nextWord' is able to extract bits '[i,i+bitLen)' where i is
// on a non-byte boundary and 'i+bitLen' is in the next byte after i however
// i+bitLen is also not on a byte boundry. If a nextWord call deferring to
// substring extracts within a byte this test extracts bits from two adjacent
// bytes in which neither the start or end is on a byte-boundary. 'nextWord'
// does this, if correct, by deferring to bytePrefix then byteSuffix and
// combining the results with shift/or.
//
// The test will create a two byte bitstring and populate it with all 65,536
// values. It will then extract all substrings and check result
TEST(bitstring, nextWord_prefixSuffix) {
  for (unsigned hiByte=0; hiByte<=255; ++hiByte) {
    for (unsigned lowByte=0; lowByte<=255; ++lowByte) {
      for (unsigned i=1; i<=7; ++i) {
        // now, depending on the start index 'i' need a bitLen 'l' which
        // carries us into the next byte but without ending on last bit in the
        // next byte
        const unsigned minLength = (8-i);               // minimum length into next byte
        for (unsigned l=2; (i+minLength+l-1)<15; ++l) { // additional bits to extract
          resetBitStringCallCounters();
          // The expected value comes in two parts:
          // *) bits i through bit 7 inclusive in lowByte
          // *) bits 0 through bit l inclusive in hiByte 
          const Benchmark::HTrie::htrie_byte expectedMaskLow = (0xff<<i);
          const Benchmark::HTrie::htrie_byte expectedMaskHi  = ~(0xff<<l);
          const Benchmark::HTrie::htrie_word expected =
                (Benchmark::HTrie::htrie_word)(lowByte&expectedMaskLow) |
                (((Benchmark::HTrie::htrie_word)(hiByte&expectedMaskHi))<<7); 
          // Actual source value
          Benchmark::HTrie::htrie_sword sword = 
            (Benchmark::HTrie::htrie_sword)(hiByte)<<8 | 
            (Benchmark::HTrie::htrie_sword)(lowByte);
          printf("expectedMaskLow %u, expectedMaskHi %u, i %u, length %u, value 0x%02x%02x\n",
            expectedMaskLow, expectedMaskHi, i, (minLength+l), hiByte, lowByte);
          Benchmark::HTrie::BitString<2> bs(sword);
          const Benchmark::HTrie::htrie_word actual = bs.nextWord(i, minLength+l);
          // Inspect result
          if (actual!=expected || !testBitStringCallCounters(1, 1, 0)) {
            printf("ERROR: line %d: bs.nextWord_prefixSuffix(%u, %u) on 0x%02x%02x failed\n",
              __LINE__, i, minLength+l, hiByte, lowByte);
            EXPECT_EQ(actual, expected);
          } else if (verbose) {
            printf("OK   : line %d: bs.nextWord_prefixSuffix(%u, %u) on 0x%02x%02x: actual: %lu, expected: %lu\n",
              __LINE__, i, minLength+l, hiByte, lowByte, actual, expected);
          }
        }
      }
    }
  }
}
