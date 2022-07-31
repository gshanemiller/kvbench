#include <radix.h>
#include <radix_memmanager.h>
#include <radix_enums.h>

CRadix::TreeIterator::TreeIterator(CRadix::MemManager *memManager, CRadix::Node256& root, u_int64_t maxDepth)
: d_memManager(memManager)                                                                                              
, d_root(root)                                                                                                          
, d_rawNode(&d_root)                                                                                                    
, d_key(0)                                                                                                              
, d_attributes(0)                                                                                                       
, d_index(0)                                                                                                            
, d_depth(0)                                                                                                          
, d_maxDepth((u_int16_t)maxDepth)
, d_end(false)                                                                                                          
, d_jump(false)
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
                                                                                                                        
CRadix::TreeIterator::~TreeIterator() {                                                                                         
  if (d_key) {                                                                                                          
    d_memManager->freeKeySpace(d_key);                                                                                  
    d_key = 0;                                                                                                          
  }                                                                                                                     
}     

std::ostream& CRadix::TreeIterator::print(std::ostream& stream) const {
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

void CRadix::TreeIterator::next() {
  // When resuming from line 87
  if (d_jump) {
    d_jump = false;
    goto jump;
  }
begin:
  while (d_index<CRadix::k_MAX_CHILDREN256) {
    if (d_memNode.ptr->d_children[d_index]==0) {
      ++d_index;
      continue;
    }

    if (d_memNode.ptr->d_children[d_index]==RadixLeafNode) {
      d_attributes = 0xffUL;
      d_key[d_depth] = (u_int8_t)(d_index);
      ++d_index;
      return;
    }

    d_nodeHelper.ptr = d_memNode.ptr->d_children[d_index];
    if (d_nodeHelper.val & RadixTermMask) {
      d_attributes = d_nodeHelper.val & (k_IS_CHILDREN_COMPRESSED | k_IS_TERMINAL_NODE);
      d_key[d_depth] = (u_int8_t)(d_index);
      d_jump = true;
      return;
    }

    // otherwise Inner node so recurse w/ stack
jump:
    assert(d_key);
    assert(d_depth<d_maxDepth);
    d_key[d_depth] = (u_int8_t)(d_index);
    d_stack.push(CRadix::TreeIterState(d_rawNode, d_index, d_depth));
    d_rawNode = d_memNode.ptr = d_memNode.ptr->d_children[d_index];
    d_memNode.val &= RadixTagClear;
    d_index = 0;
    d_depth++;
    goto begin;
  }

  if (!d_stack.empty()) {
    CRadix::TreeIterState state = d_stack.top();
    d_index = state.d_index+1;
    d_depth = state.d_depth;
    d_rawNode = d_memNode.ptr = state.d_node;
    d_memNode.val &= RadixTagClear;
    d_stack.pop();
    goto begin;
  }

  d_end = true;
}
