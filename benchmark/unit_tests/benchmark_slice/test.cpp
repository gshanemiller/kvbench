#include <benchmark_slice.h>
#include <gtest/gtest.h>

#include <string>

TEST(slice, size) {
  Benchmark::Slice<char> slice;
  EXPECT_EQ(sizeof(u_int64_t), sizeof(slice));

  Benchmark::Slice<unsigned char> uslice;
  EXPECT_EQ(sizeof(u_int64_t), sizeof(uslice));
}

TEST(slice, charEmpty) {
  Benchmark::Slice<char> cSlice;
  EXPECT_EQ(cSlice.size(), 0);
  EXPECT_TRUE(cSlice.data()==0);
  void *ptr = static_cast<void*>(cSlice);
  EXPECT_TRUE(ptr==0);

  Benchmark::Slice<char> cCopySlice(cSlice);
  EXPECT_EQ(cCopySlice.size(), 0);
  EXPECT_TRUE(cCopySlice.data()==0);
  ptr = static_cast<void*>(cCopySlice);
  EXPECT_TRUE(ptr==0);

  Benchmark::Slice<char> cCCopySlice = cSlice;
  EXPECT_EQ(cCCopySlice.size(), 0);
  EXPECT_TRUE(cCCopySlice.data()==0);
  ptr = static_cast<void*>(cCCopySlice);
  EXPECT_TRUE(ptr==0);
}

TEST(slice, constCharEmpty) {
  const Benchmark::Slice<char> cSlice;
  EXPECT_EQ(cSlice.size(), 0);
  EXPECT_TRUE(cSlice.data()==0);
  const void *ptr = static_cast<const void*>(cSlice);
  EXPECT_TRUE(ptr==0);

  const Benchmark::Slice<char> cCopySlice(cSlice);
  EXPECT_EQ(cCopySlice.size(), 0);
  EXPECT_TRUE(cCopySlice.data()==0);
  ptr = static_cast<const void*>(cCopySlice);
  EXPECT_TRUE(ptr==0);

  const Benchmark::Slice<char> cCCopySlice = cSlice;
  EXPECT_EQ(cCCopySlice.size(), 0);
  EXPECT_TRUE(cCCopySlice.data()==0);
  ptr = static_cast<const void*>(cCCopySlice);
  EXPECT_TRUE(ptr==0);
}

TEST(slice, unsignedCharEmpty) {
  Benchmark::Slice<unsigned char> cSlice;
  EXPECT_EQ(cSlice.size(), 0);
  EXPECT_TRUE(cSlice.data()==0);
  void *ptr = static_cast<void*>(cSlice);
  EXPECT_TRUE(ptr==0);

  Benchmark::Slice<unsigned char> cCopySlice(cSlice);
  EXPECT_EQ(cCopySlice.size(), 0);
  EXPECT_TRUE(cCopySlice.data()==0);
  ptr = static_cast<void*>(cCopySlice);
  EXPECT_TRUE(ptr==0);

  Benchmark::Slice<unsigned char> cCCopySlice = cSlice;
  EXPECT_EQ(cCCopySlice.size(), 0);
  EXPECT_TRUE(cCCopySlice.data()==0);
  ptr = static_cast<void*>(cCCopySlice);
  EXPECT_TRUE(ptr==0);
}

TEST(slice, constUnsignedCharEmpty) {
  const Benchmark::Slice<unsigned char> cSlice;
  EXPECT_EQ(cSlice.size(), 0);
  EXPECT_TRUE(cSlice.data()==0);
  const void *ptr = static_cast<const void*>(cSlice);
  EXPECT_TRUE(ptr==0);

  const Benchmark::Slice<unsigned char> cCopySlice(cSlice);
  EXPECT_EQ(cCopySlice.size(), 0);
  EXPECT_TRUE(cCopySlice.data()==0);
  ptr = static_cast<const void*>(cCopySlice);
  EXPECT_TRUE(ptr==0);

  const Benchmark::Slice<unsigned char> cCCopySlice = cSlice;
  EXPECT_EQ(cCCopySlice.size(), 0);
  EXPECT_TRUE(cCCopySlice.data()==0);
  ptr = static_cast<const void*>(cCCopySlice);
  EXPECT_TRUE(ptr==0);
}

static const struct {
  int           d_lineNum;  // source line number
  u_int16_t     d_size;     // size of string in bytes
  u_int8_t      d_data[16]; // up to 16 bytes of data
} VALUES[] = {
  //line      size data
  //----      ---- ----------------------------
  { __LINE__,   1,  {0}                           },  // empty string
  { __LINE__,   2,  {0,1}                         },  // embedded null
  { __LINE__,   3,  {1,2,3},                      },  // non-printable bytes no null
  { __LINE__,   4,  {'a', 'A', 'C', 0},           },  // "aAC" 0 terminated string
  { __LINE__,   4,  {'a', 0,   'F', 'G'}          },  // embedded null not 0 terminated
};

const std::size_t NUM_VALUES = sizeof VALUES / sizeof *VALUES;

