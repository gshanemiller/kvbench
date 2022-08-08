#include <benchmark_slice.h>
#include <cradix_tree.h>
#include <cradix_memmanager.h>
#include <gtest/gtest.h>
#include <algorithm>
#include <vector>

static const struct {
  int           d_lineNum;  // source line number
  u_int16_t     d_size;     // size of string in bytes
  u_int8_t      d_data[16]; // up to 16 bytes of data
} REFERENCE_VALUES[] = {
  //line      size data
  //----      ---- ----------------------------
  { __LINE__,   1,  {'A'}                                               },
  { __LINE__,   1,  {'P'}                                               },
  { __LINE__,   1,  {'Z'}                                               },

  { __LINE__,   2,  {'A', 'o'}                                          },
  { __LINE__,   2,  {'A', 'a'}                                          },
  { __LINE__,   2,  {'A', 'z'}                                          },

  { __LINE__,   3,  {'A', 'o', 'o'}                                     },
  { __LINE__,   3,  {'A', 'o', 'a'}                                     },
  { __LINE__,   3,  {'A', 'o', 'z'}                                     },

  { __LINE__,   4,  {'P', 'i', 'Z', 'a'}                                },
};

const std::size_t NUM_REFERENCE_VALUES = sizeof REFERENCE_VALUES / sizeof *REFERENCE_VALUES;
const u_int32_t bufferSize = 0x100000;

// Case 0a one leaf at a time
//
// Before            After
// +--------+       +--------+
// |  root  |  |->  |  root  | -> 0xff
// +--------+       +--------+
//
// Test: On empty tree, add a key of size 1
//
TEST(cradix, case0a) {
  u_int8_t byte; 
  CRadix::TreeStats stats;

  for (unsigned i=0; i<CRadix::k_MAX_CHILDREN; ++i) {
    byte = i;
    Benchmark::Slice<unsigned char> key(&byte, 1);

    CRadix::MemManager mem(bufferSize, 4);;
    CRadix::Tree tree(&mem);

    int rc = tree.find(key);
    assert(rc==CRadix::e_NOT_FOUND);
    
    rc = tree.insert(key);
    assert(rc==CRadix::e_OK);
    
    rc = tree.find(key);
    assert(rc==CRadix::e_EXISTS);

    tree.statistics(&stats);
    EXPECT_EQ(stats.d_innerNodeCount, 0);
    EXPECT_EQ(stats.d_leafCount, 1);
    EXPECT_EQ(stats.d_terminalCount, 1);
    EXPECT_EQ(stats.d_emptyChildCount, 255);
    EXPECT_EQ(stats.d_maxDepth, 1);
    EXPECT_EQ(stats.d_maxDepth, tree.currentMaxDepth());
    EXPECT_EQ(stats.d_totalCompressedSizeBytes, 1028);
    EXPECT_EQ(stats.d_totalUncompressedSizeBytes, 2048);

    CRadix::MemStats mstats;
    mem.statistics(&mstats);
    EXPECT_EQ(mstats.d_allocCount, 1);
    EXPECT_EQ(mstats.d_deadCount, 0);
    EXPECT_EQ(mstats.d_freeCount, 0);
    EXPECT_EQ(mstats.d_currentSizeBytes, 1028);
    EXPECT_EQ(mstats.d_maximumSizeBytes, 1028);
    EXPECT_EQ(mstats.d_requestedBytes, 1028);
    EXPECT_EQ(mstats.d_freedBytes, 0);
    EXPECT_EQ(mstats.d_deadBytes, 0);
    EXPECT_EQ(mstats.d_sizeBytes, bufferSize);
  }
}

