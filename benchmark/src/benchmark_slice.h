#pragma once

// PURPOSE: Hold reference to an unowned memory array for read-only

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#include <string>

#include <trie/benchmark_htrie_bititerator.h>

namespace Benchmark {

template<typename T>
class Slice {
  // DATA
  const unsigned int d_size; // size of string including 0 terminator
  const T           *d_data; // pointer to array of type 'T*' (not owned)

public:
  // CREATORS
  Slice() = delete;
    // Default constructor not provided

  explicit Slice(unsigned int sz, const T *data);
    // Create Slice from specified 'data' of specified 'sz'. Behavior is defined provided 'data' non-zero, and the
    // memory range '[d_data, d_data+d_size)' is valid, contiguous over the lifetime of this Slice.

  explicit Slice(const std::string& data);
    // Create Slice from specified 'data'. 'data' lifetime must equal or exceed this Slice.

  Slice(const Slice& other) = default;
    // Copy constructor

  ~Slice() = default;
    // Destroy this object

  // ACCESSORS
  unsigned int size() const;
    // Return 'size' attribute

  const T *data() const;
    // Return 'data' attribute

  HTrie::BitIterator iterator() const;
    // Return an iterator on this object initialized to start of memory

  // MANIPULATORS
  void reset(unsigned int sz, const T *data);
    // Reset 'this' to hold a pointer to memory at specified 'data' of specific 'sz'. Behavior is defined provided
    // 'data' non-zero, and the memory range '[d_data, d_data+d_size)' is valid, contiguous. 

  HTrie::BitIterator iterator();
    // Return an iterator on this object initialized to start of memory

  Slice& operator=(const Slice& rhs) = default;
    // Assignment operator

  // ASPECTS
  void print() const;
    // Pretty-print this object to stdout followed by newline.
};

// FREE OPERATORS
template<class T>
bool operator==(const Slice<T>& lhs, const Slice<T>& rhs);

// INLINE DEFINITIONS
// CREATORS
template<class T>
inline
Slice<T>::Slice(unsigned int sz, const T *data)
: d_size(sz)
, d_data(data)
{
  assert(sz>0);
  assert(data);
}

template<class T>
inline
Slice<T>::Slice(const std::string &data)
: d_size(data.length())
, d_data(reinterpret_cast<const T *>(data.c_str()))
{
  assert(sz>0);
  assert(data);
}

// ACCESSORS
template<class T>
inline
unsigned int Slice<T>::size() const {
  return d_size;
}

template<class T>
inline
const T *Slice<T>::data() const {
  return d_data;
}

template<class T>
inline
HTrie::BitIterator Slice<T>::iterator() const {
  return HTrie::BitIterator(d_size<<3, d_data);
}

// MANIPULATORS
template<class T>
inline
void Slice<T>::reset(unsigned int sz, const T *data) {
  assert(sz>0);
  assert(data);
  d_size = sz;
  d_data = data;
}

template<class T>
inline
HTrie::BitIterator Slice<T>::iterator() {
  return HTrie::BitIterator(d_size<<3, d_data);
}

// ASPECTS
template<>
inline
void Slice<char>::print() const {
  if (d_data!=0) {
    printf("Slice<char> size: %u, data: '", d_size);
    const char *start(d_data);
    for(unsigned i=0; i<d_size; ++i, ++start) {
      if (isprint(*start)) {
        putchar(*start);
      } else {
        printf("0x%02x", *start);
      }
    }
    printf("'\n");
  } else {
    printf("Slice<char> size: %u, data: *NULL*\n", d_size);
  }
}

// FREE OPERATORS
template<class T>
inline
bool operator==(const Slice<T>& lhs, const Slice<T>& rhs) {
  return (lhs.size()==rhs.size()) ? 0==memcmp(lhs.data(), rhs.data(), lhs.size()) : false;
}

} // namespace Benchmark
