#include <benchmark_slice.h>
#include <patricia_tree.h>
#include <gtest/gtest.h>

extern Patricia::MemoryManager memManager;                                                                                     

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

TEST(slice, mkRoot) {
  Patricia::Tree *ptr = memManager.allocTree();
  EXPECT_TRUE(ptr!=0);
  EXPECT_TRUE(ptr->root==0);
  memManager.freeTree(ptr);
  memManager.print();
}

TEST(slice, addOneKey) {
  for (unsigned i=0; i<NUM_VALUES; ++i) {
    Benchmark::UKey key(VALUES[i].d_data, VALUES[i].d_size);
    Patricia::Tree *tree = memManager.allocTree();
    auto rc = Patricia::insertKey(tree, key);
    EXPECT_EQ(rc, Patricia::Errno::e_OK);
    rc = Patricia::findKey(tree, key);
    EXPECT_EQ(rc, Patricia::Errno::e_OK);
    std::vector<Benchmark::UKey> leaf;
    Patricia::allKeysSorted(tree, leaf);
    for (unsigned i=0; i<leaf.size(); ++i) {
      leaf[i].print();
    }
    memManager.freeTree(tree);
    memManager.print();
  }
}

TEST(slice, addMuliKey) {
  Patricia::Tree *tree = memManager.allocTree();
  for (unsigned i=0; i<NUM_VALUES; ++i) {
    Benchmark::UKey key(VALUES[i].d_data, VALUES[i].d_size);
    auto rc = Patricia::insertKey(tree, key);
    EXPECT_EQ(rc, Patricia::Errno::e_OK);
    if (rc!=Patricia::Errno::e_OK) {
      printf("ERROR: insert failed: ");
      key.print();
    } else {
      printf("OK: insert success: ");
      key.print();
    }
    rc = Patricia::findKey(tree, key);
    EXPECT_EQ(rc, Patricia::Errno::e_OK);
    if (rc!=Patricia::Errno::e_OK) {
      printf("ERROR: find failed: ");
      key.print();
    } else {
      printf("OK: find success: ");
      key.print();
    }
  }

  std::vector<Benchmark::UKey> leaf;
  Patricia::allKeysSorted(tree, leaf);
  for (unsigned i=0; i<leaf.size(); ++i) {
    leaf[i].print();
  }

  memManager.freeTree(tree);
  memManager.print();
}