// Case 0b all leafs added to one tree
TEST(cradix, case0b) {
  u_int8_t byte; 
  CRadix::MemManager mem(bufferSize, 4);
  CRadix::Tree tree(&mem);
  CRadix::TreeStats stats;

  for (unsigned i=0; i<CRadix::k_MAX_CHILDREN; ++i) {
    byte = i;
    Benchmark::Slice<unsigned char> key(&byte, 1);

    int rc = tree.find(key);
    assert(rc==CRadix::e_NOT_FOUND);
    
    rc = tree.insert(key);
    assert(rc==CRadix::e_OK);
    
    rc = tree.find(key);
    assert(rc==CRadix::e_EXISTS);

    // All previous keys should still be there
    for (unsigned j=0; j<i; j++) {
      byte = j;
      Benchmark::Slice<unsigned char> skey(&byte, 1);
      rc = tree.find(skey);
      assert(rc==CRadix::e_EXISTS);
    }
  }

  tree.statistics(&stats);
  EXPECT_EQ(stats.d_innerNodeCount, 0);
  EXPECT_EQ(stats.d_leafCount, 256);
  EXPECT_EQ(stats.d_terminalCount, 256);
  EXPECT_EQ(stats.d_emptyChildCount, 0);
  EXPECT_EQ(stats.d_maxDepth, 1);
  EXPECT_EQ(stats.d_maxDepth, tree.currentMaxDepth());
  EXPECT_EQ(stats.d_totalCompressedSizeBytes, 1028);
  EXPECT_EQ(stats.d_totalUncompressedSizeBytes, 2048);

  CRadix::MemStats mstats;
  mem.statistics(&mstats);
  EXPECT_EQ(mstats.d_allocCount, 1);
  EXPECT_EQ(mstats.d_deadCount, 0);
  EXPECT_EQ(mstats.d_freeCount, 0);
  EXPECT_EQ(mstats.d_currentSizeBytes, 1028);
  EXPECT_EQ(mstats.d_maximumSizeBytes, 1028);
  EXPECT_EQ(mstats.d_requestedBytes, 1028);
  EXPECT_EQ(mstats.d_freedBytes, 0);
  EXPECT_EQ(mstats.d_deadBytes, 0);
  EXPECT_EQ(mstats.d_sizeBytes, bufferSize);
}

// Case 1a: Update a Node256 child pointer from 0xff to bonafide Node256
//          one leaf at a time
//
//  Before                  After
// +--------+ 'o'           +--------+ 'o'    'T'
// |  root  | -> 0xff  |->  |  root  | -> m+2 -> 0xff
// +--------+               +--------+
//
// Test: on empty tree add 'o'. Then add key 'oT' pointer to 0xff must be replaced with m+2
// so it can point to leaf node 'T' without losing 'o' as a valid key
//
TEST(cradix, case1a) {
  u_int8_t byte[2];
  CRadix::TreeStats stats;

  for (unsigned i=0; i<CRadix::k_MAX_CHILDREN; ++i) {
    byte[0] = i;
    Benchmark::Slice<unsigned char> key(byte+0, 1);

    CRadix::MemManager mem(bufferSize, 4);;
    CRadix::Tree tree(&mem);

    int rc = tree.find(key);
    assert(rc==CRadix::e_NOT_FOUND);
    
    rc = tree.insert(key);
    assert(rc==CRadix::e_OK);

    rc = tree.find(key);
    assert(rc==CRadix::e_EXISTS);

    // Now add key {<i>,'T'} so that <i> is a prefix of {<i>,'T'} 
    byte[1] = 'T';
    Benchmark::Slice<unsigned char> key1(byte+0, 2);

    rc = tree.find(key1);
    assert(rc==CRadix::e_NOT_FOUND);
    
    rc = tree.insert(key1);
    assert(rc==CRadix::e_OK);

    rc = tree.find(key1);
    assert(rc==CRadix::e_EXISTS);

    tree.statistics(&stats);
    EXPECT_EQ(stats.d_innerNodeCount, 1);
    EXPECT_EQ(stats.d_leafCount, 1);
    EXPECT_EQ(stats.d_terminalCount, 2);
    EXPECT_EQ(stats.d_emptyChildCount, 255);
    EXPECT_EQ(stats.d_maxDepth, 2);
    EXPECT_EQ(stats.d_maxDepth, tree.currentMaxDepth());
    EXPECT_EQ(stats.d_totalCompressedSizeBytes, 1048);
    EXPECT_EQ(stats.d_totalUncompressedSizeBytes, 4096);

    CRadix::MemStats mstats;
    mem.statistics(&mstats);
    EXPECT_EQ(mstats.d_allocCount, 2);
    EXPECT_EQ(mstats.d_deadCount, 0);
    EXPECT_EQ(mstats.d_freeCount, 0);
    EXPECT_EQ(mstats.d_currentSizeBytes, 1048);
    EXPECT_EQ(mstats.d_maximumSizeBytes, 1048);
    EXPECT_EQ(mstats.d_requestedBytes, 1048);
    EXPECT_EQ(mstats.d_freedBytes, 0);
    EXPECT_EQ(mstats.d_deadBytes, 0);
    EXPECT_EQ(mstats.d_sizeBytes, bufferSize);
  }
}

