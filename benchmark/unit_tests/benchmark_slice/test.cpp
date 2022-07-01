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
