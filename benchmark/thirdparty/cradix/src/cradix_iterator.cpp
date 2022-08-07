#include <cradix_iterator.h>

void CRadix::Iterator::next() {
  // When resuming from 'd_jump = true'
  if (d_jump) {
    d_jump = false;
    goto jump;
  }

begin:
  while (d_index<=d_maxIndex) {
    assert((d_node&k_NODE256_IS_LEAF)==0);
    d_childNode = d_nodePtr->offset(d_index);

    if (d_childNode<k_MEMMANAGER_MIN_OFFSET) {
      if (d_childNode==k_NODE256_IS_LEAF) {
        d_attributes = k_NODE256_IS_LEAF;
        d_key[d_depth] = (u_int8_t)(d_index);
        ++d_index;
        return;
      } else {
        assert(d_childNode==0);
        ++d_index;
        continue;
      }
    }

    // Must be an inner node
    assert(d_childNode>=k_MEMMANAGER_MIN_OFFSET);

    // But could be a terminal inner node
    if (d_childNode & k_NODE256_IS_TERMINAL) {
      d_attributes = k_NODE256_IS_TERMINAL;
      d_key[d_depth] = (u_int8_t)(d_index);
      d_jump = true;
      printf("JUMP: d_childNode %u d_node %u d_index %u d_depth %u\n", d_childNode, d_node, d_index, d_depth);
      return;
    }

jump:
    // otherwise non-terminal inner node so recurse w/ stack
    printf("REST-JUMP: d_childNode %u d_node %u d_index %u d_depth %u\n", d_childNode, d_node, d_index, d_depth);
    assert(d_depth<d_maxDepth);
    assert((d_childNode&k_NODE256_IS_LEAF)==0);
    d_key[d_depth] = (u_int8_t)(d_index);
    d_stack.push(CRadix::IterState(d_node, d_index, d_depth));
    // following d_childNode: reset
    d_node = d_childNode;
    d_nodePtr = (Node256*)(d_basePtr+(d_childNode&k_NODE256_NO_TAG_MASK));
    d_index = d_nodePtr->minIndex();
    d_maxIndex = d_nodePtr->maxIndex();
    d_depth++;
    goto begin;
  }

  if (!d_stack.empty()) {
    d_index = d_stack.top().d_index+1;
    d_depth = d_stack.top().d_depth;
    d_node = d_stack.top().d_node;
    d_nodePtr = (Node256*)(d_basePtr+(d_node&k_NODE256_NO_TAG_MASK));
    d_maxIndex = d_nodePtr->maxIndex();
    d_stack.pop();
    goto begin;
  }

  d_end = true;
}
