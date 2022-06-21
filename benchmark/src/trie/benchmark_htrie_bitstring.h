#pragma once

// PURPOSE: Fixed size array-like container of bits
//          
// CLASSES:
//  Benchmark::HTrie::BitSting: Fixed size array of bits. Capacity set at
//                              compile time with template parameter supporting
//                              random reads and write through 'append'. Class
//                              also provides helper methods for Trie creation
//                              for bit extraction, difference calculations

#include <trie/benchmark_htrie_typedefs.h>
#include <trie/benchmark_htrie_bititerator.h>
#include <trie/benchmark_htrie_config.h>

#include <iostream>
#include <assert.h>
#include <initializer_list>

namespace Benchmark {
namespace HTrie     {

struct BitStringStats {
  static unsigned int d_byteSuffixCalls;
  static unsigned int d_bytePrefixCalls;
  static unsigned int d_substringCalls;
  static unsigned int d_3byteEndCalls;
  static unsigned int d_2byteEndCalls;
  static unsigned int d_1byteEndCalls;
};

union ByteMoveHelper {                                                                                                               
  htrie_word  word;                                                                                                   
  htrie_uint  uint[2];                                                                                                
  htrie_sword sword[4];                                                                                               
  htrie_byte  byte[8];                                                                                                
};

extern htrie_word (*benchmark_htrie_byteMove[9])(const htrie_byte *);
extern htrie_word (*benchmark_htrie_byteMovePrefix[8])(const htrie_byte *, htrie_byte endBit);

template<htrie_size N>
class BitString {
  // DATA
  htrie_size d_size;    // size of string in bits
  htrie_byte d_data[N]; // backing array

  // STATIC ASSERT
  static_assert(sizeof(d_data)>0);

public:
  // CREATORS
  BitString();
    // Construct an empty BitString with a capacity of N bytes or 8N bits

  BitString(const std::initializer_list<bool>& data);
    // Construct an empty BitString then append each bit from specified
    // initializer list 'data' up to the capacity which ever comes first
 
  explicit BitString(const char *data);
    // Construct an empty BitString then append at most N bits from specified
    // 'data'. Each character in data is either '0' (ASCII 48) or not. If '0'
    // is found, a zero bit is appened otherwise a 1 bit. 'data' is scanned
    // from 'data+0' one byte at a time until data[i]=0 e.g. the C string zero
    // terminator is seen or the capacity was reached whichever comes first. 

  explicit BitString(htrie_word data);
    // Construct a BitString containing a copy of specified 'data'. The
    // behavior is defined provided template parameter N equals sizeof(data).
    // 'size()==capacity()' on return

  explicit BitString(htrie_uint data);
    // Construct a BitString containing a copy of specified 'data'. The
    // behavior is defined provided template parameter N equals sizeof(data).

  explicit BitString(htrie_byte data);
    // Construct a BitString containing a copy of specified 'data'. The
    // behavior is defined provided template parameter N equals sizeof(data).
    // 'size()==capacity()' on return

  explicit BitString(htrie_sword data);
    // Construct a BitString containing a copy of specified 'data'. The
    // behavior is defined provided template parameter N equals sizeof(data).
    // 'size()==capacity()' on return

  BitString(const BitString& other);
    // Create BitString object from specified 'other'. Upon return return
    // content, capacity, and lengths are equal.

  ~BitString() = default;
    // Destroy this object

  // ACCESSORS
  htrie_size capacity() const;
    // Return the maximum number of bits this object may hold

  htrie_size size() const;
    // Return the size of this string in bits

  bool operator[](htrie_index i) const;
    // Return the bit at specified index 'i' where 'i' has units of bits. The
    // behavior is defined provided 'i<size()'

  htrie_index longestCommonPrefix(const BitString& rhs, bool *eos);                                                              
    // Set specified 'eos=false' and return a non-negative integer 'i' which
    // represents the offset in bits at which the first bit difference between
    // this, rhs occurs. For example,
    //   'this=10011..., rhs=10100...' -> i=2
    //   'this=00011..., rhs=10100...' -> i=0
    // in which bit position 0 appears far left and higher bits to the right.
    // If there is no bit difference the 'eos=true' and the return value is
    // not defined.

  BitIterator iterator() const;
    // Return an iterator initialized at beginning of this BitString

  // MANIPULATORS
  void append(bool value);
    // Append 'value' to end of bit string. The behavior is defined provided
    // 'size()<capacity()'

