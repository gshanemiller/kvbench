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

// BitString has the same intent of std::vector<bool> except BitString has
// fixed memory capacity with a smaller API. Storage is set at compile time
// through a template parameter N (unit bytes). BitString capacity is then 
// 8N bits and 'size()' ranges from 0 (empty) to 'capacity()' (full). Typical
// BitString construction examples:
//
// (*) BitString<10> bs0;                 // empty, capacity 80bits
// (*) BitString<20> bs1 = {false, true}; // 160 bit capacity with 2 bits {0,1}
//
// Read/write operations:
//
// (*) bool bit = bs1[1];                 // read and return bit 1
// (*) bs1.append(false);                 // append to end-of-string 'false/0'
//
// Aspects:
//
// (*) bs1.print(std::cout);              // pretty print string
// (*) BitIterator iter = bs1.iterator(); // read-only iterator on 'bs1'
//
// Trie Helpers:
//
// Patricia, HOT tries store some or all of its data as bit strings. A crucial
// requirement is to find bit differences or similarities between two strings.
// BitString has two helpers (descriptions elided):
//
// (*) btrie_word  nextWord(htrie_index i, htrie_len bitLen);
//      // Extract and return the 'bitLen' bits starting at bit index i.
//
// (*) btrie_index longestCommonPrefix(const BitString& rhs, bool *eos)
//      // Set 'eos=false' and return the index of the first bit difference 
//      // between this object and 'rhs' or set 'eos=true' if there are no
//      // differences
//
// Conceptually, the longest common prefix between two strings are all the bits
// i in '[0, n)' in which lhs[i]==rhs[i] for the largest value of n so that
// lhs[n]!=rhs[n]. Example: lhs="ababc", rhs="abazz" -> "aba" so n=3.
//
// Finding this prefix is based on 'nextWord' which extracts the next 'bitLen'
// bits from a BitString. lhs/rhs words can be efficiently compared to find 'n'
// and hence the prefix. By repeatedly calling nextWord the longest prefix can
// be found. The basic challenges of nextWord are two:
//
// (*) Storage is byte based but indexing is bit based
// (*) Reading more bits than requested
//
// nextWord breaks the problem into three parts: start, middle, and end. Start
// reads the next few bits 'f' so that i+f is on a byte boundary. The middle is
// processing blocks of 8-bits. The end deals with any remaining bits that are
// left. Depending on 'i, bitLen' specified in the nextWord call, the algorithm
// could leave the call in the start, middle, or end part of the code.
//
// Since the middle part is vanilla array manipulation, consider part one. The
// start index may be on a byte boundary or not. To efficiently check which
// case '(i&7)' will be true when 'i' is not a multiple of of 8. Assume i is
// not on a byte boundary. 'd_data[i>>3]' will contain all the bits we want plus
// others we do not want. To get first few the bits requested, we must mask off
// the other bits not requested then shift what remains into bit-position-0 of
// the the result. If this is done correctly, the first 'f' bits will appear in
// the result starting at bit position 0. 'i+f' will fall on a byte boundary.
//
// The number of bits f we need in 'd_data[i>>3]' is equal to 'i%8==i&7'. This
// must be true since f is the remainder after dividing i by 8 whence '(i+f)%8'
// is zero by definition of mod-8. The starting position 'p' of those 'f' bits
// in 'd_data[i>>3]' is 8-f. Example on 80bit string with i=59:
//
// (*) 59&7!=0 so i not on an 8-byte boundary
// (*) 59%8==(59&7)=3 and 8-3 = 5. Therefore we need 5 bits at pos 59-63:
//
//                          First 8 bits of result
//                +-----+-----+-----+-----+-----+-----+-----+-----+
//                |  0  |  1  |  2  |  3  |  4  |  5  |  6  |  7  |
//                +-----+-----+-----+-----+-----+-----+-----+-----+
//                  /------------------------\
//                   \   bit 59              +--bit-63--------+
//                    \____________                           |
//                                 \      We want these bitsa V
// Byte 0          Byte 7           >/---------------------------\
// +-----+        +-----+-----+-----+-----+-----+-----+-----+-----+
// |  0  |   ...  | 56  | 57  | 58  | 59  | 60  | 61  | 62  | 63  |
// +-----+        +-----+-----+-----+-----+-----+-----+-----+-----+
//                 bit position-0                            pos-7
//
// The mask we need to capture bits is 00011111b = 248. By observing that if
// we initialize a mask value to 0xff (so all 1 bits) then left-shift it by
// 3 bits (i&7) it'll roll on 3 zeros in the lower 3 bits leaving everything
// else 1. This is the mask we want. From here it's down hill.
//
// Example:
// ------------------------------
// Suppose 'd_data[i>>3]==255' in byte 7. Extracting bits 59-63 into the result
// starting at bit-position-0 through bit 4 will give 31 because 11111b is 31:
// 
// 1. indx = 59                 = 59
// 2. byte = d_data[indx>>3]    = 255
// 3. shft = (59&7)             = 3
// 4. mask = (0xff<<sfht)       = 248         // enables all bit 59 onwards
// 5. byte&mask                 = 248 
// 6. (byte&mask)>>shft         = 31
//
// The only problem with this algorithm is that it may read beyond the end-of-
// string. If the bit-string is 62-bits long so bits [0,61] are defined this
// algorithm will include bits 62,63 in result position 3,4. Bits 62,63
// could contain junk data, and so could the result. In the alternative the
// end-of-string might be well past bit position 63 however the caller really
// did not want all the bits in the end of the byte. Caller may only want bits
// through 61 based on the 'i=59, bitLen=3' specified in the call to nextWord.
// Note, this algorithm cannot and does not read invalid memory. BitString
// allocates backing memory in byte sized chunks, and the byte in step (2)
// above is a valid byte in the backing memory. Also note that if the algorithm
// does not read beyond the end-of-string, there is no need to memset the
// backing memory to 0 in constructors since reads can only see validly written
// bits.
//
// The number of bits the algorithm copies into result in this example so far
// is '8-(59^7)=5'. So if '5>bitLen' the result will have to be remasked. The
// number of bits to be masked on in result is just 'bitLen' bits. The actual
// implementation fixes the mask with roll-on-zero hac fixing the mask. The
// final algorithm follows:
// while hiMask turns off bits **after** i+bitLen-1:
//
// 1. indx    = 59                 = 59
// 2. bitLen  = 3                  = 3   // get bits 59,60,61
// 3. byte    = d_data[indx>>3]    = 255
// 4. shft    = (59&7)             = 3   // bit 59 starts at bit 3 in byte 
// 5. endBit  = (59&7)+3           = 6   // bit 62 starts at bit 6 in byte
// 6. lowMask =  (0xff<<sfht)      = 248 // mask for all bits i to end-of-byte
// 7. hiMask  = ~(0xff<<endBit)    = 63  // mask for all bits >= endBit
// 8. byte & lowMask & hiMask      = 56  // bits 59,60,61 isolated
// 9. (byte&lowMask&hiMask)>>shft  = 7   // final answer
//
//            -bit 7   -bit 0
//           /        /
// byte   :  1111 1111            255
// lowMask:  1111 1000            248
// hiMask :  0011 1111            63
// -------------------------------------- AND
//           0011 1000            56
// -------------------------------------- right shift 3
//                 111            7

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
    // equivalent to 'i&7!=0', and 'i<size()'. Bit 'i' is placed into
    // bit-position-0 in result, 'i+1' into bit-position 1 in result, and so on.
    // This is 'nextWord' start sub-case when all bits from i onwards in i's
    // byte are required

  htrie_word bytePrefix(htrie_index i, htrie_len bitLen, htrie_size shift);
    // Return all bits b in '[i,i+bitLen)' left shifted by 'shift'. Behavior is
    // defined provided all of the following are true:
    //   * 'i%8==0' equivalent to 'i&7==0'
    //   * 'bitLen>0',
    //   * 'i' and 'i+bitLen' are <= size()
    //   * byte holding bit 'i, i+bitLen' both in byte e.g. 'bitLen' in [0,7] 
    //   * shift<=63-7: so prefix of <=7 bits cannot shift off end of result 
    // Bit 'i' is set into bit-position-0 plus shift in result, 'i+1' into bit
    // position 1+shift in result, and so on. Note that 'shift' is required 
    // because prefix bits are often bitwise-OR with a previous result. The
    // shift ensures bits are concatenated correctly

  htrie_word substring(htrie_index i, htrie_len bitLen);
    // Return all bits b in '[i,i+bitLen)'. Behavior is defined provided
    // 'i%8!=0' equivalent to 'i&7!=0', 'bitLen>0', 'i+bitLen' is in the
    // same byte as is 'i', plus 'i<size() && (i+bitLen)<=size()'. Bit 'i' is
    // placed into bit-position-0 in result, 'i+1' into bit-position 1 in result,
    // and so on. This is 'nextWord' sub-case when bits '[i,i+bitLen)' are in
    // the same byte and 'i+bitLen<8' so 'byteSuffix' does not apply.

  htrie_word nextWord(htrie_index i, htrie_len bitLen);
    // Return all bits b in '[i,i+bitLen)'. Behavior is defined provided
    // 'i<size() && i+bitLen<size()' plus 'bitLen<=64'. Bit 'i' is placed into
    // bit-position-0 in result, 'i+1' into bit-position 1 in result,                                       
    // and so on. This routine defers to helpers in certain cases.

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
htrie_word BitString<N>::substring(htrie_index i, htrie_len bitLen) {
  assert(i&7);
  assert(bitLen>0);
  assert(i<d_size);
  assert((i+bitLen)<=d_size);

#ifndef NDEBUG
  ++BitStringStats::d_substringCalls;
#endif

  // We get 'bitLen' bits e.g. if bitLen=3 get i,i+1,i+2
  assert((i>>3)==((i+bitLen-1)>>3));

  const htrie_byte shft   =  (i&7);
  const htrie_byte endBit =  (shft+bitLen);
  const htrie_byte byte   =  d_data[i>>3];
  const htrie_byte loMask =  (0xff<<shft);
  const htrie_byte hiMask = ~(0xff<<endBit);

  return (byte&loMask&hiMask)>>shft;
}

