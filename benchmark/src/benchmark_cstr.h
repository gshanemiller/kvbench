#pragma once

// PURPOSE: Represent C-string
//
// CLASSES:
//  Benchmark::CStr: Hold a pointer to a C-string (zero terminated) with size containing only printable characters.

#include <ctype.h>
#include <assert.h>
#include <string.h>

namespace Benchmark {

class CStr {
  // DATA
  unsigned int  d_size;           // size of string including 0 terminator
  char         *d_data;           // C-string with 0 terminator (not owned)

public:
  // CREATORS
  explicit CStr(unsigned int sz, char *data);
    // Create CStr from specified 'data' of specified 'sz'. Behavior is defined provided 'data' non-zero, 'sz>0',
    // string is zero terminated, memory range '[d_data, d_data+d_size)' is valid, contiguous. Finally the ith char
    // in data must satisfy 'isprint(data[i])' except for the terminating zero.

  CStr(const CStr& other) = default;
    // Copy constructor

  ~CStr() = default;
    // Destroy this object. 'd_data' not freed.

  // ACCESSORS
  unsigned int length() const;
    // Return 'length' of 'd_data' definitionally 'size()-1'

  unsigned int size() const;
    // Return 'size' attribute

  const char *data() const;
    // Return 'data' attribute

  // MANIPULATORS
  unsigned int length();
    // Return 'length' of 'd_data' definitionally 'size()-1'

  unsigned int size();
    // Return 'size' attribute

  char *data();
    // Return 'data' attribute

  CStr& operator=(const CStr& rhs) = default;
    // Assignment operator
};

// FREE OPERATORS
bool operator==(const CStr& lhs, const CStr& rhs);

// INLINE DEFINITIONS
inline
CStr::CStr(unsigned int sz, char *data)
: d_size(sz)
, d_data(data)
{
  assert(sz>0);
  assert(data);
  assert(data[sz-1]==0);
#ifndef NDEBUG
  for (unsigned int i=0; i<d_size-1; ++i) {
    assert(isprint(d_data[i]));
  }
#endif
}

// ACCESSORS
inline
unsigned int CStr::length() const {
  return d_size-1;
}

inline
unsigned int CStr::size() const {
  return d_size;
}

inline
const char *CStr::data() const {
  return d_data;
}

// MANIPULATORS
inline
unsigned int CStr::length() {
  return d_size-1;
}

inline
unsigned int CStr::size() {
  return d_size;
}

inline
char *CStr::data() {
  return d_data;
}

// FREE OPERATORS
inline
bool operator==(const CStr& lhs, const CStr& rhs) {
  return (lhs.size()==rhs.size()) ? 0==strcmp(lhs.data(), rhs.data()) : false;
}

} // namespace Benchmark
