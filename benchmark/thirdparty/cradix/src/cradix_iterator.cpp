#include <cradix_iterator.h>

void CRadix::Iterator::next() {
  // When resuming from 'd_jump = true'
  if (d_jump) {
    d_jump = false;
    goto jump;
  }

begin:
  while (d_index<=d_maxIndex) {
    d_childNode = d_memNode.ptr->offset(d_index);

    if (d_childNode<k_MEMMANAGER_MIN_OFFSET) {
      if (d_childNode==k_NODE256_IS_LEAF) {
        d_attributes = d_childNode;
        d_key[d_depth] = (u_int8_t)(d_index);
        ++d_index;
        return;
      } else if (!d_childNode) {
        ++d_index;
        continue;
      }
    }

    // leaf node and zero nodes already handled
    // at this point must be an inner node
    assert(d_childNode>=k_MEMMANAGER_MIN_OFFSET);

    // but could be a terminal inner node
    if (d_childNode & k_NODE256_IS_TERMINAL) {
      d_attributes = k_NODE256_IS_TERMINAL;
      // clear terminal mask leaving vanilla inner node offset
      d_childNode &= k_NODE256_CLR_TERMINAL_MASK;
      d_key[d_depth] = (u_int8_t)(d_index);
      d_jump = true;
      return;
    }

jump:
    // otherwise non-terminal inner node so recurse w/ stack
    assert(d_depth<d_maxDepth);
    assert((d_childNode & k_NODE256_IS_LEAF) == 0);
    assert((d_childNode & k_NODE256_IS_TERMINAL) == 0);
    d_key[d_depth] = (u_int8_t)(d_index);
    d_stack.push(CRadix::IterState(d_childNode, d_index, d_depth));
    // make a valid Node256* from d_childNode
    d_memNode.uint8Ptr = d_basePtr;
    d_memNode.val += d_childNode;
    d_index = d_memNode.ptr->minIndex();
    d_maxIndex = d_memNode.ptr->maxIndex();
    d_depth++;
    goto begin;
  }

  if (!d_stack.empty()) {
    CRadix::IterState state = d_stack.top();
    d_index = state.d_index+1;
    d_depth = state.d_depth;
    d_childNode = state.d_node;
    d_stack.pop();
    goto begin;
  }

  d_end = true;
}