template<htrie_size N>
HTRIE_ALWAYS_INLINE
htrie_word BitString<N>::bytePrefix(htrie_index i, htrie_len bitLen, htrie_size shift) {
  assert((i&7)==0);
  assert(bitLen>0);
  assert(bitLen<=7);
  assert(i<d_size);
  assert((i+bitLen)<=d_size);
  // We get 'bitLen' bits e.g. if bitLen=3 get i,i+1,i+2
  assert((i>>3)==((i+bitLen-1)>>3));
  assert(shift<(63-7));

#ifndef NDEBUG
  ++BitStringStats::d_bytePrefixCalls;
#endif

  //                  +-------+     +-----------+       +-----+
  //                 /  byte   \   / bits needed \     / shift |
  return htrie_word(d_data[i>>3] & (~(0xff<<bitLen))) << shift;
}

template<htrie_size N>
inline
htrie_word BitString<N>::nextWord(htrie_index i, htrie_len bitLen) {
  assert(i<d_size);
  assert(bitLen>0);
  assert((i+bitLen)<=d_size);

  htrie_word ret(0);

  // Are we NOT starting on a byte-boundary aka "start" sub-case
  if (i&7) {
    if (bitLen>=7) {
      // advance to byte boundary
      ret = byteSuffix(i);

      // This is the number of bits 'byteSuffix' handled to move up one byte
      const htrie_index delta = (8-(i&7));
      assert(delta>=0 && delta<8);

      // Ensure i+delta still in valid memory
      assert((i+delta)<=size());
      // We're now on a byte boundary at next byte. Check that:
      assert(((i+delta)&7)==0);
      // Ensure advanced to next byte
      assert(((i+delta)>>3)==((i>>3)+1));

      // Update bitLen
      bitLen -= delta;

      // If there's only <8 more bits left tack those on now and return.
      // The shift term ensures these remaining bits are placed after whatever
      // is already is in ret so they're contiguous in ret
      if (bitLen && bitLen<8) {
        return ret | bytePrefix(i+delta, bitLen, 8-(i&7));
      } else if (bitLen==0) {
        // No more bits to process: we're done
        return ret;
      }
      // Fall into block case aka 'middle' subcase
    } else {
      return substring(i, bitLen);
    }
  } else if (bitLen<8) {
    return bytePrefix(i, bitLen, 0);
  }

  assert(bitLen>0);

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