  bool operator[](htrie_index i);
    // Return the bit at specified index 'i' where 'i' has units of bits. The
    // behavior is defined provided 'i<size()'

  BitIterator iterator();
    // Return an iterator initialized at beginning of this BitString

  const BitString& operator=(const BitString& rhs);
    // Copy state from specified 'rhs' to this object and return a non-modifable
    // reference to this. Upon return contant, length, capacity are equal

  // PRIVATE MANIPULATORS
public:
  htrie_word byteSuffix(htrie_index i);
    // Return bits from specified 'i' plus all the subsequent bits until the
    // end-of-byte in which 'i' occurs. Behavior is defined provided 'i%8!=0'
    // and 'i<size()'. Bit 'i' is placed into bit-position-0 in result, 'i+1'
    // into bit-position 1 in result, and so on

  htrie_word bytePrefix(htrie_index start, htrie_index end, htrie_size n);
    // Return prefix bits in a byte from bit 'start' on a byte boundary through
    // 'end' inclusive in the same byte. The behavior is defined provided:
    //   * 'start%8==0'
    //   * 'start<=end'
    //   * 'start<size()'
    //   * 'end<size()'
    //   * 'end-start<7'
    //   * 'start, end in same byte of backing data'
    //   * 'n<=63-7 so shift does not truncate the return value
    // Bit 'start' is placed into bit-position-0 plus n in result, 'start+1'
    // into bit-position 1+n in result, and so on. Note that range notation
    // this means returns all bits b in '[start, end]' left shifted n. Note
    // that 'end' cannot refer to bit position 7 (value 128) since the prefix
    // would simplify to the entire byte. Another routine handles that case.

  htrie_word substring(htrie_index start, htrie_index end);
    // Return all bits b in '[start, end]' in one byte where 'start, end' does
    // not start on bit-position-0 or end on bit-position-7. Behavior is defined
    // provided:
    //   * 'start%8!=0'
    //   * 'start<=end'
    //   * 'start<size()'
    //   * 'end<size()'
    //   * 'start, end in same byte of backing data'
    //   * 'end-start<6'
    // Bit 'start' is placed into bit-position-0 in result, 'start+1' into bit
    // position 1 in result, and so on

  htrie_word nextWord(htrie_index start, htrie_index end);
    // Return at most 64 bits '[start,end]'. Behavior is defined provided:
    //   * start<size()
    //   * start<=end
    //   * end<size()
    //   * end-start+1<=64
    // Bit 'start' is placed into bit-position-0 in result, 'start+1' into bit
    // position 1 in result, and so on

  htrie_word optNextWord(htrie_index start, htrie_index end);
    // Return at most 64 bits '[start,end]'. Behavior is defined provided:
    //   * start<size()
    //   * start<=end
    //   * end<size()
    //   * end-start+1<=64
    // Bit 'start' is placed into bit-position-0 in result, 'start+1' into bit
    // position 1 in result, and so on. Note, this method attempts to be an
    // optimized version of 'nextWord'

