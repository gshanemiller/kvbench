#include <radix.h>
#include <radix_memmanager.h>
#include <radix_enums.h>

namespace Radix {
  static Node256 *RadixLeafNode = reinterpret_cast<Node256*>(k_IS_LEAF_NODE);
}

int Radix::Node256::insert(MemManager *memManager, const u_int8_t* key, u_int16_t index, u_int16_t size) {
  assert(memManager!=0);
  assert(key!=0);
  assert(index<size);
  assert(size>0);

  do {
    


int Radix::Tree::insert(const Benchmark::Slice<u_int8_t> key) {
  const u_int8_t  key0 = key.data()[0];
  const u_int16_t size = key.size();
  if (d_root[key0]>RadixLeafNode) {
    return d_root[key0]->insert(d_memManager, key.data(), 0, size);
  } else if (d_root[key0]==0) {
    if (size==1) {
      d_root[key0] = RadixLeafNode; 
      return Radix::e_OK;
    }
    d_root[key0] = d_memManager->mallocNode256();
    // Now insert rest of the key continuing from index 1
    assert(size>1);
    return d_root[key0]->insert(d_memManager, key.data(), 1, size);
  } else if (d_root[key0]==RadixLeafNode) {
    d_root[key0] = d_memManager->mallocNode256();
    // Now insert rest of the key continuing from index 1
    return d_root[key0]->insert(d_memManager, key.data(), 1, size);
  } else {
  }
}
