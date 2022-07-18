#include <radix.h>
#include <radix_memmanager.h>
#include <radix_enums.h>

#include <stack>

namespace Radix {
  static Node256 *RadixLeafNode = reinterpret_cast<Node256*>(k_IS_LEAF_NODE);
  const u_int64_t RadixTermMask = Radix::k_IS_TERMINAL_NODE;
  const u_int64_t RadixNodeMask = 0xFFFFFFFFFFFFFF00UL;
}

int Radix::Tree::findHelper(const u_int8_t *key, const u_int16_t size) const {
  assert(key!=0);
  assert(size>0);

  Node256* node = const_cast<Node256*>(&d_root);

  union {
    Node256  *ptr;    // as pointer
    u_int64_t val;    // as u_int64_t
  } childNode;

  for (u_int16_t i=0; i<size; ++i) {
    childNode.ptr = node->d_children[key[i]];
    if (childNode.ptr>RadixLeafNode) {
      childNode.val &= RadixNodeMask;
      node = childNode.ptr; 
    } else if (childNode.ptr==0) {
      return e_NOT_FOUND;
    } else if (childNode.ptr==RadixLeafNode) {
      return ((i+1U)==size) ? e_EXISTS : e_NOT_FOUND;
    }
  }

  // Terminated on last byte in key
  printf("findHelper terminated\n");

  return (childNode.ptr==RadixLeafNode || (childNode.val & RadixTermMask))
    ? e_EXISTS
    : e_NOT_FOUND;
}

int Radix::Tree::insertHelper(const u_int8_t *key, const u_int16_t size,
  u_int16_t *lastMatchIndex, Node256 **lastMatch) {
  assert(key!=0);
  assert(size>0);
  assert(index!=0);
  assert(lastMatch!=0);
  assert(*lastMatch!=0);

  union {
    Node256  *ptr;    // as pointer
    u_int64_t val;    // as u_int64_t
  } node;

  node.ptr = &d_root;
  Node256 *pNode;     // parent of node

  for (u_int16_t i=0; i<size; ++i) {
    // Follow edge @ key[i] from node to child
    Node256 *childNode = node.ptr->d_children[key[i]];
    if (childNode>RadixLeafNode) {
      pNode = node.ptr;
      node.ptr = childNode;
      node.val &= RadixNodeMask;
    } else if (childNode==0) {
      *lastMatchIndex = i;
      *lastMatch = node.ptr;
      return e_NOT_FOUND;
    } else {
      assert(childNode==RadixLeafNode);
      *lastMatchIndex = i+1;
      if (*lastMatchIndex!=size) {
        // Last byte matched ends on leaf node. However the whole key
        // was not found so insert will have work to do. But in order
        // to do that, lastMatch has to be promoted to a Node256, and
        // the pointer to it needs to be updated. Leafs have no kids
        *lastMatch = d_memManager->mallocNode256();
        pNode->d_children[i-1] = *lastMatch;
        return e_NOT_FOUND;
      } else {
        *lastMatch = childNode;
        return e_EXISTS;
      }
    }
  }

  assert(pNode);

  // Edge from pNode to node refers to last byte in the key. So there's a
  // definite match. To get here pNode must be a inner node since all other
  // cases must have already returned from loop above. The remaining issue
  // is to ensure the link/pointer from pNode to node is marked terminal.
  // We'll reuse node union to so mark it

  node.ptr = pNode->d_children[size-1];
  node.val |= RadixTermMask;
  pNode->d_children[size-1] = node.ptr;

  return e_EXISTS;
}

int Radix::Tree::insert(const Benchmark::Slice<u_int8_t> key) {
  const u_int16_t size = key.size();
  const u_int8_t *keyPtr = key.data();

  // Search for key
  Node256 *lastMatch(0);
  u_int16_t lastMatchIndex(0);
  if (insertHelper(keyPtr, size, &lastMatchIndex, &lastMatch) == e_EXISTS) {
    return e_EXISTS;
  }

  // Not found so have a inner child node in tree where last match found
  assert(lastMatchIndex<size-1);
  assert(lastMatch!=0);
  assert(lastMatch!=RadixLeafNode);

  u_int8_t byte(keyPtr[lastMatchIndex]);

  if (lastMatchIndex+1==size) {
    // Insert last char & return OK
    lastMatch->d_children[byte] = RadixLeafNode;
  } else {
    assert(lastMatchIndex+1<size);
    while (lastMatchIndex < size-1) {
      lastMatch->d_children[byte] = d_memManager->mallocNode256();
      lastMatch = lastMatch->d_children[byte];
      byte = keyPtr[++lastMatchIndex];
    }
    // Insert last char & return OK
    assert(lastMatchIndex+1==size);
    lastMatch->d_children[byte] = RadixLeafNode;
  }

  return Radix::e_OK;
}

void Radix::Tree::statistics(TreeStats *stats) const {
  assert(stats);
  stats->reset();

  u_int16_t i(0);
  u_int16_t depth(0);
  std::stack<Radix::TreeIterState> stack;

  Radix::Node256 *node = const_cast<Radix::Node256*>(&d_root);

begin:
  for (; i<Radix::k_MAX_CHILDREN256; ++i) {
    if (node->d_children[i]==0) {
      ++stats->d_emptyChildCount;
      continue;
    }

    // We have a leaf or vanilla Node256. Either way depth increases
    if ((depth+1U)>stats->d_maxDepth) {
      stats->d_maxDepth = depth+1;
    }

    if (node->d_children[i]==RadixLeafNode) {
      ++stats->d_leafCount;
      continue;
    }

    // Recurse w/ stack
    ++stats->d_innerNodeCount;                                                                                           
    stack.push(Radix::TreeIterState(node, i, depth));
    node = node->d_children[i];
    i = 0xffff; // increments to back to 0
    ++depth;
  }

  if (!stack.empty()) {
    Radix::TreeIterState state = stack.top();
    node = state.d_node;
    i = state.d_index+1;
    depth = state.d_depth;
    stack.pop();
    goto begin;
  }
}

void Radix::Tree::dotGraph(std::ostream& stream) const {
  char buf[16];
  u_int16_t i(0);
  u_int16_t depth(0);
  std::stack<Radix::TreeIterState> stack;
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
    stack.push(Radix::TreeIterState(node, i, depth));
    node = node->d_children[i];
    i = 0xffff; // increments to back to 0
  }

  if (!stack.empty()) {
    Radix::TreeIterState state = stack.top();
    i = state.d_index+1;
    node = state.d_node;
    depth = state.d_depth;
    stack.pop();
    goto begin;
  }

  stream << "}" << std::endl;
}
