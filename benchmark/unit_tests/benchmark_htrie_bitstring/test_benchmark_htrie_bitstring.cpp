#include <trie/benchmark_htrie_bitstring.h>
#include <gtest/gtest.h>

static bool verbose=true;

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
      const Benchmark::HTrie::htrie_byte byte = (Benchmark::HTrie::htrie_byte)(b);
      Benchmark::HTrie::BitString<1> bs(byte);
      // Get all bits from i onwards in i's byte
      const Benchmark::HTrie::htrie_word actual = bs.byteSuffix(i);
      const Benchmark::HTrie::htrie_word expected = (b&254)>>i;
      // Inspect result
      if (actual!=expected) {
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
// To test a 1-byte bitstring will be constructed to hold all possible 255 
// values. All bitstrings of length [1,7] will be extracted and left shifted
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
        if (actual!=expected) {
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
        if (actual!=expected) {
          printf("ERROR: line %d: bs.substring(%u, %u) on %u failed\n", __LINE__, i, l, b);
          EXPECT_EQ(actual, expected);
        } else if (verbose) {
          printf("OK   : line %d: bs.substring(%u, %u) on %u: actual: %lu, expected: %lu\n", __LINE__, i, l, b, actual, expected);
        }
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
        if (actual!=expected) {
          printf("ERROR: line %d: bs.nextWord(%u, %u) on %u failed\n", __LINE__, i, l, b);
          EXPECT_EQ(actual, expected);
        } else if (verbose) {
          printf("OK   : line %d: bs.nextWord(%u, %u) on %u: actual: %lu, expected: %lu\n", __LINE__, i, l, b, actual, expected);
        }
      }
    }
  }
}