// Redo case 1a except all work done to same tree
TEST (cradix, case1b) {
  CRadix::MemManager mem(bufferSize, 4);;
  CRadix::Tree tree(&mem);
  u_int8_t byte[2];

  for (unsigned i=0; i<CRadix::k_MAX_CHILDREN; ++i) {
    byte[0] = i;
    Benchmark::Slice<unsigned char> key(byte+0, 1);

    int rc = tree.find(key);
    assert(rc==CRadix::e_NOT_FOUND);
    
    rc = tree.insert(key);
    assert(rc==CRadix::e_OK);
    
    rc = tree.find(key);
    assert(rc==CRadix::e_EXISTS);

    // Now add key {<i>,'T'} so that <i> is a prefix of {<i>,'T'} 
    byte[1] = 'T';
    Benchmark::Slice<unsigned char> key1(byte+0, 2);

    rc = tree.find(key1);
    assert(rc==CRadix::e_NOT_FOUND);
    
    rc = tree.insert(key1);
    assert(rc==CRadix::e_OK);
    
    rc = tree.find(key1);
    assert(rc==CRadix::e_EXISTS);
  }

  CRadix::TreeStats stats;
  tree.statistics(&stats);
  EXPECT_EQ(stats.d_innerNodeCount, 256);
  EXPECT_EQ(stats.d_leafCount, 256);
  EXPECT_EQ(stats.d_terminalCount, 512);
  EXPECT_EQ(stats.d_emptyChildCount, 0);
  EXPECT_EQ(stats.d_maxDepth, 2);
  EXPECT_EQ(stats.d_maxDepth, tree.currentMaxDepth());
  EXPECT_EQ(stats.d_totalCompressedSizeBytes, 6148);
  EXPECT_EQ(stats.d_totalUncompressedSizeBytes, 526336);

  CRadix::MemStats mstats;
  mem.statistics(&mstats);
  EXPECT_EQ(mstats.d_allocCount, 257);
  EXPECT_EQ(mstats.d_deadCount, 0);
  EXPECT_EQ(mstats.d_freeCount, 0);
  EXPECT_EQ(mstats.d_currentSizeBytes, 6148);
  EXPECT_EQ(mstats.d_maximumSizeBytes, 6148);
  EXPECT_EQ(mstats.d_requestedBytes, 6148);
  EXPECT_EQ(mstats.d_freedBytes, 0);
  EXPECT_EQ(mstats.d_deadBytes, 0);
  EXPECT_EQ(mstats.d_sizeBytes, bufferSize);
}

