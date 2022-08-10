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

TEST (cradix, node256_ctor) {
    const u_int32_t index=10;
    const u_int32_t offset=0xdeadbeef;
    int32_t newMin, newMax;
  
    CRadix::MemManager mem(bufferSize, 4);;
    EXPECT_TRUE(CRadix::k_MEMMANAGER_DEFAULT_CAPACITY>0);

    u_int32_t nodeOffset = mem.newNode256(CRadix::k_MEMMANAGER_DEFAULT_CAPACITY, index, offset);
    EXPECT_TRUE(nodeOffset>=CRadix::k_MEMMANAGER_MIN_OFFSET);

    CRadix::Node256 *nodePtr = (CRadix::Node256*)(mem.basePtr()+nodeOffset);
    EXPECT_TRUE(nodePtr==mem.ptr(nodeOffset));
    EXPECT_FALSE(nodePtr->isDead());

    EXPECT_EQ(nodePtr->minIndex(), index);
    EXPECT_EQ(nodePtr->maxIndex(), index);

    // there's one item in node
    EXPECT_EQ(nodePtr->size(), 1);
    EXPECT_EQ(nodePtr->usize(), 1);

    // 1 u_int32_t was used to store 'offset'
    EXPECT_EQ(nodePtr->spareCapacity(), CRadix::k_MEMMANAGER_DEFAULT_CAPACITY-1);
    EXPECT_EQ(nodePtr->uspareCapacity(), CRadix::k_MEMMANAGER_DEFAULT_CAPACITY-1);

    // number of items thay can be stored is what's stored now + spare capacity
    EXPECT_EQ(nodePtr->capacity(), CRadix::k_MEMMANAGER_DEFAULT_CAPACITY);

    // get offset set in ctor
    EXPECT_EQ(nodePtr->offset(index), offset);
    EXPECT_EQ(nodePtr->tryOffset(index), offset);

    // reset offset @ same index
    nodePtr->setOffset(index, offset-1);
    EXPECT_EQ(nodePtr->tryOffset(index), offset-1);
    EXPECT_EQ(nodePtr->offset(index), offset-1);

    // reset offset @ same index
    nodePtr->trySetOffset(index, offset-2, newMin, newMax);
    EXPECT_EQ(nodePtr->tryOffset(index), offset-2);
    EXPECT_EQ(nodePtr->offset(index), offset-2);

    // mark dead and confirm
    nodePtr->markDead();
    EXPECT_TRUE(nodePtr->isDead());
}

TEST (cradix, node256_canSetOffset) {
    const u_int32_t index=10;
    const u_int32_t offset=0xdeadbeef;
    CRadix::MemManager mem(bufferSize, 4);;

    // Create a node with key byte 10 = 0xdeadbeef w/ default capacity
    u_int32_t nodeOffset = mem.newNode256(CRadix::k_MEMMANAGER_DEFAULT_CAPACITY, index, offset);
    CRadix::Node256 *nodePtr = mem.ptr(nodeOffset);

    // Since the min/max is 10 capcity 4 we can only set [7,10] low end to [10,13] high end
    // without memory allocation.
    u_int32_t lowest = index - CRadix::k_MEMMANAGER_DEFAULT_CAPACITY + 1;
    u_int32_t highest = index + CRadix::k_MEMMANAGER_DEFAULT_CAPACITY - 1;
    int32_t oldMin, oldMax, newMin, newMax, delta;
    for (u_int32_t i=0; i<lowest; ++i) {
      EXPECT_FALSE(nodePtr->canSetOffset(i, oldMin, oldMax, newMin, newMax, delta));
    }
    for (u_int32_t i=highest+1; i<256; ++i) {
      EXPECT_FALSE(nodePtr->canSetOffset(i, oldMin, oldMax, newMin, newMax, delta));
    }
    for (u_int32_t i=lowest; i<=highest; ++i) {
      EXPECT_TRUE(nodePtr->canSetOffset(i, oldMin, oldMax, newMin, newMax, delta));
    }
}