  // ASPECTS
public:
  std::ostream& print(std::ostream& stream) const;
    // Pretty-print this object to specified stream
};

// FREE OPERATORS
template<htrie_size N>
inline
std::ostream& operator<<(std::ostream& stream, const BitString<N>& object);
    // Print into specified 'stream' a human readable dump of 'object' returning 'stream'

// INLINE DEFINITIONS

// CREATORS
template<htrie_size N>
inline
BitString<N>::BitString()
: d_size(0)
{
  memset(d_data, 0, N);
}

template<htrie_size N>
inline
BitString<N>::BitString(const std::initializer_list<bool>& data)
: d_size(0)
{
  memset(d_data, 0, N);
  for (auto iter = data.begin(); iter!=data.end(); ++iter) {
    append(*iter);
  }
}

template<htrie_size N>
inline
BitString<N>::BitString(const char *data)
: d_size(0)
{
  assert(data);
  memset(d_data, 0, N);
  for (htrie_index i=0; data[i]; ++i) {
    if (size()<capacity()) {
      append(data[i]!='0');
    }
  }
}

template<htrie_size N>
inline
BitString<N>::BitString(htrie_word data) {
  static_assert(N==sizeof(htrie_word));
  memcpy(d_data, &data, sizeof(htrie_word));
  d_size = 64;
}

template<htrie_size N>
inline
BitString<N>::BitString(htrie_uint data) {
  static_assert(N==sizeof(htrie_uint));
  memcpy(d_data, &data, sizeof(htrie_uint));
  d_size = 32;
}

template<htrie_size N>
inline
BitString<N>::BitString(htrie_byte data) {
  static_assert(N==sizeof(htrie_byte));
  d_data[0] = data;
  d_size = 8;
}

template<htrie_size N>
inline
BitString<N>::BitString(htrie_sword data) {
  static_assert(N==sizeof(data));
  d_data[0] = data & 0xff;
  d_data[1] = (data & 0xff00)>>8;
  d_size = 16;
}

template<htrie_size N>
inline
BitString<N>::BitString(const BitString& other)
: d_size(other.d_size)
{
  memcpy(d_data, other.d_data, N);
}

// ACCESSORS
template<htrie_size N>
inline
htrie_size BitString<N>::capacity() const {
  return sizeof(d_data)<<3;
}

template<htrie_size N>
inline
htrie_size BitString<N>::size() const {
  return d_size;
}

template<htrie_size N>
inline
bool BitString<N>::operator[](htrie_index i) const {
  assert(i<size());
  return d_data[i>>3] & (1<<(i&7));
}

template<htrie_size N>
inline
BitIterator BitString<N>::iterator() const {
  return BitIterator(size(), d_data);
}

// MANIPULATORS
template<htrie_size N>
inline
void BitString<N>::append(bool value) {
  assert(size()<capacity());
  if (value) {
    htrie_byte byte = d_data[d_size>>3];
    byte |= (1<<(d_size&7));
    d_data[d_size>>3] |= byte;
  }
  ++d_size;
}

template<htrie_size N>
inline
bool BitString<N>::operator[](htrie_index i) {
  assert(i<size());
  return d_data[i>>3] & (1<<(i&7));
}

template<htrie_size N>
inline
BitIterator BitString<N>::iterator() {
  return BitIterator(size(), d_data);
}

template<htrie_size N>
inline
const BitString<N>& BitString<N>::operator=(const BitString<N>& rhs) {
  d_size = rhs.d_size;
  memcpy(d_data, rhs.d_data, N);
  return *this;
}

template<htrie_size N>
HTRIE_ALWAYS_INLINE
htrie_word BitString<N>::byteSuffix(htrie_index i) {
  assert(i&7);
  assert(i<d_size);

#ifndef NDEBUG
  ++BitStringStats::d_byteSuffixCalls;
#endif

  return (d_data[i>>3]&254)>>(i&7);
}

template<htrie_size N>
HTRIE_ALWAYS_INLINE
htrie_word BitString<N>::bytePrefix(htrie_index start, htrie_index end, htrie_size n) {
  assert((start&7)==0);
  assert(start<=end);
  assert(start<d_size);
  assert(end<d_size);
  assert(end-start<7);
  assert((start>>3)==(end>>3));
  assert(n<(63-7));

#ifndef NDEBUG
  ++BitStringStats::d_bytePrefixCalls;
#endif

  //                   +-------+          +----------------+         +-----+
  //                  /  byte   \        /    bits needed   \       / shift |
  return (htrie_word(d_data[start>>3]) & (~(0xff<<(end-start+1)))) <<    n;
}

template<htrie_size N>
HTRIE_ALWAYS_INLINE
htrie_word BitString<N>::substring(htrie_index start, htrie_index end) {
  assert((start&7)!=0);
  assert(start<=end);
  assert(start<d_size);
  assert(end<d_size);
  assert((start>>3)==(end>>3));
  assert(end-start<6);

#ifndef NDEBUG
  ++BitStringStats::d_substringCalls;
#endif

  const htrie_byte shft   =  start&7;
  const htrie_byte endBit =  (end&7)+1;
  const htrie_byte byte   =  d_data[start>>3];
  const htrie_byte hiMask = ~(0xff<<endBit);
  const htrie_byte loMask =  0xff<<shft;

  return (byte&loMask&hiMask)>>shft;
}

template<htrie_size N>
inline
htrie_word BitString<N>::nextWord(htrie_index start, htrie_index end) {
  assert(start<d_size);
  assert(end<d_size);
  assert(start<=end);
  assert(end-start+1<=64);

  htrie_word ret(0);
  htrie_word shift(0);

  union {
    htrie_word  word;
    htrie_uint  uint[2];
    htrie_sword sword[4];
    htrie_byte  byte[8];
  } retHelper;

  htrie_index endByte(end>>3);
  htrie_index startByte(start>>3);

  if (start&7) {
    if (endByte!=startByte) {
      // advance to byte boundary
      ret = byteSuffix(start);

      // This is the number of bits 'byteSuffix' handled to move up one byte
      shift = (8-(start&7));
      assert(shift>0 && shift<=8);

      // Ensure start+shift still in valid memory
      assert((start+shift)<d_size);
      // We're now on a byte boundary at next byte. Check that:
      assert(((start+shift)&7)==0);
      // Ensure advanced to next byte
      assert(((start+shift)>>3)==((start>>3)+1));

      // If there's not a full byte left tack on prefix and return now
      if (end-start-shift<=6) {
        return ret | bytePrefix(start+shift, end, 8-(start&7));
      }

      // Fall into block case aka 'middle' subcase after fixing start
      start += shift;
    } else if ((end&7)!=7) {
      return substring(start, end);
    } else {
      return byteSuffix(start);
    }
  } else if (endByte==startByte && (end&7)!=7) {
    return bytePrefix(start, end, 0);
  }

  // Must be on a byte boundary
  assert((start&7)==0);

  // Number of bits left to deal with
  const htrie_index left = end-start+1;
  assert(left>0);

  if ((left&7)==0) {
    assert((left>=8)&&(left<=64));

    if (left==64) {
      assert(ret==0);
      assert(shift==0);
      assert((start>>3)==startByte);
      return *(reinterpret_cast<htrie_word*>(d_data+startByte));
    }

    // Update startByte
    startByte = start>>3;
    // Init helper
    retHelper.word = 0;

    if (left==56) {
      retHelper.uint[0] = *reinterpret_cast<htrie_uint*>(d_data+startByte);
      retHelper.byte[4] = d_data[startByte+4];
      retHelper.byte[5] = d_data[startByte+5];
      retHelper.byte[6] = d_data[startByte+6];
      ret |= (retHelper.word<<shift);
    } else if (left==48) {
      retHelper.uint[0] = *reinterpret_cast<htrie_uint*>(d_data+startByte);
      retHelper.byte[4] = d_data[startByte+4];
      retHelper.byte[5] = d_data[startByte+5];
      ret |= (retHelper.word<<shift);
    } else if (left==40) {
      retHelper.uint[0] = *reinterpret_cast<htrie_uint*>(d_data+startByte);
      retHelper.byte[4] = d_data[startByte+4];
      ret |= (retHelper.word<<shift);
    } else if (left==32) {
      ret |= htrie_word(*reinterpret_cast<htrie_uint*>(d_data+startByte)) << shift;
    } else if (left==24) {
      retHelper.sword[0] = *reinterpret_cast<htrie_sword*>(d_data+startByte);
      retHelper.byte[2] = d_data[startByte+2];
      ret |= (retHelper.word<<shift);
    } else if (left==16) {
      ret |= htrie_word(*reinterpret_cast<htrie_sword*>(d_data+startByte)) << shift;
    } else { // left==8
      ret |= htrie_word(d_data[startByte]) << shift;
    }
  } else {
    assert((left>8)&&(left<64));
    assert(left&7);

    // Update startByte
    startByte = start>>3;
    // Init helper
    retHelper.word = 0;

    if (left>56) {
      retHelper.uint[0] = *reinterpret_cast<htrie_uint*>(d_data+startByte);
      retHelper.sword[2] = *reinterpret_cast<htrie_sword*>(d_data+startByte+4);
      retHelper.byte[6] = d_data[startByte+6];
      retHelper.byte[7] = bytePrefix(start+56, end, 0);
      ret |= (retHelper.word<<shift);
    } else if (left>48) {
      retHelper.uint[0] = *reinterpret_cast<htrie_uint*>(d_data+startByte);
      retHelper.sword[2] = *reinterpret_cast<htrie_sword*>(d_data+startByte+4);
      retHelper.byte[6] = bytePrefix(start+48, end, 0);
      ret |= (retHelper.word<<shift);
    } else if (left>40) {
      retHelper.uint[0] = *reinterpret_cast<htrie_uint*>(d_data+startByte);
      retHelper.byte[4] = d_data[startByte+4];
      retHelper.byte[5] = bytePrefix(start+40, end, 0);
      ret |= (retHelper.word<<shift);
    } else if (left>32) {
      retHelper.uint[0] = *reinterpret_cast<htrie_uint*>(d_data+startByte);
      retHelper.byte[4] = bytePrefix(start+32, end, 0);
      ret |= (retHelper.word<<shift);
    } else if (left>24) {
      retHelper.sword[0] = *reinterpret_cast<htrie_sword*>(d_data+startByte);
      retHelper.byte[2] = d_data[startByte+2];
      retHelper.byte[3] = bytePrefix(start+24, end, 0);
      ret |= (retHelper.word<<shift);
    } else if (left>16) {
      retHelper.sword[0] = *reinterpret_cast<htrie_sword*>(d_data+startByte);
      retHelper.byte[2] = bytePrefix(start+16, end, 0);
      ret |= (retHelper.word<<shift);
    } else { // left>8
      retHelper.byte[0] = d_data[startByte];
      retHelper.byte[1] = bytePrefix(start+8, end, 0);
      ret |= (retHelper.word<<shift);
    }
  }

  return ret;
}

template<htrie_size N>
inline
htrie_word BitString<N>::optNextWord(htrie_index start, htrie_index end) {
  assert(start<d_size);
  assert(end<d_size);
  assert(start<=end);
  assert(end-start+1<=64);

  htrie_index startByte(start>>3);
  const htrie_index endByte(end>>3);

  if (endByte==startByte) {
    const htrie_byte shift  =  start&7;
    const htrie_byte endBit =  (end&7)+1;
    const htrie_byte hiMask = ~(0xff<<endBit);
    const htrie_byte loMask =  0xff<<shift;
    return (d_data[startByte]&loMask&hiMask)>>shift;
  }

  const bool startOnByte = (start&7)==0;
  const bool moveBytes   = ((end-start+1)&7)==0;

  if (startOnByte) {
      if (moveBytes) {
        return benchmark_htrie_byteMove[(end-start+1)>>3](d_data+startByte);
      } else {
        return benchmark_htrie_byteMovePrefix[((end-start)>>3)&7](d_data+startByte, (end&7)+1);
      }
  }

  // Slurp up bits in suffix of first byte
  htrie_word ret = byteSuffix(start);

  // This is the number of bits 'byteSuffix' handled to move up one byte
  htrie_word shift = (8-(start&7));
  assert(shift>0 && shift<=8);

  // Ensure start+shift still in valid memory
  assert((start+shift)<d_size);
  // We're now on a byte boundary at next byte. Check that:
  assert(((start+shift)&7)==0);
  // Ensure advanced to next byte
  assert(((start+shift)>>3)==((start>>3)+1));

  // If there's not a full byte left tack on prefix and return now
  if (end-start-shift<=6) {
    return ret | bytePrefix(start+shift, end, 8-(start&7));
  }

  // Advance start to account for 'byteSuffix' work
  start += shift;
  // Must be on a byte boundary here
  assert((start&7)==0);

  // Number of bytes left to deal with
  const htrie_index left = end-start+1;
  assert(left>0);

  if ((left&7)==0) {
    return ret | (benchmark_htrie_byteMove[left>>3](d_data+(start>>3)) << shift);
  } else {
    return ret | (benchmark_htrie_byteMovePrefix[((left-1)>>3)&7](d_data+(start>>3), (end&7)+1) << shift);
  }
}

// ASPECTS
template<htrie_size N>
inline
std::ostream& BitString<N>::print(std::ostream& stream) const {
  // stream << "\"BitString\": {"                        << std::endl;
  // stream << "  \"data\"         : " << (void*)(d_data)<< std::endl;                                             
  // stream << "  \"capacityBits\" : " << capacity()     << std::endl;
  // stream << "  \"sizeBits\"     : " << size()         << std::endl;
  // stream << "  \"value\"        : '";
  for (htrie_index i=0; i<d_size; ++i) {
    char ch = (d_data[i>>3] & (1<<(i&7))) ? '1' : '0';
    stream << ch;
  }
  // stream << "'" << std::endl;
  // stream << "}" << std::endl;
  stream << std::endl;
  return stream;
}

template<htrie_size N>
inline
std::ostream& operator<<(std::ostream& stream, const BitString<N>& object) {
  return object.print(stream);
}

} // namespace HTrie
} // namespace Benchmark
