#pragma once

// PURPOSE: An array-like container of bits with capacity of at most N-bytes
//          
// CLASSES:
//          Benchmark::HTrie::BitSting

#include <trie/benchmark_htrie_typedefs.h>
#include <trie/benchmark_htrie_bititerator.h>

#include <iostream>
#include <assert.h>
#include <initializer_list>

namespace Benchmark {
namespace HTrie     {

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
 
  BitString(const char *data);
    // Construct an empty BitString then append at most N bits from specified
    // 'data'. Each character in data is either '0' (ASCII 48) or not. If '0'
    // is found, a zero bit is appened otherwise a 1 bit. 'data' is scanned
    // from 'data+0' one byte at a time until data[i]=0 e.g. the C string zero
    // terminator is seen or the capacity was reached whichever comes first. 

  explicit BitString(const BitString& other);
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

  BitIterator iterator() const;
    // Return an iterator on this string initialized at beginning of string

  // MANIPULATORS
  void append(bool value);
    // Append 'value' to end of bit string. The behavior is defined provided
    // 'size()<capacity()'

  bool operator[](htrie_index i);
    // Return the bit at specified index 'i' where 'i' has units of bits. The
    // behavior is defined provided 'i<size()'

  BitIterator iterator();
    // Return an iterator on this string initialized at beginning of string

  void reset();
    // Revert this object to the empty bit string

  const BitString& operator=(const BitString& rhs);
    // Copy state from specified 'rhs' to this object and return a non-modifable
    // reference to this. Upon return contant, length, capacity are equal

  // ASPECTS
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
  memset(d_data, 0, sizeof(d_data));
}

template<htrie_size N>
inline
BitString<N>::BitString(const std::initializer_list<bool>& data)
: d_size(0)
{
  memset(d_data, 0, sizeof(d_data));
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
  memset(d_data, 0, sizeof(d_data));
  for (htrie_index i=0; data[i]; ++i) {
    if (size()<capacity()) {
      append(data[i]!='0');
    }
  }
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
void BitString<N>::reset() {
  memset(d_data, 0, sizeof(d_data));
  d_size = 0;
}

template<htrie_size N>
inline
const BitString<N>& BitString<N>::operator=(const BitString<N>& rhs) {
  d_size = rhs.d_size;
  memcpy(d_data, rhs.d_data, N);
  return *this;
}

// ASPECTS
template<htrie_size N>
inline
std::ostream& BitString<N>::print(std::ostream& stream) const {
  stream << "\"BitString\": {"                        << std::endl;
  stream << "  \"data\"         : " << (void*)(d_data)<< std::endl;                                             
  stream << "  \"capacityBits\" : " << capacity()     << std::endl;
  stream << "  \"sizeBits\"     : " << size()         << std::endl;
  stream << "  \"value\"        : '";
  for (htrie_index i=0; i<d_size; ++i) {
    char ch = (d_data[i>>3] & (1<<(i&7))) ? '1' : '0';
    stream << ch;
  }
  stream << "'" << std::endl;
  stream << "}" << std::endl;
  return stream;
}

template<htrie_size N>
inline
std::ostream& operator<<(std::ostream& stream, const BitString<N>& object) {
  return object.print(stream);
}

} // namespace HTrie
} // namespace Benchmark