TEST (cradix, node256_trySetOffset_case2) {
  int32_t min, max, newMin, newMax, delta;
  const u_int32_t baseValue = 0xdeadbeef;

  // For each initial index in a new constructed node
  for(u_int32_t index=0; index<CRadix::k_MAX_CHILDREN; ++index) {
    // for each initial capacity (min of 1, max 256) a new node could have 
    for (u_int32_t capacity=1; capacity<=CRadix::k_MAX_CHILDREN; ++capacity) {
      CRadix::MemManager mem(bufferSize, 4);;
      CRadix::Tree tree(&mem);

      // Make a node with initial 'index' and initial 'capacity'
      u_int32_t nodeOffset = mem.newNode256(capacity,  index, baseValue);
      CRadix::Node256 *nodePtr = mem.ptr(nodeOffset);

      // Calc highest possible legal index we can set post construction
      u_int32_t highest(0xffff);
      if (index+capacity>=CRadix::k_MAX_CHILDREN) {
        highest = CRadix::k_MAX_CHILDREN-1;
      } else {
        highest = index + capacity - 1;
      }

      // Now try setting everything in '[index, highest]'
      u_int32_t value = baseValue;
      for (u_int32_t i=index; i<=highest; ++i, ++value) {
        printf("\n\ntry cap %u index %u :: highest %u set-index %u\n", capacity, index, highest, i);
        EXPECT_TRUE(nodePtr->trySetOffset(i, value, min, max));
        // make sure can read back value just set
        EXPECT_EQ(nodePtr->offset(i), value);
        EXPECT_EQ(nodePtr->tryOffset(i), value);
        // min should be and remain 'index'
        EXPECT_EQ(nodePtr->minIndex(), index);
        // max will increase with i
        EXPECT_EQ(nodePtr->maxIndex(), i);
        // make sure all previous values remain stable as set
        u_int32_t checkValue = baseValue;                                                                                    
        for (u_int32_t j=index; j<i; ++j, ++checkValue) {
          EXPECT_EQ(nodePtr->offset(j), checkValue);
          EXPECT_EQ(nodePtr->tryOffset(j), checkValue);
        }
      }

/*
      if (capacity!=CRadix::k_MAX_CHILDREN) {
        // Everything not in '[index, highest]' cannot be settable
        for (u_int32_t i=0; i<index; i++) {
          printf("re-check-low-side: %u\n", i);
          EXPECT_FALSE(nodePtr->canSetOffset(i, min, max, newMin, newMax, delta));
        }
        for (u_int32_t i=highest+1; i<CRadix::k_MAX_CHILDREN; i++) {
          printf("re-check-high-side: %u\n", i);
          EXPECT_FALSE(nodePtr->canSetOffset(i, min, max, newMin, newMax, delta));
        }
      } else {
        // everything is settable
        for (u_int32_t i=0; i<CRadix::k_MAX_CHILDREN; i++) {
          EXPECT_TRUE(nodePtr->canSetOffset(i, min, max, newMin, newMax, delta));
        }
      }
*/
    }
  }
}

TEST (cradix, node256_trySetOffset_case1) {
  int32_t min, max, newMin, newMax, delta;
  const u_int32_t baseValue = 0xdeadbeef;

  // For each initial index in a new constructed node
  for(u_int32_t index=0; index<CRadix::k_MAX_CHILDREN; ++index) {
    // for each initial capacity (min of 1, max 256) a new node could have 
    for (u_int32_t capacity=1; capacity<=CRadix::k_MAX_CHILDREN; ++capacity) {
      CRadix::MemManager mem(bufferSize, 4);;
      CRadix::Tree tree(&mem);

      // Make a node with initial 'index' and initial 'capacity'
      u_int32_t nodeOffset = mem.newNode256(capacity, index, baseValue);
      CRadix::Node256 *nodePtr = mem.ptr(nodeOffset);

      // Calc smallest possible legal index we can set post construction
      u_int32_t lowest(0xffff);
      if (index>=capacity) {
        lowest = index - capacity + 1;
      } else {
        lowest = 0;
      }

      // Now try setting everything in '[lowest, lowest+capacity-1]'
      u_int32_t value = baseValue;
      for (u_int32_t i=lowest; i<lowest+capacity; ++i, ++value) {
        printf("\n\ntry cap %u index %u :: lowest %u set-index %u\n", capacity, index, lowest, i);
        EXPECT_TRUE(nodePtr->trySetOffset(i, value, min, max));
        // make sure can read back value just set
        EXPECT_EQ(nodePtr->offset(i), value);
        EXPECT_EQ(nodePtr->tryOffset(i), value);
        // min should be and remain 'lowest'
        EXPECT_EQ(nodePtr->minIndex(), lowest);
        // max will stay at its initial
        // EXPECT_EQ(nodePtr->maxIndex(), index);
        // make sure all previous values remain stable as set
        u_int32_t checkValue = baseValue;                                                                                    
        for (u_int32_t j=lowest; j<i; ++j, ++checkValue) {
          EXPECT_EQ(nodePtr->offset(j), checkValue);
          EXPECT_EQ(nodePtr->tryOffset(j), checkValue);
        }
      }

/*
      if (capacity!=CRadix::k_MAX_CHILDREN) {
        // Everything not in '[lowest, lowest+capacity-1]' cannot be settable
        for (u_int32_t i=0; i<lowest; i++) {
          EXPECT_FALSE(nodePtr->canSetOffset(i, min, max, newMin, newMax, delta));
        }
        for (u_int32_t i=lowest+capacity+1; i<CRadix::k_MAX_CHILDREN; i++) {
          EXPECT_FALSE(nodePtr->canSetOffset(i, min, max, newMin, newMax, delta));
        }
      } else {
        // everything is settable
        for (u_int32_t i=0; i<CRadix::k_MAX_CHILDREN; i++) {
          EXPECT_TRUE(nodePtr->canSetOffset(i, min, max, newMin, newMax, delta));
        }
      }
*/
    }
  }                                                                                                                   
}

TEST (cradix, multiInsertPermuations) {
  std::vector<unsigned> perm;
  for (unsigned i=0; i<NUM_REFERENCE_VALUES; ++i) {
    perm.push_back(i);
  }

  u_int32_t permCount(0);

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

    if ((++permCount%100000)==0) {
      printf("running permutation %u...\n", permCount);
    }
  } while(std::next_permutation(perm.begin(), perm.end()));
}