TEST(slice, charConstructors) {
  for (unsigned i=0; i<NUM_VALUES; ++i) {
    const u_int8_t *constTmpPtr = VALUES[i].d_data;
    u_int8_t *tmpPtr = const_cast<u_int8_t*>(constTmpPtr);
    Benchmark::Slice<char> slice(reinterpret_cast<const char*>(VALUES[i].d_data), VALUES[i].d_size);

    slice.print();

    EXPECT_EQ(slice.size(), VALUES[i].d_size);
    EXPECT_TRUE(slice.data() == reinterpret_cast<char*>(tmpPtr));

    Benchmark::Slice<char> cCopySlice(slice);
    EXPECT_EQ(cCopySlice.size(), VALUES[i].d_size);
    EXPECT_TRUE(cCopySlice.data() == reinterpret_cast<char*>(tmpPtr));

    Benchmark::Slice<char> cCCopySlice = slice;
    EXPECT_EQ(cCCopySlice.size(), VALUES[i].d_size);
    EXPECT_TRUE(cCCopySlice.data() == reinterpret_cast<char*>(tmpPtr));
  }
}

TEST(slice, constCharConstructors) {
  for (unsigned i=0; i<NUM_VALUES; ++i) {
    const u_int8_t *constTmpPtr = VALUES[i].d_data;
    u_int8_t *tmpPtr = const_cast<u_int8_t*>(constTmpPtr);
    const Benchmark::Slice<char> slice(reinterpret_cast<const char*>(VALUES[i].d_data), VALUES[i].d_size);

    slice.print();

    EXPECT_EQ(slice.size(), VALUES[i].d_size);
    EXPECT_TRUE(slice.data() == reinterpret_cast<char*>(tmpPtr));

    const Benchmark::Slice<char> cCopySlice(slice);
    EXPECT_EQ(cCopySlice.size(), VALUES[i].d_size);
    EXPECT_TRUE(cCopySlice.data() == reinterpret_cast<char*>(tmpPtr));

    const Benchmark::Slice<char> cCCopySlice = slice;
    EXPECT_EQ(cCCopySlice.size(), VALUES[i].d_size);
    EXPECT_TRUE(cCCopySlice.data() == reinterpret_cast<char*>(tmpPtr));
  }
}

TEST(slice, ucharConstructors) {
  for (unsigned i=0; i<NUM_VALUES; ++i) {
    const u_int8_t *constTmpPtr = VALUES[i].d_data;
    u_int8_t *tmpPtr = const_cast<u_int8_t*>(constTmpPtr);
    Benchmark::Slice<unsigned char> slice(VALUES[i].d_data, VALUES[i].d_size);

    slice.print();

    EXPECT_EQ(slice.size(), VALUES[i].d_size);
    EXPECT_TRUE(slice.data() == tmpPtr);

    Benchmark::Slice<unsigned char> cCopySlice(slice);
    EXPECT_EQ(cCopySlice.size(), VALUES[i].d_size);
    EXPECT_TRUE(cCopySlice.data() == tmpPtr);

    Benchmark::Slice<unsigned char> cCCopySlice = slice;
    EXPECT_EQ(cCCopySlice.size(), VALUES[i].d_size);
    EXPECT_TRUE(cCCopySlice.data() == tmpPtr);
  }
}

TEST(slice, constUCharConstructors) {
  for (unsigned i=0; i<NUM_VALUES; ++i) {
    const u_int8_t *constTmpPtr = VALUES[i].d_data;
    const Benchmark::Slice<unsigned char> slice(VALUES[i].d_data, VALUES[i].d_size);

    slice.print();

    EXPECT_EQ(slice.size(), VALUES[i].d_size);
    EXPECT_TRUE(slice.data() == constTmpPtr);

    const Benchmark::Slice<unsigned char> cCopySlice(slice);
    EXPECT_EQ(cCopySlice.size(), VALUES[i].d_size);
    EXPECT_TRUE(cCopySlice.data() == constTmpPtr);

    const Benchmark::Slice<unsigned char> cCCopySlice = slice;
    EXPECT_EQ(cCCopySlice.size(), VALUES[i].d_size);
    EXPECT_TRUE(cCCopySlice.data() == constTmpPtr);
  }
}

TEST(slice, charReset) {
  for (unsigned i=0; i<NUM_VALUES; ++i) {
    const u_int8_t *constTmpPtr = VALUES[i].d_data;

    Benchmark::Slice<char> slice;
    slice.reset(reinterpret_cast<const char*>(constTmpPtr), VALUES[i].d_size);

    EXPECT_EQ(slice.size(), VALUES[i].d_size);
    EXPECT_TRUE(slice.data() == reinterpret_cast<const char*>(constTmpPtr));
  }
}

TEST(slice, ucharReset) {
  for (unsigned i=0; i<NUM_VALUES; ++i) {
    const u_int8_t *constTmpPtr = VALUES[i].d_data;

    Benchmark::Slice<unsigned char> slice;
    slice.reset(constTmpPtr, VALUES[i].d_size);

    EXPECT_EQ(slice.size(), VALUES[i].d_size);
    EXPECT_TRUE(slice.data() == constTmpPtr);
  }
}

