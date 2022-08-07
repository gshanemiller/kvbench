#include <benchmark_slice.h>
#include <cradix_tree.h>
#include <cradix_memmanager.h>
// #include <gtest/gtest.h>
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

#define EXPECT_EQ(x,y) assert((x)==(y))

// Case 0a one leaf at a time
//
// Before            After
// +--------+       +--------+
// |  root  |  |->  |  root  | -> 0xff
// +--------+       +--------+
//
// Test: On empty tree, add a key of size 1
//
void test_cradix_case0a() {
  u_int8_t byte; 
  CRadix::TreeStats stats;

  for (unsigned i=0; i<CRadix::k_MAX_CHILDREN; ++i) {
    byte = i;
    Benchmark::Slice<unsigned char> key(&byte, 1);

    CRadix::MemManager mem(0x100000, 4);;
    CRadix::Tree tree(&mem);

    int rc = tree.find(key);
    assert(rc==CRadix::e_NOT_FOUND);
    
    rc = tree.insert(key);
    assert(rc==CRadix::e_OK);
    
    rc = tree.find(key);
    assert(rc==CRadix::e_EXISTS);

    tree.statistics(&stats);
    stats.print(std::cout);
/*
    EXPECT_EQ(stats.d_innerNodeCount, 0);
    EXPECT_EQ(stats.d_leafCount, 1);
    EXPECT_EQ(stats.d_emptyChildCount, 255);
    EXPECT_EQ(stats.d_maxDepth, 1);
    EXPECT_EQ(stats.d_maxDepth, tree.currentMaxDepth());
*/

    CRadix::MemStats mstats;
    mem.statistics(&mstats);
    mstats.print(std::cout);
/*
    EXPECT_EQ(mstats.d_allocCount, 0);
    EXPECT_EQ(mstats.d_freeCount, 0);
    EXPECT_EQ(mstats.d_currentSizeBytes, 0);
    EXPECT_EQ(mstats.d_maximumSizeBytes, 0);
    EXPECT_EQ(mstats.d_requestedBytes, 0);
    EXPECT_EQ(mstats.d_freedBytes, 0);
*/

    CRadix::Iterator iter = tree.begin();
    while (!iter.end()) {
      iter.print(std::cout);
      iter.next();
    }
  }
}

