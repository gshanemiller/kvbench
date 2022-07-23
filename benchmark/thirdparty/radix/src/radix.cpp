#include <radix.h>
#include <radix_memmanager.h>
#include <radix_enums.h>

namespace Radix {
  static Node256 *RadixLeafNode = reinterpret_cast<Node256*>(k_IS_LEAF_NODE);
  const u_int64_t RadixTermMask = Radix::k_IS_TERMINAL_NODE;
  const u_int64_t RadixTagClear = 0xFFFFFFFFFFFFFFFDUL;
}

Radix::TreeIterator::TreeIterator(Radix::MemManager *memManager, Radix::Node256& root, u_int64_t maxDepth)                                   
: d_memManager(memManager)                                                                                              
, d_root(root)                                                                                                          
, d_rawNode(&d_root)                                                                                                    
, d_key(0)                                                                                                              
, d_attributes(0)                                                                                                       
, d_index(0)                                                                                                            
, d_keySize(0)                                                                                                          
, d_end(false)                                                                                                          
{                                                                                                                       
  assert(d_memManager!=0);                                                                                              
  if (maxDepth>0) {                                                                                                     
    d_memNode.ptr = d_rawNode;                                                                                          
    d_key = d_memManager->mallocKeySpace(maxDepth);                                                                   
    next();                                                                                                             
  } else {                                                                                                              
    d_end = true;                                                                                                       
  }                                                                                                                     
}                                                                                                                       
                                                                                                                        
Radix::TreeIterator::~TreeIterator() {                                                                                         
  if (d_key) {                                                                                                          
    d_memManager->freeKeySpace(d_key);                                                                                  
    d_key = 0;                                                                                                          
  }                                                                                                                     
}     

std::ostream& Radix::TreeIterator::print(std::ostream& stream) const {
  assert(!d_end);

  stream << "key: '";

  for (unsigned i=0; i<keySize(); ++i) {
    if (!isprint(d_key[i])) {
      char buf[5];
      sprintf(buf, "0x%02x", d_key[i]);
      stream << buf;
    } else {
      stream << d_key[i];
    }
  }

  stream << "' isCompressed: "  << isCompressed()
         << " isTerminal: "     << isTerminal()
         << " isLeafNode: "     << isLeaf()
         << std::endl;
  return stream;
}

void Radix::TreeIterator::next() {
begin:
  while (d_index<Radix::k_MAX_CHILDREN256) {
    if (d_memNode.ptr->d_children[d_index]==0) {
      ++d_index;
      continue;
    }

    if (d_memNode.ptr->d_children[d_index]==RadixLeafNode) {
      d_key[d_keySize]=(u_int8_t)d_index;
      d_attributes = 0xffUL;
      ++d_index;
      return;
    }

    d_nodeHelper.ptr = d_memNode.ptr->d_children[d_index];
    if (d_nodeHelper.val & RadixTermMask) {
      d_key[d_keySize]=(u_int8_t)d_index;
      d_attributes = 0xffUL;
      d_attributes = d_nodeHelper.val & (k_IS_CHILDREN_COMPRESSED | k_IS_TERMINAL_NODE);
      ++d_index;
      return;
    }

    // otherwise Inner node so recurse w/ stack
    d_stack.push(Radix::TreeIterState(d_rawNode, d_index, d_keySize));
    d_rawNode = d_memNode.ptr = d_memNode.ptr->d_children[d_index];
    d_memNode.val &= RadixTagClear;
    d_index = 0xffff; // increments to back to 0
    ++d_keySize;
  }

  if (!d_stack.empty()) {
    Radix::TreeIterState state = d_stack.top();
    d_index = state.d_index+1;
    d_rawNode = d_memNode.ptr = state.d_node;
    d_memNode.val &= RadixTagClear;
    d_stack.pop();
    goto begin;
  }

  d_end = true;
}