// Case 2a: Update a Node256 from 'm' to 'm+2'
//
//  Before                  After
// +--------+ 'P'   'o'          +--------+ 'P'     'o'
// |  root  | -> m  -> 0xff  |-> |  root  | ->  m+2 -> 0xff
// +--------+                    +--------+
//
// Test: On empty tree add key 'Po'. Then add key 'P'.
//
TEST(cradix, case2a) {
  u_int8_t byte[2];
  CRadix::TreeStats stats;

  for (unsigned i=0; i<CRadix::k_MAX_CHILDREN; ++i) {
    byte[0] = i;
    byte[1] = 'o';
    Benchmark::Slice<unsigned char> key(byte+0, 2);

    CRadix::MemManager mem(bufferSize, 4);;
    CRadix::Tree tree(&mem);

    int rc = tree.find(key);
    assert(rc==CRadix::e_NOT_FOUND);
    
    rc = tree.insert(key);
    assert(rc==CRadix::e_OK);

    rc = tree.find(key);
    assert(rc==CRadix::e_EXISTS);

    // Now add key '<i>' which is a prefix if key
    Benchmark::Slice<unsigned char> key1(byte+0, 1);

    rc = tree.find(key1);
    assert(rc==CRadix::e_NOT_FOUND);
    
    rc = tree.insert(key1);
    assert(rc==CRadix::e_OK);
    
    rc = tree.find(key1);
    assert(rc==CRadix::e_EXISTS);

    tree.statistics(&stats);
    EXPECT_EQ(stats.d_innerNodeCount, 1);
    EXPECT_EQ(stats.d_leafCount, 1);
    EXPECT_EQ(stats.d_terminalCount, 2);
    EXPECT_EQ(stats.d_emptyChildCount, 255);
    EXPECT_EQ(stats.d_maxDepth, 2);
    EXPECT_EQ(stats.d_maxDepth, tree.currentMaxDepth());
    EXPECT_EQ(stats.d_totalCompressedSizeBytes, 1048);
    EXPECT_EQ(stats.d_totalUncompressedSizeBytes, 4096);

    CRadix::MemStats mstats;
    mem.statistics(&mstats);
    EXPECT_EQ(mstats.d_allocCount, 2);
    EXPECT_EQ(mstats.d_deadCount, 0);
    EXPECT_EQ(mstats.d_freeCount, 0);
    EXPECT_EQ(mstats.d_currentSizeBytes, 1048);
    EXPECT_EQ(mstats.d_maximumSizeBytes, 1048);
    EXPECT_EQ(mstats.d_requestedBytes, 1048);
    EXPECT_EQ(mstats.d_freedBytes, 0);
    EXPECT_EQ(mstats.d_deadBytes, 0);
    EXPECT_EQ(mstats.d_sizeBytes, bufferSize);
  }
}

// Redo case 2a except all work done to same tree
TEST (cradix, case2b) {
  CRadix::MemManager mem(bufferSize, 4);;
  CRadix::Tree tree(&mem);
  u_int8_t byte[2];

  for (unsigned i=0; i<CRadix::k_MAX_CHILDREN; ++i) {
    byte[0] = i;
    byte[1] = 'o';
    Benchmark::Slice<unsigned char> key(byte+0, 2);

    int rc = tree.find(key);
    assert(rc==CRadix::e_NOT_FOUND);
    
    rc = tree.insert(key);
    assert(rc==CRadix::e_OK);
    
    rc = tree.find(key);
    assert(rc==CRadix::e_EXISTS);

    // Now add key '<i>' so it's a prefix of key
    Benchmark::Slice<unsigned char> key1(byte+0, 1);

    rc = tree.find(key1);
    assert(rc==CRadix::e_NOT_FOUND);
    
    rc = tree.insert(key1);
    assert(rc==CRadix::e_OK);
    
    rc = tree.find(key1);
    assert(rc==CRadix::e_EXISTS);
  }

  CRadix::TreeStats stats;
  tree.statistics(&stats);
  EXPECT_EQ(stats.d_innerNodeCount, 256);
  EXPECT_EQ(stats.d_leafCount, 256);
  EXPECT_EQ(stats.d_terminalCount, 512);
  EXPECT_EQ(stats.d_emptyChildCount, 0);
  EXPECT_EQ(stats.d_maxDepth, 2);
  EXPECT_EQ(stats.d_maxDepth, tree.currentMaxDepth());
  EXPECT_EQ(stats.d_totalCompressedSizeBytes, 6148);
  EXPECT_EQ(stats.d_totalUncompressedSizeBytes, 526336);

  CRadix::MemStats mstats;
  mem.statistics(&mstats);
  EXPECT_EQ(mstats.d_allocCount, 257);
  EXPECT_EQ(mstats.d_deadCount, 0);
  EXPECT_EQ(mstats.d_freeCount, 0);
  EXPECT_EQ(mstats.d_currentSizeBytes, 6148);
  EXPECT_EQ(mstats.d_maximumSizeBytes, 6148);
  EXPECT_EQ(mstats.d_requestedBytes, 6148);
  EXPECT_EQ(mstats.d_freedBytes, 0);
  EXPECT_EQ(mstats.d_deadBytes, 0);
  EXPECT_EQ(mstats.d_sizeBytes, bufferSize);
}

