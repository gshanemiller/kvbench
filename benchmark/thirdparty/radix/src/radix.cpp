#include <radix.h>
#include <radix_memmanager.h>
#include <radix_enums.h>

#include <stack>

namespace Radix {
  static Node256 *RadixLeafNode = reinterpret_cast<Node256*>(k_IS_LEAF_NODE);
}

int Radix::Tree::internalFind(const u_int8_t *key, const u_int16_t size,                                                           
    u_int16_t *lastMatchIndex, Node256 **lastMatch) const {
  assert(key!=0);
  assert(size>0);
  assert(index!=0);
  assert(lastMatch!=0);

  Node256 *node = const_cast<Node256*>(&d_root);

  for (u_int16_t i=0; i<size; ++i) {
    const u_int8_t byte(key[i]);
    Radix::Node256 *childEdge = node->d_children[byte];
    if (childEdge>RadixLeafNode) {
      node = childEdge; 
    } else if (childEdge==0) {
      *lastMatchIndex = i;
      *lastMatch = node;
      return Radix::e_NOT_FOUND;
    } else if (node->d_children[byte]==RadixLeafNode) {
      *lastMatchIndex = i+1;
      *lastMatch = childEdge;
      return (*lastMatchIndex==size) ? Radix::e_EXISTS : Radix::e_NOT_FOUND;
    }
  }

  // Control should never get here
  assert(0);
  return Radix::e_NOT_FOUND;
}

int Radix::Tree::insert(const Benchmark::Slice<u_int8_t> key) {
  const u_int16_t size = key.size();
  const u_int8_t *keyPtr = key.data();

  // Search for key
  Node256 *lastMatch(0);
  u_int16_t lastMatchIndex(0);
  if (Radix::Tree::internalFind(keyPtr, size, &lastMatchIndex, &lastMatch) == Radix::e_EXISTS) {
    return Radix::e_EXISTS;
  }

  // Not found so we should have a child node in tree where last match found
  assert(lastMatchIndex<size);
  assert(lastMatch!=0);

  // Insert remainder of key starting in 'lastMatch' at 'lastMatchIndex'
  for (; lastMatchIndex < size; ++lastMatchIndex) {
    // Look one child node down from lastMatch location
    u_int8_t byte(keyPtr[lastMatchIndex]);
    Radix::Node256 *childEdge = lastMatch->d_children[byte];
    if (childEdge>RadixLeafNode) {
      lastMatch = childEdge;
    } else if (childEdge==0) {
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
          byte = keyPtr[++lastMatchIndex];
        }
        // Insert last char & return OK
        assert(lastMatchIndex+1==size);
        lastMatch->d_children[byte] = RadixLeafNode;
        return Radix::e_OK;
      }
    } else {
      // childEdge was a leaf node. Promote it to Node256 and insert rest
      // of characters which is a linked list of nodes from there on down
      assert(childEdge==RadixLeafNode);
      lastMatch->d_children[byte] = d_memManager->mallocNode256();
      lastMatch = lastMatch->d_children[byte];
      for (++lastMatchIndex; lastMatchIndex < size-1; ++lastMatchIndex) {
        byte = keyPtr[lastMatchIndex];
        lastMatch->d_children[byte] = d_memManager->mallocNode256();
        lastMatch = lastMatch->d_children[byte];
      }
      // Insert last char & return OK
      assert(lastMatchIndex==size-1);
      lastMatch->d_children[keyPtr[lastMatchIndex]] = RadixLeafNode;
      return Radix::e_OK;
    }
  }

  // Control should never get here
  assert(0);
  return Radix::e_NOT_FOUND;
}

void Radix::Tree::statistics(TreeStats *stats) const {
  assert(stats);
  stats->reset();

  unsigned i(0);
  std::stack<unsigned> indexStack;
  std::stack<Radix::Node256*> nodeStack;
  Radix::Node256 *node = const_cast<Radix::Node256*>(&d_root);

begin:
  for (; i<Radix::k_MAX_CHILDREN256; ++i) {
    if (node->d_children[i]==0) {
      ++stats->d_emptyChildCount;
      continue;
    }

    // We have a leaf or vanilla Node256. Either way depth increases
    if ((stats->d_maxDepth+1)>stats->d_maxDepth) {
      ++stats->d_maxDepth;
    }

    if (node->d_children[i]==RadixLeafNode) {
      ++stats->d_leafCount;
      continue;
    }

    // Recurse w/ stack
    ++stats->d_innerNodeCount;                                                                                           
    nodeStack.push(node);
    indexStack.push(i);
    assert(nodeStack.size()==indexStack.size());
    node = node->d_children[i];
    i = 0;
  }

  if (!nodeStack.empty()) {
    node = nodeStack.top();
    i = indexStack.top()+1;
    nodeStack.pop();
    indexStack.pop();
    assert(nodeStack.size()==indexStack.size());
    goto begin;
  }
}

void Radix::Tree::dotGraph(std::ostream& stream) const {
  char buf[16];
  unsigned i(0);
  std::stack<unsigned> indexStack;
  std::stack<Radix::Node256*> nodeStack;
  Radix::Node256 *node = const_cast<Radix::Node256*>(&d_root);

  stream << "digraph Radix {" << std::endl;
  stream << "  " << reinterpret_cast<u_int64_t>(node) << " [shape=diamond, label=\"root\"]" << std::endl;

begin:
  for (; i<Radix::k_MAX_CHILDREN256; ++i) {
    if (node->d_children[i]==0) {
      continue;
    }

    if (node->d_children[i]==RadixLeafNode) {
      // The leaf node
      if (isprint(i)) {
        buf[0] = char(i);
        buf[1] = 0;
      } else {
        sprintf(buf, "0x%02u", i);
      }
      stream << "  "
             << reinterpret_cast<u_int64_t>(node->d_children[i])
             << " [shape=box, "
             << "label=\""
             << buf
             << "\"]"
             << std::endl;
      // The edge from parent to leaf
      stream << "  "
             << reinterpret_cast<u_int64_t>(node)
             << " -> "
             << reinterpret_cast<u_int64_t>(node->d_children[i])
             << std::endl;
      continue;
    }

    // The inner node
    if (isprint(i)) {
      buf[0] = char(i);
      buf[1] = 0;
    } else {
      sprintf(buf, "0x%02u", i);
    }
    stream << "  "
           << reinterpret_cast<u_int64_t>(node->d_children[i])
             << " [shape=circle, "
             << "label=\""
             << buf
             << "\"]"
           << std::endl;
    // The edge from parent to inner node
    stream << "  "
           << reinterpret_cast<u_int64_t>(node)
           << " -> "
           << reinterpret_cast<u_int64_t>(node->d_children[i])
           << std::endl;

    // Recurse w/ stack
    nodeStack.push(node);
    indexStack.push(i);
    assert(nodeStack.size()==indexStack.size());
    node = node->d_children[i];
    i = 0;
  }

  if (!nodeStack.empty()) {
    node = nodeStack.top();
    i = indexStack.top()+1;
    nodeStack.pop();
    indexStack.pop();
    assert(nodeStack.size()==indexStack.size());
    goto begin;
  }

  stream << "}" << std::endl;
}