int Radix::Tree::findHelper(const u_int8_t *key, const u_int16_t size) const {
  assert(key!=0);
  assert(size>0);

  Node256* node = const_cast<Node256*>(&d_root);

  union {
    Node256  *ptr;    // as pointer
    u_int64_t val;    // as u_int64_t
  } childNode;

  bool childWasTerminal(false);

  for (u_int16_t i=0; i<size; ++i) {
    childNode.ptr = node->d_children[key[i]];
    if (childNode.ptr>RadixLeafNode) {
      childWasTerminal = childNode.val & RadixTermMask;
      childNode.val &= RadixTagClear;
      node = childNode.ptr; 
    } else if (childNode.ptr==0) {
      return e_NOT_FOUND;
    } else if (childNode.ptr==RadixLeafNode) {
      return ((i+1U)==size) ? e_EXISTS : e_NOT_FOUND;
    }
  }

  return (childNode.ptr==RadixLeafNode || childWasTerminal) ? e_EXISTS : e_NOT_FOUND;
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
  Node256 *pNode = &d_root; // raw parent of node

  for (u_int16_t i=0; i<size; ++i) {
    // Follow edge @ key[i] from node to child
    Node256 *childNode = node.ptr->d_children[key[i]];
    if (childNode>RadixLeafNode) {
      pNode = node.ptr;
      node.ptr = childNode;
      node.val &= RadixTagClear;
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
        // the pointer to it needs to be updated. Leafs have no children
        *lastMatch = d_memManager->mallocNode256();
        pNode->d_children[key[i]] = *lastMatch;
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

  node.ptr = pNode->d_children[key[size-1]];
  node.val |= RadixTermMask;
  pNode->d_children[key[size-1]] = node.ptr;

  return e_OK;
}

int Radix::Tree::insert(const Benchmark::Slice<u_int8_t> key) {
  int rc;
  const u_int16_t size = key.size();
  const u_int8_t *keyPtr = key.data();

  // Search for key
  Node256 *lastMatch(0);
  u_int16_t lastMatchIndex(0);
  if ((rc = insertHelper(keyPtr, size, &lastMatchIndex, &lastMatch)) != e_NOT_FOUND) {
    return rc;
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

  if (size>d_currentMaxDepth) {
    d_currentMaxDepth = size;
  }

  return Radix::e_OK;
}

void Radix::Tree::statistics(TreeStats *stats) const {
  assert(stats);
  stats->reset();

  u_int16_t i(0);
  u_int16_t depth(0);
  std::stack<Radix::TreeIterState> stack;

  Radix::Node256 *rawNode = const_cast<Radix::Node256*>(&d_root);

  union {
    Node256  *ptr;    // as pointer
    u_int64_t val;    // as u_int64_t
  } memNode, nodeHelper;

  memNode.ptr = rawNode;

  stats->d_totalSizeBytes += sizeof(Node256); // of root

  // On entry to loop rawNode & memNode point
  // to root there are no tags to worry about
begin:
  for (; i<Radix::k_MAX_CHILDREN256; ++i) {
    if (memNode.ptr->d_children[i]==RadixLeafNode) {
      ++stats->d_leafCount;
      if ((depth+1U)>stats->d_maxDepth) {
        stats->d_maxDepth = depth+1;
      }
      continue;
    }

    if (memNode.ptr->d_children[i]==0) {
      ++stats->d_emptyChildCount;
      continue;
    }

    // otherwise Inner node
    ++stats->d_innerNodeCount;
    if ((depth+1U)>stats->d_maxDepth) {
      stats->d_maxDepth = depth+1;
    }
    nodeHelper.ptr = memNode.ptr->d_children[i];
    if (nodeHelper.val & RadixTermMask) {
      ++stats->d_leafCount;
    }
    stats->d_totalSizeBytes += sizeof(Node256); // inner node
    
    // Recurse w/ stack
    stack.push(Radix::TreeIterState(rawNode, i, depth));
    rawNode = memNode.ptr = memNode.ptr->d_children[i];
    memNode.val &= RadixTagClear;
    i = 0xffff; // increments to back to 0
    ++depth;
  }

  if (!stack.empty()) {
    Radix::TreeIterState state = stack.top();
    i = state.d_index+1;
    rawNode = memNode.ptr = state.d_node;
    memNode.val &= RadixTagClear;
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

  Radix::Node256 *rawNode = const_cast<Radix::Node256*>(&d_root);

  union {
    Node256  *ptr;    // as pointer
    u_int64_t val;    // as u_int64_t
  } memNode, nodeHelper;

  memNode.ptr = rawNode;

  stream << "digraph Radix {" << std::endl;
  stream << "  " << reinterpret_cast<u_int64_t>(rawNode) << " [shape=diamond, label=\"root\"]" << std::endl;

  // On entry to loop rawNode & memNode point
  // to root there are no tags to worry about
begin:
  for (; i<Radix::k_MAX_CHILDREN256; ++i) {
    if (memNode.ptr->d_children[i]==RadixLeafNode) {
      // The leaf node
      if (isprint(i)) {
        buf[0] = char(i);
        buf[1] = 0;
      } else {
        sprintf(buf, "0x%02u", i);
      }
      stream << "  \""
             << reinterpret_cast<u_int64_t>(memNode.ptr->d_children[i])
             << "_" 
             << buf
             << "\""
             << " [shape=box, "
             << "label=\""
             << buf
             << "\"]"
             << std::endl;
      // The edge from parent to leaf
      stream << "  "
             << reinterpret_cast<u_int64_t>(rawNode)
             << " -> \""
             << reinterpret_cast<u_int64_t>(memNode.ptr->d_children[i])
             << "_" 
             << buf
             << "\""
             << std::endl;
      continue;
    }

    if (memNode.ptr->d_children[i]==0) {
      continue;
    }

    // otherwise Inner node
    if (isprint(i)) {
      buf[0] = char(i);
      buf[1] = 0;
    } else {
      sprintf(buf, "0x%02u", i);
    }
    nodeHelper.ptr = memNode.ptr->d_children[i];
    if (nodeHelper.val & RadixTermMask) {
      stream << "  "
             << reinterpret_cast<u_int64_t>(nodeHelper.ptr)
             << " [shape=box, "
             << "label=\""
             << buf
             << "\"]"
             << std::endl;
    } else { 
      stream << "  "
             << reinterpret_cast<u_int64_t>(nodeHelper.ptr)
             << " [shape=circle, "
             << "label=\""
             << buf
             << "\"]"
             << std::endl;
    }
    // The edge from parent to inner node
    stream << "  "
           << reinterpret_cast<u_int64_t>(rawNode)
           << " -> "
           << reinterpret_cast<u_int64_t>(nodeHelper.ptr)
           << std::endl;

    // Recurse w/ stack
    stack.push(Radix::TreeIterState(rawNode, i, depth));
    rawNode = memNode.ptr = memNode.ptr->d_children[i];
    memNode.val &= RadixTagClear;
    i = 0xffff; // increments to back to 0
  }

  if (!stack.empty()) {
    Radix::TreeIterState state = stack.top();
    i = state.d_index+1;
    rawNode = memNode.ptr = state.d_node;
    memNode.val &= RadixTagClear;
    depth = state.d_depth;
    stack.pop();
    goto begin;
  }

  stream << "}" << std::endl;
}

void Radix::Tree::destroy() {
  u_int16_t i(0);
  std::stack<Radix::TreeIterState> stack;

  Radix::Node256 *rawNode = const_cast<Radix::Node256*>(&d_root);

  union {
    Node256  *ptr;    // as pointer
    u_int64_t val;    // as u_int64_t
  } memNode;

  memNode.ptr = rawNode;

begin:
  for (; i<Radix::k_MAX_CHILDREN256; ++i) {
    // Easy case
    if (memNode.ptr->d_children[i]==RadixLeafNode) {
      continue;
    }

    // Easy case
    if (memNode.ptr->d_children[i]==0) {
      continue;
    }

    // Inner node, recurse w/ stack
    stack.push(Radix::TreeIterState(rawNode, i, 0));
    rawNode = memNode.ptr = memNode.ptr->d_children[i];
    memNode.val &= RadixTagClear;
    i = 0xffff; // increments to back to 0
  }

  if (rawNode!=&d_root) {
    d_memManager->freeNode256(rawNode);
  }

  if (!stack.empty()) {
    Radix::TreeIterState state = stack.top();
    i = state.d_index+1;
    rawNode = memNode.ptr = state.d_node;
    memNode.val &= RadixTagClear;
    stack.pop();
    goto begin;
  }

  // To avoid double delete on stale pointers
  memset(d_root.d_children, 0, sizeof(d_root));
}
