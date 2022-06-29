#pragma once

// PURPOSE: Compactly represent a reference to an array of elements of type T
//          The representation is a bit field of 64 bits: 48 bits to point to
//          the underlying data plus 16 bits for the size. This works because
//          supported hardware is such that all virtual addresses are only 48
//          bits or less per `cpuid`.
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
  union {
    u_int64_t d_data: 48; // pointer to array of type T (not owned)
    u_int64_t d_size: 16; // size of string including 0 terminator if any
    u_int64_t d_opaque;   // all 64-bits
  };

public:
  // CREATORS
  Slice();
    // Create an empty slice having 0 size pointing to address 0

  explicit Slice(T *data, ssize size);
    // Create Slice from specified 'data' of specified 'size'. Behavior is defined provided 'size>0', 'data' non-zero,
    // and the memory range '[d_data, d_data+d_size)' is valid, contiguous. It is also required the upper 16 bits of
    // 'data' are 0. This requirement is trivially met if `cpuid` reports the maximum linear (virtual) address bits
    // is 48 bits or less.

  explicit Slice(void *ptr);
    // Create a Slice from specified 'ptr'. Behavior is defined provided the upper 16 bits of 'ptr' are the slice's
    // size and the lower 48 bits point to the underlying data. After moving the bits from 'ptr' into member attributes
    // 'd_data, d_size' defined behavior also requires 'd_size>0' and the memory range '[d_data, d_data+d_size)' is
    // valid, contiguous.

  explicit Slice(const void *ptr);
    // Create a Slice from specified 'ptr'. Behavior is defined provided the upper 16 bits of 'ptr' are the slice's
    // size and the lower 48 bits point to the underlying data. After moving the bits from 'ptr' into member attributes
    // 'd_data, d_size' defined behavior also requires 'd_size>0' and the memory range '[d_data, d_data+d_size)' is
    // valid, contiguous.

  Slice(const Slice& other) = default;
    // Copy constructor

  ~Slice() = default;
    // Destroy this object. 'd_data' not freed/destroyed because it is not owned.

  // ACCESSORS
  ssize size() const;
    // Return 'size' attribute

  const T *data() const;
    // Return 'data' attribute

  explicit operator const void*() const;
    // Cast the underlying representation into a 'const void*' so that the upper 16 bits holds the size
    // and lower 48 bits holds the pointer to the underlying data

  explicit operator void*();
    // Cast the underlying representation into a 'void*' so that the upper 16 bits holds the size and lower
    // 48 bits holds the pointer to the underlying data

  bool equal(void *ptr);
    // Return true of the Slice at 'ptr' is equal to this and false otherwise. 'ptr' encodes a pointer to a
    // a Slice per the Slice constructor taking 'void*' and with the same requirements for defined behavior.

  explicit operator bool();

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
: d_opaque(0)
{
}
 
template<class T>
inline
Slice<T>::Slice(T *data, ssize size)
: d_data(data)
, d_size(size)
{
  assert(data);
  assert(size>0);
  assert(reinterpret_cast<u_int64_t>(data)==(reinterpret_cast<u_int64_t>(data) & 0xFFFFFFFFFFFFULL));
}

template<class T>
inline
Slice<T>::Slice(void *ptr)
: d_opaque(reinterpret_cast<u_int64_t>(ptr))
{
  assert(reinterpret_cast<u_int64_t>(ptr)==(reinterpret_cast<u_int64_t>(ptr) & 0xFFFFFFFFFFFFULL));
  assert(d_size>0);
}

template<class T>
inline
Slice<T>::Slice(const void *ptr)
: d_opaque(reinterpret_cast<u_int64_t>(ptr))
{
  assert(reinterpret_cast<u_int64_t>(ptr)==(reinterpret_cast<u_int64_t>(ptr) & 0xFFFFFFFFFFFFULL));
  assert(d_size>0);
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
  return reinterpret_cast<T*>(d_data);
}

template<class T>
inline
Slice<T>::operator const void*() const {
  return reinterpret_cast<const void*>(d_opaque);
}

template<class T>
inline
Slice<T>::operator void*() {
  return reinterpret_cast<void*>(d_opaque);
}

template<class T>
inline
Slice<T>::operator bool() {
  return d_size;
}

template<class T>
inline
bool Slice<T>::equal(void *ptr) {
  const u_int64_t uptr = reinterpret_cast<u_int64_t>(ptr);
  assert(uptr==(uptr & 0xFFFFFFFFFFFFULL));
  assert(d_size>0);
  if ((uptr >> 48) == d_size) {
    return 0==memcmp(reinterpret_cast<void*>(uptr & 0xFFFFFFFFFFFFULL), reinterpret_cast<void*>(d_data), d_size);
  } else {
    return false;
  }
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
  return reinterpret_cast<T*>(d_data);
}

template<class T>
inline
void Slice<T>::reset(ssize sz, T* data) {
  assert(reinterpret_cast<u_int64_t>(data)==(reinterpret_cast<u_int64_t>(data) & 0xFFFFFFFFFFFFULL));
  d_data = reinterpret_cast<u_int64_t>(data);
  d_size = sz;
}

// ASPECTS
template<>
inline
void Slice<char>::print() const {
  if (d_data!=0) {
    printf("Slice<char> size: %u, data: '", d_size);
    const char *start = reinterpret_cast<const char*>(d_data);
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

typedef Slice<char>           Key;
typedef Slice<unsigned char> UKey;

} // namespace Benchmark
