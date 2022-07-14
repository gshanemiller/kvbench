#include <radix.h>
#include <radix_memmanager.h>
#include <radix_enums.h>

namespace Radix {
  static Node256 *RadixLeafNode = reinterpret_cast<Node256*>(k_IS_LEAF_NODE);
}

int Radix::Tree::internalFind(const u_int8_t *key, const u_int16_t size,                                                           
    u_int16_t *lastMatchIndex, Node256 **lastMatch) const {
  assert(key!=0);
  assert(size>0);
  assert(index!=0);
  assert(lastMatch!=0);

  Node256 *node = &root;

  for (u_int16_t i=0; i<size; ++i) {
    const u_int8_t byte key[i];
    Radix::Node256 *childEdge = node->d_children[byte];
    if (childEdge>RadixLeafNode) {
      node = childEdge; 
    } else if (childEdge==0) {
      *lastMatchIndex = i;
      *lastMatch = node;
      return Radix::e_NOT_FOUND;
    } else if (node->d_children[byte]==RadixLeafNode) {
      *lastMatchIndex = i+1;
      *lastMatch = child;
      return (*lastMatchIndex==size) ? Radix::e_EXISTS : Radix::e_NOT_FOUND;
    }
  }

  // Control should never get here
  assert(0);
  return Radix::e_NOT_FOUND;
}

int Radix::Tree::insert(const Benchmark::Slice<u_int8_t> key) {
  Node256 *lastMatch(0);
  u_int16_t lastMatchIndex(0);
  const u_int8_t *key = key.data();
  const u_int16_t size = key.size();

  // Search for key
  if ((rc = Radix::Tree::internalFind(key, size, &lastMatchIndex, &lastMatch))
    == Radix::e_EXISTS) {
    return rc;
  }

  // Not found so we should have a child node in tree where last match found
  assert(lastMatchIndex<key.size());

  // Insert remainder of key starting at 'lastMatch' 
  for (; lastMatchIndex < size; ++lastMatchIndex) {
    u_int8_t byte key[lastMatchIndex];
    if (lastMatch>RadixLeafNode) {
      lastMatch = lastMatch->d_children[byte];
    } else if (lastMatch==0) {
      if (lastMatchIndex+1==size) {
        // Insert last char & return OK
        lastMatch->d_children[byte] = RadixLeafNode;
        return Radix::e_OK;
      } else {
        assert(lastMatchIndex+1<size);
        // It's a linked list of nodes from here-on-down
        while (lastMatchIndex < size-1) {
          lastMatch->d_children[byte] = d_memManager->mallocNode256();
          lastMatch = lastMatch->d_children[byte];
          byte = key[++lastMatchIndex];
        }
        // Insert last char & return OK
        assert(lastMatchIndex==size-1);
        lastMatch->d_children[byte] = RadixLeafNode;
        return Radix::e_OK;
      }
    } else {
      // lastMatch was a leaf node. Promote it to Node256 and insert rest
      // of characters which is a linked list of nodes from there on down
      assert(lastMatch==RadixLeafNode);
      lastMatch->d_children[byte] = d_memManager->mallocNode256();
      lastMatch = lastMatch->d_children[byte];
      for (++lastMatchIndex; lastMatchIndex < size-1) {
        byte = key[lastMatchIndex];
        lastMatch->d_children[byte] = d_memManager->mallocNode256();
        lastMatch = lastMatch->d_children[byte];
      }
      // Insert last char & return OK
      assert(lastMatchIndex==size-1);
      lastMatch->d_children[key[lastMatchIndex]] = RadixLeafNode;
      return Radix::e_OK;
    }
  }

  // Control should never get here
  assert(0);
  return Radix::e_NOT_FOUND;
}
