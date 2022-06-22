#pragma once

#include <assert.h>
#include <iostream>
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

  explicit TestNode(htrie_byte maxKeySize, NodeType nodeType);

  // MANIUPULATORS
  htrie_byte encodeMaxSize(htrie_byte maxKeySize, htrie_byte highestNonZeroByte);

  htrie_byte encodeKeySize(htrie_byte byteSize, htrie_byte bitLen);
};

inline
TestNode::TestNode(htrie_byte maxKeySize, NodeType nodeType)
: d_maxLen(maxKeySize)
, d_nodeType(nodeType)
{
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
  htrie_byte    d_data[];

  // CREATORS
  TestNode1();
};

inline
TestNode1::TestNode1()
: TestNode(8, NodeType::NODE_1)
{
}

struct TestNode4: public TestNode {
  // DATA
  TestNode     *d_children[4];
  htrie_byte    d_data[];

  // CREATORS
  TestNode4();
};

inline
TestNode4::TestNode4()
: TestNode(8, NodeType::NODE_4)
{
}

struct TestNode16: public TestNode {
  // DATA
  TestNode     *d_children[16];
  htrie_byte    d_data[];

  TestNode16();
};

inline
TestNode16::TestNode16()
: TestNode(8, NodeType::NODE_16)
{
}

struct TestNode64: public TestNode {
  // DATA
  TestNode     *d_children[64];
  htrie_byte    d_data[];

  // CREATORS
  TestNode64();
};

inline
TestNode64::TestNode64()
: TestNode(8, NodeType::NODE_64)
{
}

} // namespace HTrie
} // namespace Benchmark
