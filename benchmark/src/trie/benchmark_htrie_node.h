#pragma once

#include <assert.h>
#include <iostream>
#include <mimalloc.h>                                                                                                   
#include <trie/benchmark_htrie_bitstring.h>

namespace Benchmark {
namespace HTrie {

enum NodeType {
  NODE_1,
  NODE_4,
  NODE_16,
  NODE_64,
};

struct TestNode {
  htrie_byte  d_maxLen;
  htrie_byte  d_nodeType;

  // CREATORS
  TestNode() = delete;
  ~TestNode() = default;
  TestNode(const TestNode&) = delete;
  TestNode& operator=(const TestNode&) = delete;
  explicit TestNode(htrie_byte maxKeySize, NodeType nodeType);

  // ACCESSORS
  htrie_byte maxKeySize() const;

  // MANIUPULATORS
  htrie_byte encodeMaxSize(htrie_byte maxKeySize, htrie_byte highestNonZeroByte);
  htrie_byte encodeKeySize(htrie_byte byteSize, htrie_byte bitLen);
};

inline
TestNode::TestNode(htrie_byte maxKeySize, NodeType nodeType)
: d_maxLen(maxKeySize<<8)
, d_nodeType(nodeType)
{
  assert(maxKeySize>=1&&maxKeySize<=15);
}

htrie_byte TestNode::maxKeySize() const {
  return (d_maxLen>>8);
}

inline
htrie_byte TestNode::encodeMaxSize(htrie_byte maxKeySize, htrie_byte highestNonZeroByte) {
  assert(maxKeySize>=1&&maxKeySize<=15);
  assert(highestNonZeroByte>=0&&highestNonZeroByte<maxKeySize);
  return (maxKeySize << 8 | highestNonZeroByte);
}

inline
htrie_byte TestNode::encodeKeySize(htrie_byte byteSize, htrie_byte bitLen) {
  assert(byteSize<=8);
  assert(bitLen<=64);
  return (byteSize<<8 | bitLen);
}

struct TestNode1: public TestNode {
  // DATA
  TestNode     *d_gtChild;
  htrie_byte   *d_data;

  // CREATORS
  TestNode1() = delete;
  TestNode1(const TestNode1&) = delete;
  TestNode1& operator=(const TestNode1&) = delete;
  explicit TestNode1(htrie_byte maxKeySize);
};

inline
TestNode1::TestNode1(htrie_byte maxKeySize)
: TestNode(maxKeySize, NodeType::NODE_1)
{
}

struct TestNode4: public TestNode {
  // DATA
  TestNode     *d_children[4];
  htrie_byte   *d_data;

  // CREATORS
  TestNode4() = delete;
  TestNode4(const TestNode4&) = delete;
  TestNode4& operator=(const TestNode4&) = delete;
  explicit TestNode4(htrie_byte maxKeySize);
};

inline
TestNode4::TestNode4(htrie_byte maxKeySize)
: TestNode(maxKeySize, NodeType::NODE_4)
{
}

struct TestNode16: public TestNode {
  // DATA
  TestNode     *d_children[16];
  htrie_byte   *d_data;

  TestNode16() = delete;
  TestNode16(const TestNode16&) = delete;
  TestNode16& operator=(const TestNode16&) = delete;
  explicit TestNode16(htrie_byte maxKeySize);
};

inline
TestNode16::TestNode16(htrie_byte maxKeySize)
: TestNode(maxKeySize, NodeType::NODE_16)
{
}

struct TestNode64: public TestNode {
  // DATA
  TestNode     *d_children[64];
  htrie_byte   *d_data;

  // CREATORS
  TestNode64() = delete;
  TestNode64(const TestNode64&) = delete;
  TestNode64& operator=(const TestNode64&) = delete;
  explicit TestNode64(htrie_byte maxKeySize);
};

inline
TestNode64::TestNode64(htrie_byte maxKeySize)
: TestNode(maxKeySize, NodeType::NODE_64)
{
}

struct MemoryManager {
  // ENUM {
  enum {
    k_MAX_NODE_HEAPS = 4,
    k_MAX_KEYSIZE_HEAPS = 8,
  };

