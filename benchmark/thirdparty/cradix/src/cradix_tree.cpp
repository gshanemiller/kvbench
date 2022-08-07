#include <cradix_tree.h>
#include <cradix_node256.h>
#include <cradix_memmanager.h>

CRadix::Tree::Tree(MemManager *memManager)
: d_memManager(memManager)
, d_root(0)
, d_currentMaxDepth(0)
{
  assert(d_memManager!=0);
  d_root = d_memManager->newRoot();
  assert(d_root);
}

CRadix::Iterator CRadix::Tree::begin() const {
  return Iterator(d_root, currentMaxDepth(), d_memManager->basePtr(), (u_int8_t*)malloc(currentMaxDepth()));
}

int CRadix::Tree::findHelper(const u_int8_t *key, const u_int16_t size) const {
  assert(key!=0);
  assert(size>0);

  u_int8_t *basePtr = const_cast<u_int8_t *>(d_memManager->basePtr());                                                      
  Node256 *node = (Node256*)(basePtr+d_root);
  u_int32_t childOffset(0);
  bool childWasTerminal(false);

  for (u_int32_t i=0; i<size; ++i) {
    childOffset = node->tryOffset(key[i]);
    if (childOffset>=k_MEMMANAGER_MIN_OFFSET) {
      childWasTerminal = childOffset & k_NODE256_IS_TERMINAL;
      node = (Node256*)(basePtr+(childOffset&k_NODE256_CLR_TERMINAL_MASK));
    } else if (childOffset==k_NODE256_IS_LEAF) {
      return ((i+1U)==size) ? e_EXISTS : e_NOT_FOUND;
    } else {
      assert(childOffset==0);
      return e_NOT_FOUND;
    }
  }

  return (childOffset==k_NODE256_IS_LEAF || childWasTerminal) ? e_EXISTS : e_NOT_FOUND;
}

int CRadix::Tree::insertHelper(const u_int8_t *key, const u_int16_t size,
  u_int16_t *lastMatchIndex, u_int32_t *lastMatch, u_int32_t *lastMatchParent) {
  assert(key!=0);
  assert(size>0);
  assert(lastMatchIndex!=0);
  assert(lastMatch!=0);
  assert(lastMatchParent!=0);

  // We're tracking 3 nodes connected by two edges: pOffset -> offset -> childOffset
  u_int32_t offset = d_root;
  u_int32_t pOffset = d_root;
  u_int32_t childOffset = 0;

  u_int8_t *basePtr = const_cast<u_int8_t *>(d_memManager->basePtr());
  Node256  *offsetPtr = (Node256*)(basePtr+d_root);

  for (u_int32_t i=0; i<size; ++i) {
    childOffset = offsetPtr->tryOffset(key[i]);
    if (childOffset>=k_MEMMANAGER_MIN_OFFSET) {
      pOffset = offset;
      offset = childOffset;
      offsetPtr = (Node256*)(basePtr+(childOffset&k_NODE256_CLR_TERMINAL_MASK));
    } else if (childOffset==0) {
      assert(i<size);
      assert(offset);
      *lastMatchIndex = i;
      *lastMatch = offset;
      *lastMatchParent = pOffset;
      return e_NOT_FOUND;
    } else {
      assert(childOffset==k_NODE256_IS_LEAF);
      *lastMatchIndex = i+1;
      if (*lastMatchIndex!=size) {
        // Last byte matched ends on leaf node. However the whole key
        // was not found so insert will have work to do. But in order
        // to do that, lastMatch has to be promoted to a Node256, and
        // the pointer to it needs to be updated. When creating this
        // new node, we reserve space and set key[*lastMatchIndex] to 0.
        // This is done, because on return, key[*lastMatchIndex] will
        // be immediately modified to have a new, decendent node in order
        // to complete insertion. We also mark this new node terminal
        // (it was a leaf implying terminal).
        *lastMatch = d_memManager->newNode256(k_MEMMANAGER_DEFAULT_CAPACITY, key[*lastMatchIndex], 0);
        offsetPtr->setOffset(key[i], (*lastMatch|k_NODE256_IS_TERMINAL));
        *lastMatchParent = pOffset;
        return e_NOT_FOUND;
      } else {
        return e_EXISTS;
      }
    }
  }

  assert(pOffset);

  // Edge from pOffset to offset refers to last byte in the key. So there's a
  // definite match. To get here pOffset must be a inner node since all other
  // cases must have already returned from loop above. The remaining issue
  // is to ensure the link/pointer from pOffset to offset is marked terminal.
  // We'll reuse node union to so mark it
  assert(pOffset>=k_MEMMANAGER_MIN_OFFSET);
  offsetPtr = (Node256*)(basePtr+(pOffset&k_NODE256_CLR_TERMINAL_MASK));
  offset = offsetPtr->offset(key[size-1]);
  offsetPtr->setOffset(key[size-1], (offset|k_NODE256_IS_TERMINAL));

  return e_OK;
}