TEST(slice, charCastVoidPtr) {
  for (unsigned i=0; i<NUM_VALUES; ++i) {
      const u_int8_t *constTmpPtr = VALUES[i].d_data;

      Benchmark::Slice<char> sliceA(reinterpret_cast<char*>(constTmpPtr), VALUES[i].d_size);
      void *actutalPtrA = static_cast<void*>(sliceA);
      const u_int64_t uptrA = reinterpret_cast<u_int64_t>(constTmpPtr) | (u_int64_t)VALUES[i].d_size << 48;
      const void *expectedPtrA= reinterpret_cast<const void*>(uptrA);
      EXPECT_TRUE(actutalPtrA == expectedPtrA);

      const Benchmark::Slice<char> sliceB(reinterpret_cast<char*>(constTmpPtr), VALUES[i].d_size);
      const void *actualPtrB = static_cast<void*>(sliceB);
      const u_int64_t uptrB = reinterpret_cast<u_int64_t>(constTmpPtr) | (u_int64_t)VALUES[i].d_size << 48;
      const void *expectedPtrB= reinterpret_cast<const void*>(uptrB);
      EXPECT_TRUE(actutalPtrB == expectedPtrB);
  }
}

TEST(slice, ucharCastVoidPtr) {
  for (unsigned i=0; i<NUM_VALUES; ++i) {
      const u_int8_t *constTmpPtr = VALUES[i].d_data;

      Benchmark::Slice<unsigned char> sliceA(constTmpPtr, VALUES[i].d_size);
      void *actutalPtrA = static_cast<void*>(sliceA);
      const u_int64_t uptrA = reinterpret_cast<u_int64_t>(constTmpPtr) | (u_int64_t)VALUES[i].d_size << 48;
      const void *expectedPtrA= reinterpret_cast<const void*>(uptrA);
      EXPECT_TRUE(actutalPtrA == expectedPtrA);

      const Benchmark::Slice<unsigned char> sliceB(constTmpPtr, VALUES[i].d_size);
      const void *actualPtrB = static_cast<void*>(sliceB);
      const u_int64_t uptrB = reinterpret_cast<u_int64_t>(constTmpPtr) | (u_int64_t)VALUES[i].d_size << 48;
      const void *expectedPtrB= reinterpret_cast<const void*>(uptrB);
      EXPECT_TRUE(actutalPtrB == expectedPtrB);
  }
}

TEST(slice, charEqual) {
  for (unsigned i=0; i<NUM_VALUES; ++i) {
    for (unsigned j=0; j<NUM_VALUES; ++j) {
      const u_int8_t *constTmpPtrA = VALUES[i].d_data;
      Benchmark::Slice<char> sliceA(reinterpret_cast<char*>(constTmpPtrA), VALUES[i].d_size);
      const u_int64_t uptrA = reinterpret_cast<u_int64_t>(constTmpPtrA) | (u_int64_t)VALUES[i].d_size << 48;
      const void *ptrA = reinterpret_cast<const void*>(uptrA);

      const u_int8_t *constTmpPtrB = VALUES[j].d_data;
      Benchmark::Slice<char> sliceB(reinterpret_cast<char*>(constTmpPtrB), VALUES[j].d_size);
      const u_int64_t uptrB = reinterpret_cast<u_int64_t>(constTmpPtrB) | (u_int64_t)VALUES[j].d_size << 48;
      const void *ptrB = reinterpret_cast<const void*>(uptrB);

      if (i==j) {
        EXPECT_EQ(sliceA.equal(ptrB), true);
        EXPECT_EQ(sliceB.equal(ptrA), true);
      } else {
        EXPECT_EQ(sliceA.equal(ptrB), false);
        EXPECT_EQ(sliceB.equal(ptrA), false);
      }
    }
  }
}

TEST(slice, ucharEqual) {
  for (unsigned i=0; i<NUM_VALUES; ++i) {
    for (unsigned j=0; j<NUM_VALUES; ++j) {
      const u_int8_t *constTmpPtrA = VALUES[i].d_data;
      Benchmark::Slice<unsigned char> sliceA(constTmpPtrA, VALUES[i].d_size);
      const u_int64_t uptrA = reinterpret_cast<u_int64_t>(constTmpPtrA) | (u_int64_t)VALUES[i].d_size << 48;
      const void *ptrA = reinterpret_cast<const void*>(uptrA);

      const u_int8_t *constTmpPtrB = VALUES[j].d_data;
      Benchmark::Slice<unsigned char> sliceB(constTmpPtrB, VALUES[j].d_size);
      const u_int64_t uptrB = reinterpret_cast<u_int64_t>(constTmpPtrB) | (u_int64_t)VALUES[j].d_size << 48;
      const void *ptrB = reinterpret_cast<const void*>(uptrB);

      if (i==j) {
        EXPECT_EQ(sliceA.equal(ptrB), true);
        EXPECT_EQ(sliceB.equal(ptrA), true);
      } else {
        EXPECT_EQ(sliceA.equal(ptrB), false);
        EXPECT_EQ(sliceB.equal(ptrA), false);
      }
    }
  }
}
