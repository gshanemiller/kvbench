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
  u_int64_t d_opaque;

public:
  // CREATORS
  Slice();
    // Create an empty slice having 0 size pointing to address 0

  explicit Slice(const T *data, ssize size);
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

  explicit Slice(u_int64_t word);
    // Create a Slice from specified 'word'. Behavior is defined provided the upper 16 bits of 'word' are the slice's
    // size and the lower 48 bits point to the underlying data. After moving the bits from 'word' into member attributes
    // 'd_data, d_size' defined behavior also requires 'd_size>0' and the memory range '[d_data, d_data+d_size)' is
    // valid, contiguous.

  Slice(const Slice& other) = default;
    // Copy constructor

  ~Slice() = default;
    // Destroy this object. 'd_data' not freed/destroyed because it is not owned.

  // ACCESSORS
  u_int64_t rawValue() const;
    // return the raw, opaque value of this Slice

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

  bool equal(const void *ptr) const;
    // Return true if the Slice at 'ptr' is equal to this and false otherwise. 'ptr' encodes a pointer to a
    // a Slice as per the Slice constructor taking 'void*' with the same defined requirements behavior.

  // MANIPULATORS
  ssize size();
    // Return 'size' attribute

  T *data();
    // Return 'data' attribute

  const T *const_data();
    // Return 'data' attribute

  void reset(const T* data, ssize sz);
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
Slice<T>::Slice(const T *data, ssize size)
: d_opaque((u_int64_t)data | (u_int64_t)size<<48)
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
  assert(reinterpret_cast<u_int64_t>(ptr) & 0xFFFFFFFFFFFFULL);
  assert(reinterpret_cast<u_int64_t>(ptr) >> 48);
}

template<class T>
inline
Slice<T>::Slice(const void *ptr)
: d_opaque(reinterpret_cast<u_int64_t>(ptr))
{
  assert(reinterpret_cast<u_int64_t>(ptr) & 0xFFFFFFFFFFFFULL);
  assert(reinterpret_cast<u_int64_t>(ptr) >> 48);
}

template<class T>
inline
Slice<T>::Slice(u_int64_t word)
: d_opaque(word)
{
  assert((word) & 0xFFFFFFFFFFFFULL);
  assert(word >> 48);
}

// ACCESSORS
template<class T>
inline
u_int64_t Slice<T>::rawValue() const {
  return d_opaque;
}

template<class T>
inline
ssize Slice<T>::size() const {
  return ssize(d_opaque>>48);
}

template<class T>
inline
const T *Slice<T>::data() const {
  return reinterpret_cast<T*>(d_opaque & 0xFFFFFFFFFFFFULL);
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
bool Slice<T>::equal(const void *ptr) const {
  const u_int64_t uptr = reinterpret_cast<u_int64_t>(ptr);
  assert(uptr & 0xFFFFFFFFFFFFULL);
  assert((uptr>>48)>0);
  if ((uptr>>48) == (d_opaque>>48)) {
    return 0==memcmp(reinterpret_cast<void*>(uptr & 0xFFFFFFFFFFFFULL), reinterpret_cast<void*>(d_opaque & 0xFFFFFFFFFFFFULL), uptr>>48);
  } else {
    return false;
  }
}

// MANIPULATORS
template<class T>
inline
ssize Slice<T>::size() {
  return d_opaque>>48;
}

template<class T>
inline
T *Slice<T>::data() {
  return reinterpret_cast<T*>(d_opaque & 0xFFFFFFFFFFFFULL);
}

template<class T>
inline
const T *Slice<T>::const_data() {
  return reinterpret_cast<const T*>(d_opaque & 0xFFFFFFFFFFFFULL);
}

template<class T>
inline
void Slice<T>::reset(const T* data, ssize sz) {
  assert(data);
  assert(sz>0);
  assert(reinterpret_cast<u_int64_t>(data)==(reinterpret_cast<u_int64_t>(data) & 0xFFFFFFFFFFFFULL));
  d_opaque = (u_int64_t)data | (u_int64_t)sz<<48;
}

// ASPECTS
template<>
inline
void Slice<char>::print() const {
  auto size = (d_opaque>>48);
  const char *ptr = reinterpret_cast<const char*>(d_opaque & 0xFFFFFFFFFFFFULL);
  if (ptr) {
    printf("Slice<char> size: %lu, data: '", size);
    for(u_int64_t i=0; i<size; ++i, ++ptr) {
      if (isprint(*ptr)) {
        putchar(*ptr);
      } else {
        printf("0x%02x", *ptr);
      }
    }
    printf("'\n");
  } else {
    printf("Slice<char> size: %lu, data: *NULL*\n", size);
  }
}

template<>
inline
void Slice<unsigned char>::print() const {
  auto size = (d_opaque>>48);
  const char *ptr = reinterpret_cast<const char*>(d_opaque & 0xFFFFFFFFFFFFULL);
  if (ptr) {
    printf("Slice<unsigned char> size: %lu, data: '", size);
    for(u_int64_t i=0; i<size; ++i, ++ptr) {
      if (isprint(*ptr)) {
        putchar(*ptr);
      } else {
        printf("0x%02x", *ptr);
      }
    }
    printf("'\n");
  } else {
    printf("Slice<unsigned char> size: %lu, data: *NULL*\n", size);
  }
}

template<>
inline
void Slice<int>::print() const {
  auto size = (d_opaque>>48);
  const int *ptr = reinterpret_cast<const int*>(d_opaque & 0xFFFFFFFFFFFFULL);
  if (ptr) {
    printf("Slice<int> size: %lu, data: '", size);
    for(u_int64_t i=0; i<size; ++i, ++ptr) {
      if ((*ptr & 0xffffff80)==0) {
        if ((*ptr&0x7f)==0) {
          printf("0x00");
        } else {
          putchar((char)(*ptr&0x7f));
        }
      } else {
        printf("0x%08x", *ptr);
      }
    }
    printf("'\n");
  } else {
    printf("Slice<int> size: %lu, data: *NULL*\n", size);
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