int CRadix::Tree::insert(const Benchmark::Slice<u_int8_t> key) {
  int rc;
  u_int32_t lastMatch(0);
  u_int32_t lastMatchParent(0);
  u_int16_t lastMatchIndex(0);
  const u_int16_t size = key.size();
  const u_int8_t *keyPtr = key.data();

  // Find node with longest preexisting prefix of key
  if ((rc = insertHelper(keyPtr, size, &lastMatchIndex, &lastMatch, &lastMatchParent)) != e_NOT_FOUND) {
    return rc;
  }

  // lastMatch is an existing node in tree s.t. it's the node where
  // 'keyPtr[lastMatchIndex]' terminates. Since the full key was not
  // found new children must be assigned to it in one of two variations:
  //
  // Case 1: there's only one more byte in key to add and therefore
  //         lastMatch @ lastMatchIndex+1 must point to k_NODE256_IS_LEAF.
  //
  // Case 2: there's two or more bytes to add. This amounts to adding
  //         a one-way linked list of children successively. The very
  //         last byte will be like case 1 except on a different parent.
  // 
  // Either way an offset in lastMatch will have to be updated and in
  // either case the index to be set may not be in node's span. That'll
  // mean it need to be copy-reallocated. That will invalidate the pointer
  // to it in lastMatch's parent. It's updated too. This updating business
  // comes the minor issue that 'lastMatch, lastMatchParent' may have
  // tagging bits on them. Those are masked out.

  assert(lastMatchIndex<size);
  assert(lastMatch>0);
  assert(lastMatch!=k_NODE256_IS_LEAF);
  assert(lastMatch>=k_MEMMANAGER_MIN_OFFSET);

  // Set up to finish insertion
  u_int8_t *basePtr = const_cast<u_int8_t *>(d_memManager->basePtr());
  Node256 *lastMatchPtr = (Node256*)(basePtr+(lastMatch&k_NODE256_NO_TAG_MASK));
  int32_t oldMin, oldMax, newMin, newMax, delta, newOffset;
  u_int8_t byte(keyPtr[lastMatchIndex]);

  if (!lastMatchPtr->canSetOffset(byte, oldMin, oldMax, newMin, newMax, delta)) {
    // lastMatch doesn't have enough capacity: reallocate
    newOffset = d_memManager->copyAllocateNode256(newMin, newMax, lastMatch&k_NODE256_NO_TAG_MASK);
    // get a pointer to lastMatch's parent
    Node256 *lastMatchParentPtr = (Node256*)(basePtr+(lastMatchParent&k_NODE256_NO_TAG_MASK));
    // Ensure 'keyPtr[lastMatchIndex-1]' next line makes sense
    assert(lastMatchIndex>0); 
    // Double check parent really points to lastMatch
    assert(lastMatchParentPtr->offset(keyPtr[lastMatchIndex-1])==lastMatch);
    // update pointer in tree to new 'lastMatch' carrying forward any tags
    lastMatchParentPtr->setOffset(keyPtr[lastMatchIndex-1], newOffset | (lastMatch&k_NODE256_ANY_TAG));
    // update pointer to 'lastMatch' for code below
    lastMatchPtr = (Node256*)(basePtr+newOffset);
  }

  // Case 2
  while (lastMatchIndex < size-1) {
    newOffset = d_memManager->newNode256(k_MEMMANAGER_DEFAULT_CAPACITY, keyPtr[lastMatchIndex+1], 0);
    lastMatchPtr->setOffset(byte, newOffset);
    lastMatchPtr = (Node256*)(basePtr+newOffset);
    byte = keyPtr[++lastMatchIndex];
  }
  // Termination of case 2 OR case 1
  lastMatchPtr->setOffset(byte, k_NODE256_IS_LEAF);

  if (size>d_currentMaxDepth) {
    d_currentMaxDepth = size;
  }

  return CRadix::e_OK;
}

