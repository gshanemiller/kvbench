#include <cradix_node256.h> 
#include <string.h>

CRadix::NodeStats CRadix::Node256::d_nodeStats;

bool CRadix::Node256::trySetOffset(const u_int32_t index, const u_int32_t offset, int32_t& newMin, int32_t& newMax) {
  assert(!isDead());
  assert(index<k_MAX_CHILDREN);

#ifdef CRADIX_NODE_RUNTIME_STATISTICS                                                                                   
  ++d_nodeStats.d_trySetOffsetCount;
#endif

  int32_t oldMin, oldMax, delta;
  if (!canSetOffset((int32_t)index, oldMin, oldMax, newMin, newMax, delta)) {
    // cannot set 'index=offset': node needs reallocation
    return false;
  }

  // This API takes exactly one offset at one one index. That means if the index
  // does not fall into case 3, then either there's a new min (and old max unchanged)
  // or there's a new max (and the old min is unchanged).
  //
  // At this point we have: d_offset[] -> [0: <oldMin offset>, ..., n: <oldMax offset>]
  //
  // Case 1: When delta!=0 && newMin < oldMin we want:
  // -------------------------------------------------
  // d_offset[] -> [0: <newMin: 'offset'>, ..., n: <oldMin offset>, ..., n+m: <oldMax offset>]
  // which is accomplished with memmove and array write. This has worst performance.
  //       
  // Case 2: When delta!=0 && newMax > oldMax we want:
  // -------------------------------------------------
  // d_offset[] -> [0: <oldMin offset>, ..., n: <oldMax offset>, ..., m+n: <newMax: 'offset']
  // which is accomplished by appending 0s then 'offset' at end. This has medium performance.
  //
  // Case 3: delta==0 implies index in [oldMinIndex, oldMaxIndex]
  // -------------------------------------------------
  // Which is an array write, and has fastest performance

  const u_int32_t oldCapacity = capacity();

  if (delta) {
    // Ensure non-zero delta implies new min or new max
    assert(!(index>=minIndex()&&index<=maxIndex()));
    if (newMin<oldMin) { 
      // Case 1: re-confim index is the new min
      assert((int32_t)index==newMin);
      // confirm oldmax did not change
      assert(oldMax==newMax);
      // ensure we're not writing beyond end of d_offset. 'size()' gives the
      // number of entries in d_offset now prior to adding new offset and 
      // updating this' minIndex():
      assert((void*)(d_offset+delta+size()) <= (void*)(d_offset+capacity()));
      // move old offsets right in d_offset. note memmove requires len in bytes
      memmove(d_offset+delta, d_offset, size()<<2);
      // check we're not memsetting beyond end of d_offset
      assert((void*)(d_offset+delta) <= (void*)(d_offset+capacity()));
      // memset new empty slots to 0
      memset(d_offset, 0, delta<<2);
      // set/update offset
      d_offset[0] = offset;
      // clear old min
      d_udata &= 0xFFFFFF00;
      // set new min
      d_udata |= index;
#ifdef CRADIX_NODE_RUNTIME_STATISTICS                                                                                   
      ++d_nodeStats.d_trySetOffsetCase1Count;
      d_nodeStats.d_bytesCleared += (delta<<2);
      d_nodeStats.d_bytesCopied += (delta<<2);
#endif
    } else  {
      // Case 2: make sure new max
      assert(newMax>oldMax);
      assert(newMin==oldMin);
      // re-confirm 'index' is the newMax
      assert((int32_t)index==newMax);
      // make sure byte count for memset next is >=0
      assert(((delta-1)*4)>=0);
      // make sure we're not memsetting beyond end of d_offset.
      // here 'size()' gives the number of entries in d_offset
      // now before adding new offset and updating new max:
      assert((size()+delta)<=(int)capacity());
      // Append 0s e.g. null
      memset(d_offset+size(), 0, delta<<2);
      // set/update offset
      d_offset[index-minIndex()] = offset;
      // clear old max
      d_udata &= 0xFFFF00FF;
      // set new max
      d_udata |= (index<<8);
#ifdef CRADIX_NODE_RUNTIME_STATISTICS                                                                                   
      ++d_nodeStats.d_trySetOffsetCase2Count;
      d_nodeStats.d_bytesCleared += (delta<<2);
#endif
    } 
  } else {
    // Case 3
    assert(index>=minIndex()&&index<=maxIndex());
    assert((index-minIndex())<usize());                                                                                    
    assert((index-minIndex())<capacity()); 
    d_offset[index-minIndex()] = offset;
#ifdef CRADIX_NODE_RUNTIME_STATISTICS                                                                                   
      ++d_nodeStats.d_trySetOffsetCase3Count;
#endif
    return true;
  }

  // Update spare capacity since slot(s) just used. Note that capacity
  // is an invariant: the number of offsets this node can hold is fixed
  // at object creation time. Using the definition of size(), capacity()
  // we can work out the new spare capacity. In this code 'size()'
  // reflects the new size since those updates happened above:
  d_udata &= 0xFF00FFFF;
  // Set new spare capacity
  assert(oldCapacity>=usize());
  d_udata |= ((oldCapacity-size()) << 16);

#ifndef NDEBUG
  assert(capacity()==oldCapacity);
#endif

  return true;
}