TEST (cradix, multiInsertPermuations) {
  std::vector<unsigned> perm;
  for (unsigned i=0; i<NUM_REFERENCE_VALUES; ++i) {
    perm.push_back(i);
  }

  do {
    CRadix::MemManager mem(bufferSize, 4);;
    CRadix::Tree tree(&mem);

    for (unsigned i=0; i<perm.size(); ++i) {
      Benchmark::Slice<unsigned char> key(REFERENCE_VALUES[perm[i]].d_data, REFERENCE_VALUES[perm[i]].d_size);

      int rc = tree.find(key);
      assert(rc==CRadix::e_NOT_FOUND);
    
      rc = tree.insert(key);
      assert(rc==CRadix::e_OK);
    
      rc = tree.find(key);
      assert(rc==CRadix::e_EXISTS);

      // All previous keys should still be found
      for (unsigned j=0; j<i; j++) {
        Benchmark::Slice<unsigned char> key(REFERENCE_VALUES[perm[j]].d_data, REFERENCE_VALUES[perm[j]].d_size);
        int rc = tree.find(key);
        assert(rc==CRadix::e_EXISTS);
      }
    }

    CRadix::TreeStats stats;
    tree.statistics(&stats);
    EXPECT_EQ(stats.d_innerNodeCount, 5);
    EXPECT_EQ(stats.d_leafCount, 7);
    EXPECT_EQ(stats.d_terminalCount, 10);
    EXPECT_EQ(stats.d_emptyChildCount, 299);
    EXPECT_EQ(stats.d_maxDepth, 4);
    EXPECT_EQ(stats.d_maxDepth, tree.currentMaxDepth());
    EXPECT_EQ(stats.d_totalCompressedSizeBytes, 1304);
    EXPECT_EQ(stats.d_totalUncompressedSizeBytes, 12288);

    CRadix::MemStats mstats;
    mem.statistics(&mstats);
/*
    EXPECT_EQ(mstats.d_allocCount, 10);
    EXPECT_EQ(mstats.d_deadCount, 4);
    EXPECT_EQ(mstats.d_freeCount, 0);
    EXPECT_EQ(mstats.d_currentSizeBytes, 1472);
    EXPECT_EQ(mstats.d_maximumSizeBytes, 1472);
    EXPECT_EQ(mstats.d_requestedBytes, 1472);
    EXPECT_EQ(mstats.d_freedBytes, 0);
    EXPECT_EQ(mstats.d_deadBytes, 168);
    EXPECT_EQ((mstats.d_currentSizeBytes-mstats.d_deadBytes), stats.d_totalCompressedSizeBytes);
*/
    EXPECT_EQ(mstats.d_sizeBytes, bufferSize);
  } while(std::next_permutation(perm.begin(), perm.end()));
}