void CRadix::Tree::statistics(TreeStats *stats) const {
  assert(stats);
  stats->reset();

/*
  u_int16_t i(0);
  u_int16_t depth(0);
  std::stack<CRadix::IterState> stack;

  CRadix::Node256 *rawNode = const_cast<CRadix::Node256*>(&d_root);

  union {
    Node256  *ptr;    // as pointer
    u_int64_t val;    // as u_int64_t
  } memNode, nodeHelper;

  memNode.ptr = rawNode;

  stats->d_totalSizeBytes += sizeof(Node256); // of root

  // On entry to loop rawNode & memNode point
  // to root there are no tags to worry about
begin:
  for (; i<CRadix::k_MAX_CHILDREN; ++i) {
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
    stack.push(CRadix::IterState(rawNode, i, depth));
    rawNode = memNode.ptr = memNode.ptr->d_children[i];
    memNode.val &= RadixTagClear;
    i = 0xffff; // increments to back to 0
    ++depth;
  }

  if (!stack.empty()) {
    CRadix::IterState state = stack.top();
    i = state.d_index+1;
    rawNode = memNode.ptr = state.d_node;
    memNode.val &= RadixTagClear;
    depth = state.d_depth;
    stack.pop();
    goto begin;
  }
*/
}

void CRadix::Tree::dotGraph(std::ostream& stream) const {
  char buf[16];
  u_int16_t i(0);
  u_int16_t depth(0);
  std::stack<CRadix::IterState> stack;

/*
  CRadix::Node256 *rawNode = const_cast<CRadix::Node256*>(&d_root);

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
  for (; i<CRadix::k_MAX_CHILDREN; ++i) {
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
    stack.push(CRadix::IterState(rawNode, i, depth));
    rawNode = memNode.ptr = memNode.ptr->d_children[i];
    memNode.val &= RadixTagClear;
    i = 0xffff; // increments to back to 0
  }

  if (!stack.empty()) {
    CRadix::IterState state = stack.top();
    i = state.d_index+1;
    rawNode = memNode.ptr = state.d_node;
    memNode.val &= RadixTagClear;
    depth = state.d_depth;
    stack.pop();
    goto begin;
  }

  stream << "}" << std::endl;
*/
}

void CRadix::Tree::destroy() {
  u_int16_t i(0);
  std::stack<CRadix::IterState> stack;

/*
  CRadix::Node256 *rawNode = const_cast<CRadix::Node256*>(&d_root);

  union {
    Node256  *ptr;    // as pointer
    u_int64_t val;    // as u_int64_t
  } memNode;

  memNode.ptr = rawNode;

begin:
  for (; i<CRadix::k_MAX_CHILDREN; ++i) {
    // Easy case
    if (memNode.ptr->d_children[i]==RadixLeafNode) {
      continue;
    }

    // Easy case
    if (memNode.ptr->d_children[i]==0) {
      continue;
    }

    // Inner node, recurse w/ stack
    stack.push(CRadix::IterState(rawNode, i, 0));
    rawNode = memNode.ptr = memNode.ptr->d_children[i];
    memNode.val &= RadixTagClear;
    i = 0xffff; // increments/wraps to back to 0
  }

  if (rawNode!=&d_root) {
    // Don't free pointers with tags on them that
    // memory allocator won't recognize
    memNode.ptr = rawNode;
    memNode.val &= RadixNoTag;
    d_memManager->freeNode256(memNode.ptr);
  }

  if (!stack.empty()) {
    CRadix::IterState state = stack.top();
    i = state.d_index+1;
    rawNode = memNode.ptr = state.d_node;
    memNode.val &= RadixTagClear;
    stack.pop();
    goto begin;
  }

  // To avoid double delete on stale pointers
  memset(d_root.d_children, 0, sizeof(d_root));
*/
}