// Case 0b all leafs added to one tree
void test_cradix_case0b() {
  u_int8_t byte; 
  CRadix::MemManager mem(0x100000, 4);
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
  stats.print(std::cout); 
/*
  EXPECT_EQ(stats.d_innerNodeCount, 0);
  EXPECT_EQ(stats.d_leafCount, CRadix::k_MAX_CHILDREN);
  EXPECT_EQ(stats.d_emptyChildCount, 0);
  EXPECT_EQ(stats.d_maxDepth, 1);
  EXPECT_EQ(stats.d_maxDepth, tree.currentMaxDepth());
*/

  CRadix::MemStats mstats;
  mem.statistics(&mstats);
  mstats.print(std::cout); 
/*
  EXPECT_EQ(mstats.d_allocCount, 0);
  EXPECT_EQ(mstats.d_freeCount, 0);
  EXPECT_EQ(mstats.d_currentSizeBytes, 0);
  EXPECT_EQ(mstats.d_maximumSizeBytes, 0);
  EXPECT_EQ(mstats.d_requestedBytes, 0);
  EXPECT_EQ(mstats.d_freedBytes, 0);
*/

  CRadix::Iterator iter = tree.begin();
  while (!iter.end()) {
    iter.print(std::cout);
    iter.next();
  }

  tree.destroy();
  mem.statistics(&mstats);
  mstats.print(std::cout); 
/*
  EXPECT_EQ(mstats.d_allocCount, 0);
  EXPECT_EQ(mstats.d_freeCount, 0);
  EXPECT_EQ(mstats.d_currentSizeBytes, 0);
  EXPECT_EQ(mstats.d_maximumSizeBytes, 0);
  EXPECT_EQ(mstats.d_requestedBytes, 0);
  EXPECT_EQ(mstats.d_freedBytes, 0);
*/
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
void test_cradix_case1a() {
  u_int8_t byte[2];
  CRadix::TreeStats stats;

  for (unsigned i=0; i<CRadix::k_MAX_CHILDREN; ++i) {
    byte[0] = i;
    Benchmark::Slice<unsigned char> key(byte+0, 1);

    CRadix::MemManager mem(0x100000, 4);;
    CRadix::Tree tree(&mem);

    int rc = tree.find(key);
    assert(rc==CRadix::e_NOT_FOUND);
    
    rc = tree.insert(key);
    assert(rc==CRadix::e_OK);

  {
    CRadix::Iterator iter = tree.begin();
    while (!iter.end()) {
      iter.print(std::cout);
      iter.next();
    }
  }

    rc = tree.find(key);
    assert(rc==CRadix::e_EXISTS);

    // Now add key {<i>,'T'} so that <i> is a prefix of {<i>,'T'} 
    byte[1] = 'T';
    Benchmark::Slice<unsigned char> key1(byte+0, 2);

    rc = tree.find(key1);
    assert(rc==CRadix::e_NOT_FOUND);
    
    rc = tree.insert(key1);
    assert(rc==CRadix::e_OK);

  {
    CRadix::Iterator iter = tree.begin();
    while (!iter.end()) {
      iter.print(std::cout);
      iter.next();
    }
  }
    
    rc = tree.find(key1);
    assert(rc==CRadix::e_EXISTS);

    tree.statistics(&stats);
    stats.print(std::cout); 
/*
    EXPECT_EQ(stats.d_innerNodeCount, 1);
    EXPECT_EQ(stats.d_leafCount, 2);
    // root + 1 inner node each of which has 255 empty slots, 1 used slot
    EXPECT_EQ(stats.d_emptyChildCount, 2*CRadix::k_MAX_CHILDREN-2);
    EXPECT_EQ(stats.d_maxDepth, 2);
    EXPECT_EQ(stats.d_maxDepth, tree.currentMaxDepth());
*/

    CRadix::MemStats mstats;
    mem.statistics(&mstats);
    mstats.print(std::cout); 
/*
    EXPECT_EQ(mstats.d_allocCount, 1);
    EXPECT_EQ(mstats.d_freeCount, 0);
    EXPECT_EQ(mstats.d_currentSizeBytes, mem.sizeOfUncompressedNode256());
    EXPECT_EQ(mstats.d_maximumSizeBytes, mem.sizeOfUncompressedNode256());
    EXPECT_EQ(mstats.d_requestedBytes, mem.sizeOfUncompressedNode256());
    EXPECT_EQ(mstats.d_freedBytes, 0);
*/

    CRadix::Iterator iter = tree.begin();
    while (!iter.end()) {
      iter.print(std::cout);
      iter.next();
    }

    tree.destroy();
    mem.statistics(&mstats);
    mstats.print(std::cout); 
/*
    EXPECT_EQ(mstats.d_allocCount, 1);
    EXPECT_EQ(mstats.d_freeCount, 1);
    EXPECT_EQ(mstats.d_currentSizeBytes, 0);
    EXPECT_EQ(mstats.d_maximumSizeBytes, mem.sizeOfUncompressedNode256());
    EXPECT_EQ(mstats.d_requestedBytes, mem.sizeOfUncompressedNode256());
    EXPECT_EQ(mstats.d_freedBytes, mem.sizeOfUncompressedNode256());
*/
  }
}

// Redo case 1a except all work done to same tree
void test_cradix_case1b() {
  CRadix::MemManager mem(0x100000, 4);;
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
  stats.print(std::cout); 
  tree.statistics(&stats);
/*
  EXPECT_EQ(stats.d_innerNodeCount, 256);
  EXPECT_EQ(stats.d_leafCount, 512);
  // We have 1 root but all its children are full. Each child of root is an inner node
  // with 256 children each and, of those, 255 empty 1 full.
  EXPECT_EQ(stats.d_emptyChildCount, CRadix::k_MAX_CHILDREN*CRadix::k_MAX_CHILDREN-CRadix::k_MAX_CHILDREN);
  EXPECT_EQ(stats.d_maxDepth, 2);
  EXPECT_EQ(stats.d_maxDepth, tree.currentMaxDepth());
*/

  CRadix::MemStats mstats;
  mem.statistics(&mstats);
  mstats.print(std::cout); 
/*
  EXPECT_EQ(mstats.d_allocCount, 256);
  EXPECT_EQ(mstats.d_freeCount, 0);
  EXPECT_EQ(mstats.d_currentSizeBytes, 256*mem.sizeOfUncompressedNode256());
  EXPECT_EQ(mstats.d_maximumSizeBytes, 256*mem.sizeOfUncompressedNode256());
  EXPECT_EQ(mstats.d_requestedBytes, 256*mem.sizeOfUncompressedNode256());
  EXPECT_EQ(mstats.d_freedBytes, 0);
*/

  CRadix::Iterator iter = tree.begin();
  while (!iter.end()) {
    iter.print(std::cout);
    iter.next();
  }

  tree.destroy();
  mem.statistics(&mstats);
  mstats.print(std::cout); 
/*
  EXPECT_EQ(mstats.d_allocCount, 256);
  EXPECT_EQ(mstats.d_freeCount, 256);
  EXPECT_EQ(mstats.d_currentSizeBytes, 0);
  EXPECT_EQ(mstats.d_maximumSizeBytes, 256*mem.sizeOfUncompressedNode256());
  EXPECT_EQ(mstats.d_requestedBytes, 256*mem.sizeOfUncompressedNode256());
  EXPECT_EQ(mstats.d_freedBytes, 256*mem.sizeOfUncompressedNode256());
*/
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
void test_cradix_case2a() {
  u_int8_t byte[2];
  CRadix::TreeStats stats;

  for (unsigned i=0; i<CRadix::k_MAX_CHILDREN; ++i) {
    byte[0] = i;
    byte[1] = 'o';
    Benchmark::Slice<unsigned char> key(byte+0, 2);

    CRadix::MemManager mem(0x100000, 4);;
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
    stats.print(std::cout); 
/*
    EXPECT_EQ(stats.d_innerNodeCount, 1);
    EXPECT_EQ(stats.d_leafCount, 2);
    // root + 1 inner node each of which has 255 empty slots, 1 used slot
    EXPECT_EQ(stats.d_emptyChildCount, 2*CRadix::k_MAX_CHILDREN-2);
    EXPECT_EQ(stats.d_maxDepth, 2);
    EXPECT_EQ(stats.d_maxDepth, tree.currentMaxDepth());
*/

    CRadix::MemStats mstats;
    mem.statistics(&mstats);
    mstats.print(std::cout); 
/*
    EXPECT_EQ(mstats.d_allocCount, 1);
    EXPECT_EQ(mstats.d_freeCount, 0);
    EXPECT_EQ(mstats.d_currentSizeBytes, mem.sizeOfUncompressedNode256());
    EXPECT_EQ(mstats.d_maximumSizeBytes, mem.sizeOfUncompressedNode256());
    EXPECT_EQ(mstats.d_requestedBytes, mem.sizeOfUncompressedNode256());
    EXPECT_EQ(mstats.d_freedBytes, 0);
*/

    CRadix::Iterator iter = tree.begin();
    while (!iter.end()) {
      iter.print(std::cout);
      iter.next();
    }

    tree.destroy();
    mem.statistics(&mstats);
    mstats.print(std::cout); 
/*
    EXPECT_EQ(mstats.d_allocCount, 1);
    EXPECT_EQ(mstats.d_freeCount, 1);
    EXPECT_EQ(mstats.d_currentSizeBytes, 0);
    EXPECT_EQ(mstats.d_maximumSizeBytes, mem.sizeOfUncompressedNode256());
    EXPECT_EQ(mstats.d_requestedBytes, mem.sizeOfUncompressedNode256());
    EXPECT_EQ(mstats.d_freedBytes, mem.sizeOfUncompressedNode256());
*/
  }
}

// Redo case 2a except all work done to same tree
void test_cradix_case2b() {
  CRadix::MemManager mem(0x100000, 4);;
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
  stats.print(std::cout); 
/*
  EXPECT_EQ(stats.d_innerNodeCount, 256);
  EXPECT_EQ(stats.d_leafCount, 512);
  // We have 1 root but all its children are full. Each child of root is an inner node
  // with 256 children each and, of those, 255 empty 1 full.
  EXPECT_EQ(stats.d_emptyChildCount, CRadix::k_MAX_CHILDREN*CRadix::k_MAX_CHILDREN-CRadix::k_MAX_CHILDREN);
  EXPECT_EQ(stats.d_maxDepth, 2);
  EXPECT_EQ(stats.d_maxDepth, tree.currentMaxDepth());
*/

  CRadix::MemStats mstats;
  mem.statistics(&mstats);
  mstats.print(std::cout); 
/*
  EXPECT_EQ(mstats.d_allocCount, 256);
  EXPECT_EQ(mstats.d_freeCount, 0);
  EXPECT_EQ(mstats.d_currentSizeBytes, 256*mem.sizeOfUncompressedNode256());
  EXPECT_EQ(mstats.d_maximumSizeBytes, 256*mem.sizeOfUncompressedNode256());
  EXPECT_EQ(mstats.d_requestedBytes, 256*mem.sizeOfUncompressedNode256());
  EXPECT_EQ(mstats.d_freedBytes, 0);
*/

  CRadix::Iterator iter = tree.begin();
  while (!iter.end()) {
    iter.print(std::cout);
    iter.next();
  }

  tree.destroy();
  mem.statistics(&mstats);
  mstats.print(std::cout); 
/*
  EXPECT_EQ(mstats.d_allocCount, 256);
  EXPECT_EQ(mstats.d_freeCount, 256);
  EXPECT_EQ(mstats.d_currentSizeBytes, 0);
  EXPECT_EQ(mstats.d_maximumSizeBytes, 256*mem.sizeOfUncompressedNode256());
  EXPECT_EQ(mstats.d_requestedBytes, 256*mem.sizeOfUncompressedNode256());
  EXPECT_EQ(mstats.d_freedBytes, 256*mem.sizeOfUncompressedNode256());
*/
}

void test_cradix_multiInsertPermuations() {
  std::vector<unsigned> perm;
  for (unsigned i=0; i<NUM_REFERENCE_VALUES; ++i) {
    perm.push_back(i);
  }

  u_int32_t permutation(0);

  do {
    CRadix::MemManager mem(0x100000, 4);;
    CRadix::Tree tree(&mem);

    printf("permuatation %u\n", permutation);

    for (unsigned i=0; i<perm.size(); ++i) {
      Benchmark::Slice<unsigned char> key(REFERENCE_VALUES[perm[i]].d_data, REFERENCE_VALUES[perm[i]].d_size);

      int rc = tree.find(key);
      assert(rc==CRadix::e_NOT_FOUND);
    
      printf("inserting key %u\n", perm[i]);

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

      CRadix::Iterator iter = tree.begin();
      while (!iter.end()) {
        iter.print(std::cout);
        iter.next();
      }
    }

    CRadix::TreeStats stats;
    tree.statistics(&stats);
    stats.print(std::cout);
/*
    EXPECT_EQ(stats.d_innerNodeCount, 5);
    EXPECT_EQ(stats.d_leafCount, 10); // should be 7?
    u_int64_t expectedEmptyChildCount =
        (CRadix::k_MAX_CHILDREN - 3)    // root
      + (CRadix::k_MAX_CHILDREN - 1)    // 'P'
      + (CRadix::k_MAX_CHILDREN - 1)    // 'i'
      + (CRadix::k_MAX_CHILDREN - 1)    // 'Z'
      + (CRadix::k_MAX_CHILDREN - 3)    // 'A'
      + (CRadix::k_MAX_CHILDREN - 3);   // 'o'
    EXPECT_EQ(stats.d_emptyChildCount, expectedEmptyChildCount);
    EXPECT_EQ(stats.d_maxDepth, 4);
    EXPECT_EQ(stats.d_maxDepth, tree.currentMaxDepth());
*/

    CRadix::MemStats mstats;
    mem.statistics(&mstats);
    mstats.print(std::cout);
/*
    EXPECT_EQ(mstats.d_allocCount, 5);
    EXPECT_EQ(mstats.d_freeCount, 0);
    EXPECT_EQ(mstats.d_currentSizeBytes, 5*mem.sizeOfUncompressedNode256());
    EXPECT_EQ(mstats.d_maximumSizeBytes, 5*mem.sizeOfUncompressedNode256());
    EXPECT_EQ(mstats.d_requestedBytes, 5*mem.sizeOfUncompressedNode256());
    EXPECT_EQ(mstats.d_freedBytes, 0);
*/

    tree.destroy();
    mem.statistics(&mstats);
    mstats.print(std::cout);
/*
    EXPECT_EQ(mstats.d_allocCount, 5);
    EXPECT_EQ(mstats.d_freeCount, 5);
    EXPECT_EQ(mstats.d_currentSizeBytes, 0);
    EXPECT_EQ(mstats.d_maximumSizeBytes, 5*mem.sizeOfUncompressedNode256());
    EXPECT_EQ(mstats.d_requestedBytes, 5*mem.sizeOfUncompressedNode256());
    EXPECT_EQ(mstats.d_freedBytes, 5*mem.sizeOfUncompressedNode256());
*/
    ++permutation;
  } while(std::next_permutation(perm.begin(), perm.end()));
}

int main(int argc, char **argv) {
  printf("=======case0a============================\n");
  test_cradix_case0a();
  printf("=======case0b============================\n\n");
  test_cradix_case0b();
  printf("=======case1a============================\n\n");
  test_cradix_case1a();
  printf("=======case1b============================\n\n");
  test_cradix_case1b();
  printf("=======case2a============================\n\n");
  test_cradix_case2a();
  printf("=======case2b============================\n\n");
  test_cradix_case2b();
  printf("=========================================\n\n");
  test_cradix_multiInsertPermuations();
  return 0;
}