  // DATA
  mi_heap_t *d_nodeHeap[k_MAX_NODE_HEAPS];
  mi_heap_t *d_partialKeyHeap[k_MAX_KEYSIZE_HEAPS];
  
  MemoryManager();
  ~MemoryManager();
  MemoryManager(const MemoryManager&) = delete;
  MemoryManager& operator=(const MemoryManager&) = delete;

  TestNode1 *makeTestNode1(htrie_byte maxKeyLen);
  TestNode4 *makeTestNode4(htrie_byte maxKeyLen);
  TestNode16 *makeTestNode16(htrie_byte maxKeyLen);
  TestNode64 *makeTestNode64(htrie_byte maxKeyLen);
};

inline
MemoryManager::MemoryManager() {
  for (unsigned i=0; i<k_MAX_NODE_HEAPS; ++i) {
    d_nodeHeap[i] = mi_heap_new();
  }

  for (unsigned i=0; i<k_MAX_KEYSIZE_HEAPS; ++i) {
    d_partialKeyHeap[i] = mi_heap_new();
  }
}

inline
MemoryManager::~MemoryManager() {
  for (unsigned i=0; i<k_MAX_NODE_HEAPS; ++i) {
    mi_heap_destroy(d_nodeHeap[i]);
  }

  for (unsigned i=0; i<k_MAX_KEYSIZE_HEAPS; ++i) {
    mi_heap_destroy(d_partialKeyHeap[i]);
  }
}

inline
TestNode1 *MemoryManager::makeTestNode1(htrie_byte maxKeyLen) {
  assert(maxKeyLen>0&&maxKeyLen<=8);
  TestNode1 *ptr = new(mi_heap_malloc_aligned(d_nodeHeap[0], sizeof(TestNode1), 4)) TestNode1(maxKeyLen);
  auto data = mi_heap_malloc_aligned(d_partialKeyHeap[maxKeyLen-1], maxKeyLen, 2);
  ptr->d_data = reinterpret_cast<htrie_byte*>(data);
  return ptr;
}

TestNode4 *MemoryManager::makeTestNode4(htrie_byte maxKeyLen) {
  assert(maxKeyLen>0&&maxKeyLen<=8);
  TestNode4 *ptr = new(mi_heap_malloc_aligned(d_nodeHeap[1], sizeof(TestNode4), 4)) TestNode4(maxKeyLen);
  auto data = (char*)mi_heap_malloc_aligned(d_partialKeyHeap[maxKeyLen-1], maxKeyLen<<2, 2);
  ptr->d_data = reinterpret_cast<htrie_byte*>(data);
  return ptr;
}

TestNode16 *MemoryManager::makeTestNode16(htrie_byte maxKeyLen) {
  assert(maxKeyLen>0&&maxKeyLen<=8);
  TestNode16 *ptr = new(mi_heap_malloc_aligned(d_nodeHeap[2], sizeof(TestNode16), 4)) TestNode16(maxKeyLen);
  auto data = (char*)mi_heap_malloc_aligned(d_partialKeyHeap[maxKeyLen-1], maxKeyLen<<4, 2);
  ptr->d_data = reinterpret_cast<htrie_byte*>(data);
  return ptr;
}

TestNode64 *MemoryManager::makeTestNode64(htrie_byte maxKeyLen) {
  assert(maxKeyLen>0&&maxKeyLen<=8);
  TestNode64 *ptr = new(mi_heap_malloc_aligned(d_nodeHeap[3], sizeof(TestNode64), 4)) TestNode64(maxKeyLen);
  auto data = (char*)mi_heap_malloc_aligned(d_partialKeyHeap[maxKeyLen-1], maxKeyLen<<6, 2);
  ptr->d_data = reinterpret_cast<htrie_byte*>(data);
  return ptr;
}

} // namespace HTrie
} // namespace Benchmark
