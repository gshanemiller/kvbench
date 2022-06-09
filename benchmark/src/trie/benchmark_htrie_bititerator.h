#pragma once

// PURPOSE: Iterate over an array bit by bit
//          
// CLASSES:
//          Benchmark::HTrie::BitIterator: Iterate over contiguous array like memory one bit at a time

#include <trie/benchmark_htrie_typedefs.h>

#include <iostream>
#include <assert.h>

namespace Benchmark {
namespace HTrie     {

class BitIterator {
  // DATA
  const htrie_byte *d_data;    // pointer to data provided in constructor
  htrie_size        d_size;    // size of data in bits provided in constructor
  htrie_index       d_index;   // index of current bit

public:
  // CREATORS
  BitIterator() = delete;
    // Default constructor not provided

  explicit BitIterator(htrie_size bitSize, const htrie_byte *data);
    // Create BitIterator to iterate over specified 'data' one bit at a time.
    // The behavior is defined provided '[data, data+bitSize)' is valid,
    // contiguous memory.

  explicit BitIterator(const BitIterator& other);
    // Create BitIterator object from specified 'other'. Upon return return
    // internal state is equal

  ~BitIterator() = default;
    // Destroy this object

  // ACCESSORS
  htrie_size bits() const;
    // Return the total number of bits represented by 'data' provided at
    // construction time

  htrie_index bitIndex() const;
    // Return the 0-based index of the current bit

  bool value() const;
    // Return the value of the bit (true=1, false=0) at 'bitIndex'

  bool end() const;
    // Return true if the end of the bit stream has been reached

  // MANIPULATORS
  void begin();
    // Reset state to bit 0 of the stream

  void next();
    // Advance to the next bit. The behavior is defined provided 'end()==false'

  const BitIterator& operator=(const BitIterator& rhs);
    // Copy state from specified 'rhs' to this object and return a non-modifable
    // reference to this.

  // ASPECTS
  std::ostream& print(std::ostream& stream) const;
    // Pretty-print this object's state to specified stream

  std::ostream& dump(std::ostream& stream) const;
    // Pretty-print bit string over entire object provided at construction time
};

// FREE OPERATORS
std::ostream& operator<<(std::ostream& stream, const BitIterator& object);
    // Print into specified 'stream' a human readable dump of 'object' returning 'stream'

// INLINE DEFINITIONS
inline
BitIterator::BitIterator(htrie_size bitSize, const htrie_byte *data) 
: d_data(data)
, d_size(bitSize)
, d_index(0)
{
  assert(data);
}

inline
BitIterator::BitIterator(const BitIterator& other)
: d_data(other.d_data)
, d_size(other.d_size)
, d_index(other.d_index)
{
}

// ACCESSORS
inline
htrie_size BitIterator::bits() const {
  return d_size;
}

inline
htrie_index BitIterator::bitIndex() const {
  return d_index;
}

inline
bool BitIterator::value() const {
  return d_data[d_index>>3] & (1<<(d_index&7));
}

inline
bool BitIterator::end() const {
  return d_index==d_size;
}

// MANIPULATORS
inline
void BitIterator::begin() {
  d_index=0;
}

inline
void BitIterator::next() {
  ++d_index;
}

inline
const BitIterator& BitIterator::operator=(const BitIterator& rhs) {
  d_data = rhs.d_data;
  d_size = rhs.d_size;
  d_index = rhs.d_index;
  return *this;
}

// INLINE FREE OPERATORS
inline
std::ostream& operator<<(std::ostream& stream, const BitIterator& object) {
  return object.print(stream);
}

} // namespace HTrie
} // namespace Benchmark
