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
};

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
    // Construct an empty BitString with a cpacity of N bytes or 8N bits

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
    // Return all bits b in '[start,end]'. Behavior is defined provided:
    //   * start<size()
    //   * start<=end
    //   * end<size()
    // Bit 'start' is placed into bit-position-0 in result, 'start+1' into bit
    // position 1 in result, and so on

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
}

template<htrie_size N>
inline
BitString<N>::BitString(const std::initializer_list<bool>& data)
: d_size(0)
{
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

  const htrie_byte shft   =  (start&7);
  const htrie_byte endBit =  (end-start+shft+1);
  const htrie_byte byte   =  d_data[start>>3];
  const htrie_byte hiMask =  (0xff<<shft);
  const htrie_byte loMask = ~(0xff<<endBit);

  return (byte&loMask&hiMask)>>shft;
}

template<htrie_size N>
inline
htrie_word BitString<N>::nextWord(htrie_index start, htrie_index end) {
  assert(start<d_size);
  assert(end<d_size);
  assert(start<=end);

  htrie_word ret(0);
 
  const htrie_index endByte(end>>3);
  const htrie_index startByte(start>>3);

  if (start&7) {
    if (endByte!=startByte) {
      // advance to byte boundary
      ret = byteSuffix(start);

      // This is the number of bits 'byteSuffix' handled to move up one byte
      const htrie_index delta = (8-(start&7));
      assert(delta>0 && delta<=8);

      // Ensure start+delta still in valid memory
      assert((start+delta)<d_size);
      // We're now on a byte boundary at next byte. Check that:
      assert(((start+delta)&7)==0);
      // Ensure advanced to next byte
      assert(((start+delta)>>3)==((start>>3)+1));

      // If there's less than 8 bits left tack those on now and return.
      // The shift term ensures these remaining bits are placed after whatever
      // is already is in ret so they're contiguous in ret
      if (end-start-delta<6) {
        return ret | bytePrefix(start+delta, end, 8-(start&7));
      }
      // Fall into block case aka 'middle' subcase after fixing start
      start += delta;
    } else {
      return substring(start, end);
    }
  } else if (endByte==startByte) {
    return bytePrefix(start, end);
  }

  return 0;
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
