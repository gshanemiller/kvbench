#include <benchmark_slice.h>
#include <radix.h>
#include <radix_memmanager.h>
#include <gtest/gtest.h>
#include <algorithm>

// TEST PLAN: A m-ary Radix tree does not depend on insertion order. Once all k keys are inserted the final shape 
// must be the same. This is important because it bounds worst case performance: there cannot be an insertion order
// that, for example, creates a 'tree' merely containing a linked list that would be a nice balanced tree in some
// other order. Thus the main aim of Radix testing is showing this property exists and works.
//
// The Radix Tree component has two important types:
//
// - Radix::Tree:    which holds the root node, a object of type Node256
// - Radix::Node256: an inner node of tree with up to 256 children.
//
// Canonical Radix Trees do not explicitly store keys or parts of keys. Key existence (absence) is implied by the
// the value of a child node. Ex. root->child['A'] -> 0 means there's no key starting with 'A'. Much of the testing
// therefore is caught up in updating these pointers.
//
// Child pointers start as,
//
// (*) zero
// (*) Radix::k_IS_LEAF_NODE (0xff) 
// (*) pointer to a Node256 object
//
// Pointers may be tagged:
//
// (*) Radix::k_IS_TERMINAL_NODE (0x02) ptr refers to a Node256 object whose edge ptr's parent to ptr
//     holds the terminal byte of a key. Ex. Inserting 'Program', 'Pro' needs the edge for 'o' marked
//     terminal so a search for 'Pro' succeeds.
// (*) Radix::k_IS_CHILDREN_COMPRESSED (0x01) ptr refers to a Node256 object which is compressed
//
// Unlike ART Radix objects don't change types through over (under) flow at runtime. Pointers, however, do change:
// In these life-cycle charts think of a Node256 object (root or inner node) and consider the ptr value in it from
// the ith child:
//                            parent
//                              |
//                     Node256  V
//                  +------------+
//                  |  children  |   Node 'N' 0x00561220...
//                  | 0 1 2 .. n |
//                  +-+-+-+----+-+
//                    | | |    |
//                    V V V    V
//                    ? ? ?    ?
// Pointer Life Cycle
// +-----------+-------------------------------+------------------------------------------------------------------+-------------+
// | State/Val | Description                   | Next State(s)                                                    | Test Case#  |
// +-----------+-------------------------------+------------------------------------------------------------------+-------------+
// | 0         | All Node256 children dflt 0   | 0    -> unchanged                                                |             |
// |           |                               | 0xff -> (Radix::k_IS_LEAF_NODE) add leaf under 'N'               | 0           |
// |           |                               | m    -> add Node256 inner node 'm' under 'N'                     |             |
// +-----------+-------------------------------+------------------------------------------------------------------+-------------+
// | 0xff      | Radix Leaf node. This special | 0xff -> unchanged                                                |             |
// |           | value reserved for pointer to | m    -> a new key is being added in which the edge from N to     | 1           |
// |           | leaf nodes                    |         N.child[i] is a prefix of a new, longer key. Therefore   |             |
// |           |                               |         N.child[i] must now point to a newly allocated Node256   |             |
// +-----------+-------------------------------+------------------------------------------------------------------+-------------+
// | m         | A Radix inner node            | m    -> unchanged                                                |             |
// |           |                               | m+1  -> m was compressed                                         |             |
// |           |                               | m+2  -> m became a terminal node                                 |             |
// |           |                               | m+3  -> m became terminal node and compressed                    |             |
// +-----------+-------------------------------+------------------------------------------------------------------+-------------+
// | m + 1     | A Radix inner node that's     | m+1  -> unchanged                                                |             |
// |           | been compressed               | m+3  -> node became terminal node for shorter key                |             |
// +-----------+-------------------------------+------------------------------------------------------------------+-------------+
// | m + 2     | A Radix inner node that's     | m+2  -> unchanged                                                |             |
// |           | also the terminal byte of a   | m+3  -> node compression occurred                                |             |
// |           | shorter key                   |                                                                  |             |
// +-----------+-------------------------------+------------------------------------------------------------------+-------------+
// | m + 3     | A Radix inner node that's     | m+3  -> unchanged                                                |             |
// |           | also a temrinal byte of a     |                                                                  |             |
// |           | shorter key and compressed    |                                                                  |             |
// +-----------+-------------------------------+------------------------------------------------------------------+-------------+
// 
// Note: m+1 <-> m | Radix::k_IS_CHILDREN_COMPRESSED
// Note: m+2 <-> m | Radix::k_IS_TERMINAL_NODE
// Note: m+3 <-> m | Radix::k_IS_CHILDREN_COMPRESSED | Radix::k_IS_TERMINAL_NODE
//

