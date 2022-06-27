#pragma once

// PURPOSE: Represent an array of elements of type T
//
// CLASSES:
//  Benchmark::Slice: Hold a pointer with size to an array of type 'T*'.

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#include <benchmark_typedefs.h>

namespace Benchmark {

template<typename T>
class Slice {
  // DATA
  T     *d_data; // pointer to array of type 'T*'
  ssize  d_size; // size of string including 0 terminator

public:
  // CREATORS
  Slice();
    // Create empty slice

  explicit Slice(T *data, ssize size);
    // Create Slice from specified 'data' of specified 'sz'. Behavior is defined provided 'data' non-zero, and the
    // memory range '[d_data, d_data+d_size)' is valid, contiguous.

  Slice(const Slice& other) = default;
    // Copy constructor

  ~Slice() = default;
    // Destroy this object. 'd_data' not freed.

  // ACCESSORS
  ssize size() const;
    // Return 'size' attribute

  const T *data() const;
    // Return 'data' attribute

  // MANIPULATORS
  ssize size();
    // Return 'size' attribute

  T *data();
    // Return 'data' attribute

  void reset(ssize sz, T* data);
    // Reset 'this' to hold a pointer to memory at specified 'data' of specific 'sz'

  Slice& operator=(const Slice& rhs) = default;
    // Assignment operator

  // ASPECTS
  void print() const;
    // Pretty-print this to stdout followed by newline.
};

// FREE OPERATORS
template<class T>
bool operator==(const Slice<T>& lhs, const Slice<T>& rhs);

// INLINE DEFINITIONS
template<class T>
inline
Slice<T>::Slice()
: d_size(0)
, d_data(0)
{
}

template<class T>
inline
Slice<T>::Slice(T *data, ssize size)
: d_data(data)
, d_size(size)
{
  assert(size>0);
  assert(data);
}

// ACCESSORS
template<class T>
inline
ssize Slice<T>::size() const {
  return d_size;
}

template<class T>
inline
const T *Slice<T>::data() const {
  return d_data;
}

// MANIPULATORS
template<class T>
inline
ssize Slice<T>::size() {
  return d_size;
}

template<class T>
inline
T *Slice<T>::data() {
  return d_data;
}

template<class T>
inline
void Slice<T>::reset(ssize sz, T* data) {
  d_size = sz;
  d_data = data;
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

// KeyEqual helper for Hashing functions
template<class T>
struct SliceEqual {
  bool operator()(const T &lhs, const T &rhs) const {
    return (lhs.size()==rhs.size()) ? 0==memcmp(lhs.data(), rhs.data(), lhs.size()) : false;
  }
};

} // namespace Benchmark
