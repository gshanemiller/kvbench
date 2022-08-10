#include <cradix_node256.h> 
#include <string.h>

bool CRadix::Node256::trySetOffset(const u_int32_t index, const u_int32_t offset, int32_t& newMin, int32_t& newMax) {
  assert(!isDead());
  assert(index<k_MAX_CHILDREN);

#ifdef CRADIX_NODE_RUNTIME_STATISTICS                                                                                   
  ++d_nodeStats.d_trySetOffsetCount;
#endif

  int32_t oldMin, oldMax, delta;
  if (!canSetOffset((int32_t)index, oldMin, oldMax, newMin, newMax, delta)) {
    return false;
  }
  assert(delta <= spareCapacity());
  printf("computed oldMin %d oldMax %d newMin %d newMax %d delta%d size() %d\n", oldMin, oldMax, newMin, newMax, delta, size());
  std::cout << "BEFORE CASES: ";
  statistics(std::cout);

#ifndef NDEBUG
  const u_int32_t oldCapacity = capacity();
#endif

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

  if (delta) {
    assert(!(index>=minIndex()&&index<=maxIndex()));
    if (newMin<oldMin) { 
      // case 1
      assert((int32_t)index==newMin);
      assert(((size()-delta+1)<<2)>=0);
      memmove(d_offset+delta, d_offset, (size()-delta+1)<<2);
      // assign newMin
      d_offset[0] = offset;
      // And out old min
      d_udata &= 0xFFFFFF00;
      // Or in new min
      d_udata |= index;
#ifdef CRADIX_NODE_RUNTIME_STATISTICS                                                                                   
      printf("case 1\n");
      ++d_nodeStats.d_trySetOffsetCase1Count;
      d_nodeStats.d_bytesCopied += (size()-delta+1)<<2;
#endif
    } else  {
      // case 2
      assert(newMax>oldMax);
      assert((int32_t)index==newMax);
      // Append 0s e.g. null
      assert(((delta-1)<<2)>=0);
      memset(d_offset+(maxIndex()-minIndex()+1), 0, (delta-1)<<2);
      // assign newMax
      d_offset[index-minIndex()] = offset;
      // And out old max
      d_udata &= 0xFFFF00FF;
      // Shift in new max
      d_udata |= (index<<8);
#ifdef CRADIX_NODE_RUNTIME_STATISTICS                                                                                   
      printf("case 2\n");
      ++d_nodeStats.d_trySetOffsetCase2Count;
      d_nodeStats.d_bytesCleared += (delta-1)<<2;
#endif
    } 
  } else {
    // case 3
    assert(index>=minIndex()&&index<=maxIndex());
    d_offset[index-minIndex()] = offset;
#ifdef CRADIX_NODE_RUNTIME_STATISTICS                                                                                   
      printf("case 3\n");
      ++d_nodeStats.d_trySetOffsetCase3Count;
#endif
    return true;
  }

  // reduce spare capacity since slot just used
  assert(uspareCapacity()>0);
  u_int32_t scap = uspareCapacity() - 1;
  // And out old spare capacity
  d_udata &= 0xFF00FFFF;
  // Shift in reduced capacity
  d_udata |= (scap << 16);

  std::cout << "AFTER CASES: ";
  statistics(std::cout);
#ifndef NDEBUG
  assert(oldCapacity==capacity());
#endif
  return true;
}