static const struct {
  int           d_lineNum;  // source line number
  u_int16_t     d_size;     // size of string in bytes
  u_int8_t      d_data[16]; // up to 16 bytes of data
} REFERENCE_VALUES[] = {
  //line      size data
  //----      ---- ----------------------------
  { __LINE__,   1,  {0}                                                 },
  { __LINE__,   2,  {0, 0}                                              },
  { __LINE__,   1,  {'A'}                                               },
  { __LINE__,   2,  {'A', 0}                                            },
  { __LINE__,   3,  {'P', 'r', 'o'}                                     },
  { __LINE__,   4,  {'P', 'r', 'o', 0}                                  },
  { __LINE__,   7,  {'P', 'r', 'o', 'j', 'e', 'c', 't'}                 },
  { __LINE__,   8,  {'P', 'r', 'o', 'j', 'e', 'c', 't', '0'}            },
};

const std::size_t NUM_REFERENCE_VALUES = sizeof REFERENCE_VALUES / sizeof *REFERENCE_VALUES;

TEST(radix, case0_0) {
  u_int8_t byte; 
  Radix::TreeStats stats;

  for (unsigned i=0; i<256; ++i) {
    byte = i;
    Benchmark::Slice<unsigned char> key(&byte, 1);

    Radix::MemManager mem;
    Radix::Tree tree(&mem);

    int rc = tree.find(key);
    EXPECT_TRUE(rc==Radix::e_NOT_FOUND);
    
    rc = tree.insert(key);
    EXPECT_TRUE(rc==Radix::e_OK);
    
    rc = tree.find(key);
    EXPECT_TRUE(rc==Radix::e_EXISTS);

    tree.statistics(&stats);
    EXPECT_EQ(stats.d_innerNodeCount, 0);
    EXPECT_EQ(stats.d_leafCount, 1);
    EXPECT_EQ(stats.d_emptyChildCount, 255);
    EXPECT_EQ(stats.d_maxDepth, 1);

    Radix::MemManagerStats mstats;
    mem.statistics(&mstats);
    EXPECT_EQ(mstats.d_allocCount, 0);
    EXPECT_EQ(mstats.d_freeCount, 0);
    EXPECT_EQ(mstats.d_currentSizeBytes, 0);
    EXPECT_EQ(mstats.d_maximumSizeBytes, 0);
    EXPECT_EQ(mstats.d_requestedBytes, 0);
    EXPECT_EQ(mstats.d_freedBytes, 0);
  }
}

TEST(radix, case0_1) {
  u_int8_t byte; 
  Radix::MemManager mem;
  Radix::Tree tree(&mem);
  Radix::TreeStats stats;

  for (unsigned i=0; i<256; ++i) {
    byte = i;
    Benchmark::Slice<unsigned char> key(&byte, 1);

    int rc = tree.find(key);
    EXPECT_TRUE(rc==Radix::e_NOT_FOUND);
    
    rc = tree.insert(key);
    EXPECT_TRUE(rc==Radix::e_OK);
    
    rc = tree.find(key);
    EXPECT_TRUE(rc==Radix::e_EXISTS);

    // All previous keys should still be there
    for (unsigned j=0; j<i; j++) {
      byte = j;
      Benchmark::Slice<unsigned char> skey(&byte, 1);
      rc = tree.find(skey);
      EXPECT_TRUE(rc==Radix::e_NOT_FOUND);
    }
  }

  tree.statistics(&stats);
  EXPECT_EQ(stats.d_innerNodeCount, 0);
  EXPECT_EQ(stats.d_leafCount, 256);
  EXPECT_EQ(stats.d_emptyChildCount, 0);
  EXPECT_EQ(stats.d_maxDepth, 1);

  Radix::MemManagerStats mstats;
  mem.statistics(&mstats);
  EXPECT_EQ(mstats.d_allocCount, 0);
  EXPECT_EQ(mstats.d_freeCount, 0);
  EXPECT_EQ(mstats.d_currentSizeBytes, 0);
  EXPECT_EQ(mstats.d_maximumSizeBytes, 0);
  EXPECT_EQ(mstats.d_requestedBytes, 0);
  EXPECT_EQ(mstats.d_freedBytes, 0);
}
